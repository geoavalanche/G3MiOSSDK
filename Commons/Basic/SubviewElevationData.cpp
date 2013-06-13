//
//  SubviewElevationData.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 2/21/13.
//
//

#include "SubviewElevationData.hpp"

#include "IStringBuilder.hpp"
#include "IFloatBuffer.hpp"
#include "IFactory.hpp"
#include "Vector3D.hpp"
#include "Vector2I.hpp"

SubviewElevationData::SubviewElevationData(const ElevationData* elevationData,
                                           //bool ownsElevationData,
                                           const Sector& sector,
                                           const Vector2I& extent,
                                           bool useDecimation) :
ElevationData(sector, extent)
//_realResolution( elevationData->getRealResolution() )
{
  if ((elevationData == NULL) ||
      (elevationData->getExtentWidth() < 1) ||
      (elevationData->getExtentHeight() < 1)) {
    ILogger::instance()->logError("SubviewElevationData can't subview given elevation data.");
    _buffer = NULL;
    return;
  }

  _hasNoData = false;
  if (useDecimation) {
    _buffer = createDecimatedBuffer( elevationData );
  }
  else {
    _buffer = createInterpolatedBuffer( elevationData );
  }
  
  //isEquivalentTo(elevationData);
}

const Vector2D SubviewElevationData::getParentXYAt(const ElevationData* elevationData,
                                                   const Geodetic2D& position) const {
  const Sector parentSector = elevationData->getSector();
  const Geodetic2D parentLower = parentSector.lower();

  const double parentX = (
                          ( position.longitude().radians() - parentLower.longitude().radians() )
                          / parentSector.getDeltaLongitude().radians()
                          * elevationData->getExtentWidth()
                          );

  const double parentY = (
                          ( position.latitude().radians() - parentLower.latitude().radians() )
                          / parentSector.getDeltaLatitude().radians()
                          * elevationData->getExtentHeight()
                          );

  return Vector2D(parentX, parentY);
}

double SubviewElevationData::getElevationBoxAt(const ElevationData* elevationData,
                                               double x0, double y0,
                                               double x1, double y1) const {
  const IMathUtils* mu = IMathUtils::instance();

  const double floorY0 = mu->floor(y0);
  const double ceilY1  = mu->ceil(y1);
  const double floorX0 = mu->floor(x0);
  const double ceilX1  = mu->ceil(x1);

  const int parentHeight = elevationData->getExtentHeight();
  const int parentWidth  = elevationData->getExtentWidth();

  if (floorY0 < 0 || ceilY1 >= parentHeight) {
    return 0;
  }
  if (floorX0 < 0 || ceilX1 >= parentWidth) {
    return 0;
  }

  double heightSum = 0;
  double area = 0;

  const double maxX = parentWidth  - 1;
  const double maxY = parentHeight - 1;

  for (double y = floorY0; y <= ceilY1; y++) {
    double ysize = 1.0;
    if (y < y0) {
      ysize *= (1.0 - (y0-y));
    }
    if (y > y1) {
      ysize *= (1.0 - (y-y1));
    }

    const int yy = (int) mu->min(y, maxY);

    for (double x = floorX0; x <= ceilX1; x++) {
      const double height = elevationData->getElevationAt((int) mu->min(x, maxX),
                                                          yy);

      if (mu->isNan(height)) {
        return mu->NanD();
      }

      double size = ysize;
      if (x < x0) {
        size *= (1.0 - (x0-x));
      }
      if (x > x1) {
        size *= (1.0 - (x-x1));
      }

      heightSum += height * size;
      area += size;
    }
  }

  return heightSum/area;
}

IFloatBuffer* SubviewElevationData::createDecimatedBuffer(const ElevationData* elevationData) {
  IFloatBuffer* buffer = IFactory::instance()->createFloatBuffer(_width * _height);

  const Vector2D parentXYAtLower = getParentXYAt(elevationData, _sector.lower());
  const Vector2D parentXYAtUpper = getParentXYAt(elevationData, _sector.upper());
  const Vector2D parentDeltaXY = parentXYAtUpper.sub(parentXYAtLower);

  IMathUtils *mu = IMathUtils::instance();

  for (int x = 0; x < _width; x++) {
    const double u0 = (double) x     / (_width - 1);
    const double u1 = (double) (x+1) / (_width - 1);
    const double x0 = u0 * parentDeltaXY._x + parentXYAtLower._x;
    const double x1 = u1 * parentDeltaXY._x + parentXYAtLower._x;

    for (int y = 0; y < _height; y++) {
      const double v0 = (double) y     / (_height - 1);
      const double v1 = (double) (y+1) / (_height - 1);
      const double y0 = v0 * parentDeltaXY._y + parentXYAtLower._y;
      const double y1 = v1 * parentDeltaXY._y + parentXYAtLower._y;

      const int index = ((_height-1-y) * _width) + x;

      const double height = getElevationBoxAt(elevationData,
                                              x0, y0,
                                              x1, y1);
      buffer->rawPut(index, (float) height);

      if (!_hasNoData) {
        if (mu->isNan(height)){
          _hasNoData = true;
        }
      }
    }
  }

  return buffer;
}

IFloatBuffer* SubviewElevationData::createInterpolatedBuffer(const ElevationData* elevationData) {
  IFloatBuffer* buffer = IFactory::instance()->createFloatBuffer(_width * _height);

  IMathUtils *mu = IMathUtils::instance();

  for (int x = 0; x < _width; x++) {
    const double u = (double) x / (_width - 1);

    const Angle longitude = _sector.getInnerPointLongitude(u);

    for (int y = 0; y < _height; y++) {
      const double v = 1.0 - ( (double) y / (_height - 1) );

      const Angle latitude = _sector.getInnerPointLatitude(v);

      const int index = ((_height-1-y) * _width) + x;

      const double height = elevationData->getElevationAt(latitude,
                                                          longitude);

      buffer->rawPut(index, (float) height);

      if (!_hasNoData) {
        if ( mu->isNan(height) ) {
          _hasNoData = true;
        }
      }
    }
  }

  return buffer;
}

SubviewElevationData::~SubviewElevationData() {
  //  if (_ownsElevationData) {
//  delete _elevationData;
  //  }
  delete _buffer;
}

double SubviewElevationData::getElevationAt(int x, int y) const {

  //  if (_buffer != NULL) {
  const int index = ((_height-1-y) * _width) + x;

  if ( (index < 0) || (index >= _buffer->size()) ) {
    printf("break point on me\n");
    return IMathUtils::instance()->NanD();
  }

  return _buffer->get(index);
  //  }
  //
  //
  //  const double u = (double) x / (_width - 1);
  //  const double v = (double) y / (_height - 1);
  //  const Geodetic2D position = _sector.getInnerPoint(u, v);
  //
  //  return getElevationAt(position.latitude(),
  //                        position.longitude());
}

const std::string SubviewElevationData::description(bool detailed) const {
  IStringBuilder *isb = IStringBuilder::newStringBuilder();
  isb->addString("(SubviewElevationData extent=");
  isb->addInt(_width);
  isb->addString("x");
  isb->addInt(_height);
  isb->addString(" sector=");
  isb->addString( _sector.description() );
  if (detailed) {
    isb->addString("\n");
    for (int row = 0; row < _width; row++) {
      //isb->addString("   ");
      for (int col = 0; col < _height; col++) {
        isb->addDouble( getElevationAt(col, row) );
        isb->addString(",");
      }
      isb->addString("\n");
    }
  }
  isb->addString(")");
  const std::string s = isb->getString();
  delete isb;
  return s;

}

Vector3D SubviewElevationData::getMinMaxAverageElevations() const {
  const IMathUtils* mu = IMathUtils::instance();

  double minHeight = mu->maxDouble();
  double maxHeight = mu->minDouble();
  double sumHeight = 0.0;

  for (int x = 0; x < _width; x++) {
    for (int y = 0; y < _height; y++) {
      const double height = getElevationAt(x, y);
      if ( !mu->isNan(height) ) {
        if (height < minHeight) {
          minHeight = height;
        }
        if (height > maxHeight) {
          maxHeight = height;
        }
        sumHeight += height;
      }
    }
  }

  if (minHeight == mu->maxDouble()) {
    minHeight = 0;
  }
  if (maxHeight == mu->minDouble()) {
    maxHeight = 0;
  }

  return Vector3D(minHeight,
                  maxHeight,
                  sumHeight / (_width * _height));
}

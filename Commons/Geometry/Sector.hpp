//
//  Sector.hpp
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 12/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_Sector_h
#define G3MiOSSDK_Sector_h

#include <vector>
#include <math.h>

#include "Geodetic2D.hpp"
#include "Context.hpp"
#include "Vector2D.hpp"
#include "Geodetic3D.hpp"
#include "Vector3D.hpp"

//class Sector_Geodetic2DCachedData;

class Sector {

private:
  const Geodetic2D _center;

  const Angle _deltaLatitude;
  const Angle _deltaLongitude;
  
  // this lazy value represent the half diagonal of the sector, measured in radians
  // it's stored in double instead of Angle class to optimize performance in android
  // this value is only used in the method Sector::isBackOriented
  mutable double _deltaRadiusInRadians;

  //const Geodetic2D getClosestPoint(const Geodetic2D& pos) const;

  /*
  // cached values for speed up in isBackOriented()
  mutable Sector_Geodetic2DCachedData* _nwData;
  mutable Sector_Geodetic2DCachedData* _neData;
  mutable Sector_Geodetic2DCachedData* _swData;
  mutable Sector_Geodetic2DCachedData* _seData;
   */
  
  mutable Vector3D* _normalizedCartesianCenter;

public:
  const Geodetic2D _lower;
  const Geodetic2D _upper;


  ~Sector();

  Sector(const Geodetic2D& lower,
         const Geodetic2D& upper) :
  _lower(lower),
  _upper(upper),
  _deltaLatitude(upper._latitude.sub(lower._latitude)),
  _deltaLongitude(upper._longitude.sub(lower._longitude)),
  _center(Angle::midAngle(lower._latitude, upper._latitude),
          Angle::midAngle(lower._longitude, upper._longitude)),
  _deltaRadiusInRadians(-1.0),
  /*
   _nwData(NULL),
  _neData(NULL),
  _swData(NULL),
  _seData(NULL),
   */
  _normalizedCartesianCenter(NULL)
  {
  }


  Sector(const Sector& sector) :
  _lower(sector._lower),
  _upper(sector._upper),
  _deltaLatitude(sector._deltaLatitude),
  _deltaLongitude(sector._deltaLongitude),
  _center(sector._center),
  _deltaRadiusInRadians(sector._deltaRadiusInRadians)
  /*
   _nwData(NULL),
  _neData(NULL),
  _swData(NULL),
  _seData(NULL),
   */
  {
    if (sector._normalizedCartesianCenter==NULL)
      _normalizedCartesianCenter = NULL;
    else {
      const Vector3D* normalizedCartesianCenter = sector._normalizedCartesianCenter;
      _normalizedCartesianCenter = new Vector3D(*normalizedCartesianCenter);
    }
      
  }

  static Sector fromDegrees(double minLat, double minLon, double maxLat, double maxLon){
    const Geodetic2D lower(Angle::fromDegrees(minLat), Angle::fromDegrees(minLon));
    const Geodetic2D upper(Angle::fromDegrees(maxLat), Angle::fromDegrees(maxLon));

    return Sector(lower, upper);
  }

  const Vector2D div(const Sector& that) const;

//  Vector2D getTranslationFactor(const Sector& that) const;

  bool fullContains(const Sector& that) const;

  Sector intersection(const Sector& that) const;

  Sector mergedWith(const Sector& that) const;

  static Sector fullSphere() {
    return Sector(Geodetic2D(Angle::fromDegrees(-90), Angle::fromDegrees(-180)),
                  Geodetic2D(Angle::fromDegrees( 90), Angle::fromDegrees( 180)));
  }

  const Geodetic2D lower() const {
    return _lower;
  }

  const Angle lowerLatitude() const {
    return _lower._latitude;
  }

  const Angle lowerLongitude() const {
    return _lower._longitude;
  }

  const Geodetic2D upper() const {
    return _upper;
  }

  const Angle upperLatitude() const {
    return _upper._latitude;
  }

  const Angle upperLongitude() const {
    return _upper._longitude;
  }

  
  bool contains(const Angle& latitude,
                const Angle& longitude) const;
  
  bool contains(const Geodetic2D& position) const {
    return contains(position._latitude,
                    position._longitude);
  }

  bool contains(const Geodetic3D& position) const {
    return contains(position._latitude,
                    position._longitude);
  }

  bool touchesWith(const Sector& that) const;

  const Angle getDeltaLatitude() const {
    return _deltaLatitude;
  }

  const Angle getDeltaLongitude() const {
    return _deltaLongitude;
  }

  const Geodetic2D getSW() const {
    return _lower;
  }

  const Geodetic2D getNE() const {
    return _upper;
  }

  const Geodetic2D getNW() const {
    return Geodetic2D(_upper._latitude,
                      _lower._longitude);
  }

  const Geodetic2D getSE() const {
    return Geodetic2D(_lower._latitude,
                      _upper._longitude);
  }

  const Geodetic2D getCenter() const {
    return _center;
  }

  // (u,v) are similar to texture coordinates inside the Sector
  // (u,v)=(0,0) in NW point, and (1,1) in SE point
  const Geodetic2D getInnerPoint(double u, double v) const;

  const Angle getInnerPointLongitude(double u) const;
  const Angle getInnerPointLatitude(double v) const;


  const Vector2D getUVCoordinates(const Geodetic2D& point) const {
    return getUVCoordinates(point._latitude, point._longitude);
  }

  Vector2D getUVCoordinates(const Angle& latitude,
                            const Angle& longitude) const {
//    return Vector2D(getUCoordinate(longitude),
//                    getVCoordinate(latitude));
    return Vector2D((longitude._radians        - _lower._longitude._radians) / _deltaLongitude._radians,
                    (_upper._latitude._radians - latitude._radians         ) / _deltaLatitude._radians);
  }

  double getUCoordinate(const Angle& longitude) const {
    return (longitude._radians - _lower._longitude._radians) / _deltaLongitude._radians;
  }

  double getVCoordinate(const Angle& latitude) const {
    return (_upper._latitude._radians - latitude._radians)   / _deltaLatitude._radians;
  }


  bool isBackOriented(const G3MRenderContext *rc,
                      double minHeight,
                      const Planet* planet,
                      const Vector3D& cameraNormalizedPosition,
                      double cameraAngle2HorizonInRadians) const;

  const Geodetic2D clamp(const Geodetic2D& pos) const;

  const std::string description() const;

  Sector* shrinkedByPercentP(float percent) const {
    const Angle deltaLatitude  = _deltaLatitude.times(percent).div(2);
    const Angle deltaLongitude = _deltaLongitude.times(percent).div(2);

    const Geodetic2D delta(deltaLatitude, deltaLongitude);

    return new Sector(_lower.add( delta ),
                      _upper.sub( delta ) );
  }

  Sector shrinkedByPercent(float percent) const {
    const Angle deltaLatitude  = _deltaLatitude.times(percent).div(2);
    const Angle deltaLongitude = _deltaLongitude.times(percent).div(2);

    const Geodetic2D delta(deltaLatitude, deltaLongitude);

    return Sector(_lower.add( delta ),
                  _upper.sub( delta ) );
  }

  bool isEqualsTo(const Sector& that) const {
    return _lower.isEqualsTo(that._lower) && _upper.isEqualsTo(that._upper);
  }
  
  bool touchesNorthPole() const {
    return (_upper._latitude._degrees >= 89.9);
  }
  
  bool touchesSouthPole() const {
    return (_lower._latitude._degrees <= -89.9);
  }

  bool touchesPoles() const {
    return ((_upper._latitude._degrees >=  89.9) ||
            (_lower._latitude._degrees <= -89.9));
  }

  double getDeltaRadiusInRadians() const {
    if (_deltaRadiusInRadians < 0)
      _deltaRadiusInRadians = IMathUtils::instance()->sqrt(_deltaLatitude.radians()  * _deltaLatitude.radians() +
                                   _deltaLongitude.radians() * _deltaLongitude.radians()) * 0.5;
    return _deltaRadiusInRadians;
  }
  
  const Vector3D getNormalizedCartesianCenter(const Planet* planet) const;


#ifdef JAVA_CODE
  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = (prime * result) + ((_lower == null) ? 0 : _lower.hashCode());
    result = (prime * result) + ((_upper == null) ? 0 : _upper.hashCode());
    return result;
  }


  @Override
  public boolean equals(final Object obj) {
    if (this == obj) {
      return true;
    }
    if (obj == null) {
      return false;
    }
    if (getClass() != obj.getClass()) {
      return false;
    }
    final Sector other = (Sector) obj;
    if (_lower == null) {
      if (other._lower != null) {
        return false;
      }
    }
    else if (!_lower.equals(other._lower)) {
      return false;
    }
    if (_upper == null) {
      if (other._upper != null) {
        return false;
      }
    }
    else if (!_upper.equals(other._upper)) {
      return false;
    }
    return true;
  }
#endif

};


#endif

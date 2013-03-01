//
//  Sector.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 22/06/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#include "Sector.hpp"
#include "Camera.hpp"
#include "Planet.hpp"

#include "IStringBuilder.hpp"

bool Sector::contains(const Angle& latitude,
                      const Angle& longitude) const {
  return
  latitude.isBetween(_lower.latitude(), _upper.latitude()) &&
  longitude.isBetween(_lower.longitude(), _upper.longitude());
}

bool Sector::fullContains(const Sector &s) const {
  return contains(s.upper()) && contains(s.lower());
}

bool Sector::touchesWith(const Sector &that) const {
  // from Real-Time Collision Detection - Christer Ericson
  //   page 79

  // Exit with no intersection if separated along an axis
  if (_upper.latitude().lowerThan(that._lower.latitude()) ||
      _lower.latitude().greaterThan(that._upper.latitude())) {
    return false;
  }
  if (_upper.longitude().lowerThan(that._lower.longitude()) ||
      _lower.longitude().greaterThan(that._upper.longitude())) {
    return false;
  }

  // Overlapping on all axes means Sectors are intersecting
  return true;
}

// (u,v) are similar to texture coordinates inside the Sector
// (u,v)=(0,0) in NW point, and (1,1) in SE point
const Geodetic2D Sector::getInnerPoint(double u, double v) const {
  return Geodetic2D(Angle::linearInterpolation( _lower.latitude(),  _upper.latitude(),  (float) (1.0-v) ),
                    Angle::linearInterpolation( _lower.longitude(), _upper.longitude(), (float)      u  ) );
}

/*
bool Sector::isBackOriented(const G3MRenderContext *rc, double height) const {
  const Camera* camera = rc->getCurrentCamera();
  const Planet* planet = rc->getPlanet();
  
  // compute sector point nearest to camera centerPoint
  const Geodetic2D center = camera->getGeodeticCenterOfView().asGeodetic2D();
  const Vector3D point    = planet->toCartesian(Geodetic3D(getClosestPoint(center), height));
  
  // compute angle between normals
  const Vector3D eye = camera->getCartesianPosition();
  return (planet->geodeticSurfaceNormal(point).dot(eye.sub(point)) <= 0);
}
*/

/*
bool Sector::isBackOriented_v1(const G3MRenderContext *rc) const {
  const Camera* camera = rc->getCurrentCamera();
  const Planet* planet = rc->getPlanet();
  
  // compute sector point nearest to centerPoint
  const Geodetic2D center = camera->getGeodeticCenterOfView().asGeodetic2D();
  const Geodetic2D point = getClosestPoint(center);
  
  // compute angle between normals
  const Vector3D normal = planet->geodeticSurfaceNormal(point);
  const Vector3D view   = camera->getViewDirection().times(-1);
  const double dot = normal.dot(view);
  
  return (dot < 0) ? true : false;
}
*/


bool Sector::isBackOriented(const G3MRenderContext *rc, double height) const {
  const Camera* camera = rc->getCurrentCamera();
  const Planet* planet = rc->getPlanet();
  const Vector3D view = camera->getViewDirection().times(-1);

  // if all the corners normals are back oriented, sector is back oriented
  if (planet->geodeticSurfaceNormal(getNE()).dot(view) > 0) { return false; }
  if (planet->geodeticSurfaceNormal(getNW()).dot(view) > 0) { return false; }
  if (planet->geodeticSurfaceNormal(getSE()).dot(view) > 0) { return false; }
  if (planet->geodeticSurfaceNormal(getSW()).dot(view) > 0) { return false; }
  return true;
}


/*
bool Sector::isBackOriented(const G3MRenderContext *rc, double height) const {
  const Planet*  planet = rc->getPlanet();
  const Vector3D eye = rc->getCurrentCamera()->getCartesianPosition();
  
  // if all the corners normals are back oriented, sector is back oriented
  const Vector3D cartesianNE = planet->toCartesian(Geodetic3D(getNE(), height));
  if (planet->geodeticSurfaceNormal(cartesianNE).dot(eye.sub(cartesianNE)) > 0) { return false; }

  const Vector3D cartesianNW = planet->toCartesian(Geodetic3D(getNW(), height));
  if (planet->geodeticSurfaceNormal(cartesianNW).dot(eye.sub(cartesianNW)) > 0) { return false; }

  const Vector3D cartesianSE = planet->toCartesian(Geodetic3D(getSE(), height));
  if (planet->geodeticSurfaceNormal(cartesianSE).dot(eye.sub(cartesianSE)) > 0) { return false; }

  const Vector3D cartesianSW = planet->toCartesian(Geodetic3D(getSW(), height));
  if (planet->geodeticSurfaceNormal(cartesianSW).dot(eye.sub(cartesianSW)) > 0) { return false; }
  
  return true;
}
*/

Sector Sector::intersection(const Sector& that) const {
  const Angle lowLat = Angle::max(lower().latitude(),  that.lower().latitude());
  const Angle lowLon = Angle::max(lower().longitude(), that.lower().longitude());
  const Geodetic2D low(lowLat, lowLon);

  const Angle upLat = Angle::min(upper().latitude(),  that.upper().latitude());
  const Angle upLon = Angle::min(upper().longitude(), that.upper().longitude());
  const Geodetic2D up(upLat, upLon);

  return Sector(low, up);
}

Sector Sector::mergedWith(const Sector& that) const {
  const Angle lowLat = Angle::min(lower().latitude(),  that.lower().latitude());
  const Angle lowLon = Angle::min(lower().longitude(), that.lower().longitude());
  const Geodetic2D low(lowLat, lowLon);

  const Angle upLat = Angle::max(upper().latitude(),  that.upper().latitude());
  const Angle upLon = Angle::max(upper().longitude(), that.upper().longitude());
  const Geodetic2D up(upLat, upLon);

  return Sector(low, up);
}

const Geodetic2D Sector::getClosestPoint(const Geodetic2D& pos) const
{
  // if pos is included, return pos
  if (contains(pos)) return pos;

  // test longitude
  Geodetic2D center = getCenter();
  double lon        = pos.longitude()._degrees;
  double centerLon  = center.longitude()._degrees;
  double oppLon1    = centerLon - 180;
  double oppLon2    = centerLon + 180;
  if (lon<oppLon1)
    lon+=360;
  if (lon>oppLon2)
    lon-=360;
  double minLon     = _lower.longitude()._degrees;
  double maxLon     = _upper.longitude()._degrees;
  //bool insideLon    = true;
  if (lon < minLon) {
    lon = minLon;
    //insideLon = false;
  }
  if (lon > maxLon) {
    lon = maxLon;
    //insideLon = false;
  }

  // test latitude
  double lat        = pos.latitude()._degrees;
  double minLat     = _lower.latitude()._degrees;
  double maxLat     = _upper.latitude()._degrees;
  //bool insideLat    = true;
  if (lat < minLat) {
    lat = minLat;
    //insideLat = false;
  }
  if (lat > maxLat) {
    lat = maxLat;
    //insideLat = false;
  }

  // here we have to handle the case where sector is close to the pole,
  // and the latitude of the other point must be seen from the other side
  Geodetic2D point(Angle::fromDegrees(lat), Angle::fromDegrees(lon));
  if (touchesNorthPole()) {
    Geodetic2D pole(Angle::fromDegrees(90), Angle::fromDegrees(0));
    Angle angle1 = pos.angleTo(point);
    Angle angle2 = pos.angleTo(pole);
    if (angle1.greaterThan(angle2))
      return pole;
  }
  if (touchesSouthPole()) {
    Geodetic2D pole(Angle::fromDegrees(-90), Angle::fromDegrees(0));
    Angle angle1 = pos.angleTo(point);
    Angle angle2 = pos.angleTo(pole);
    if (angle1.greaterThan(angle2))
      return pole;
  }
  return point;

  /*
   const Angle lat = pos.latitude().nearestAngleInInterval(_lower.latitude(), _upper.latitude());
   const Angle lon = pos.longitude().nearestAngleInInterval(_lower.longitude(), _upper.longitude());
   return Geodetic2D(lat, lon);*/
}

const std::string Sector::description() const {
  IStringBuilder *isb = IStringBuilder::newStringBuilder();
  isb->addString("(Sector ");
  isb->addString(_lower.description());
  isb->addString(" - ");
  isb->addString(_upper.description());
  isb->addString(")");
  const std::string s = isb->getString();
  delete isb;
  return s;  
}

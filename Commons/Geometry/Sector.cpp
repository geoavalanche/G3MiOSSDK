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


bool Sector::contains(const Geodetic2D &position) const {
  return position.isBetween(_lower, _upper);
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
  return Geodetic2D(Angle::lerp( _lower.latitude(),  _upper.latitude(),  (float) (1.0-v) ),
                    Angle::lerp( _lower.longitude(), _upper.longitude(), (float)      u  ) );
}

bool Sector::isBackOriented(const G3MRenderContext *rc) const {
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

Sector Sector::intersection(const Sector& s) const {
  const Angle lowLat = Angle::max(lower().latitude(),  s.lower().latitude());
  const Angle lowLon = Angle::max(lower().longitude(), s.lower().longitude());
  const Geodetic2D low(lowLat, lowLon);
  
  const Angle upLat = Angle::min(upper().latitude(),  s.upper().latitude());
  const Angle upLon = Angle::min(upper().longitude(), s.upper().longitude());
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
  
  // here we have to handle the case where sectos is close to the pole, 
  // and the latitude of the other point must be seen from the other side
  
  
  return Geodetic2D(Angle::fromDegrees(lat), Angle::fromDegrees(lon));


  
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

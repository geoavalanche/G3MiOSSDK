//
//  Ellipsoid.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 31/05/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#include "Ellipsoid.hpp"


Ellipsoid::Ellipsoid(const Vector3D& radii):
_radii(radii),
_radiiSquared(Vector3D(radii.x() * radii.x() ,
                       radii.y() * radii.y(),
                       radii.z() * radii.z())),
_radiiToTheFourth(Vector3D(_radiiSquared.x() * _radiiSquared.x() ,
                           _radiiSquared.y() * _radiiSquared.y(),
                           _radiiSquared.z() * _radiiSquared.z())),
_oneOverRadiiSquared(Vector3D(1.0 / (radii.x() * radii.x() ),
                              1.0 / (radii.y() * radii.y()),
                              1.0 / (radii.z() * radii.z())))
{
  
}


Vector3D Ellipsoid::geodeticSurfaceNormal(const Geodetic3D& geodetic) const {
  double cosLatitude = geodetic.latitude().cosinus();
  
  return Vector3D(cosLatitude * geodetic.longitude().cosinus(),
                  cosLatitude * geodetic.longitude().sinus(),
                  geodetic.latitude().sinus());
}

Vector3D Ellipsoid::geodeticSurfaceNormal(const Geodetic2D& geodetic) const {
  double cosLatitude = geodetic.latitude().cosinus();
  
  return Vector3D(cosLatitude * geodetic.longitude().cosinus(),
                  cosLatitude * geodetic.longitude().sinus(),
                  geodetic.latitude().sinus());
}

std::vector<double> Ellipsoid::intersectionsDistances(const Vector3D& origin,
                                                      const Vector3D& direction) const {
  std::vector<double> intersections;
  
  int __ASK_Normalized_or_not;
  //direction.Normalize();
  
  // By laborious algebraic manipulation....
  const double a = (direction.x() * direction.x() * _oneOverRadiiSquared.x() +
                    direction.y() * direction.y() * _oneOverRadiiSquared.y() +
                    direction.z() * direction.z() * _oneOverRadiiSquared.z());
  
  const double b = 2.0 * (origin.x() * direction.x() * _oneOverRadiiSquared.x() +
                          origin.y() * direction.y() * _oneOverRadiiSquared.y() +
                          origin.z() * direction.z() * _oneOverRadiiSquared.z());
  
  const double c = (origin.x() * origin.x() * _oneOverRadiiSquared.x() +
                    origin.y() * origin.y() * _oneOverRadiiSquared.y() +
                    origin.z() * origin.z() * _oneOverRadiiSquared.z() - 1.0);
  
  // Solve the quadratic equation: ax^2 + bx + c = 0.
  // Algorithm is from Wikipedia's "Quadratic equation" topic, and Wikipedia credits
  // Numerical Recipes in C, section 5.6: "Quadratic and Cubic Equations"
  const double discriminant = b * b - 4 * a * c;
  if (discriminant < 0.0) {
    // no intersections
    return intersections;
  }
  else if (discriminant == 0.0) {
    // one intersection at a tangent point
    //return new double[1] { -0.5 * b / a };
    intersections.push_back(-0.5 * b / a);
    return intersections;
  }
  
  const double t = -0.5 * (b + (b > 0.0 ? 1.0 : -1.0) * sqrt(discriminant));
  const double root1 = t / a;
  const double root2 = c / t;
  
  // Two intersections - return the smallest first.
  if (root1 < root2) {
    intersections.push_back(root1);
    intersections.push_back(root2);
  }
  else {
    intersections.push_back(root2);
    intersections.push_back(root1);
  }
  return intersections;
}


Vector3D Ellipsoid::toVector3D(const Geodetic3D& geodetic) const {
  const Vector3D n = geodeticSurfaceNormal(geodetic);
  const Vector3D k = _radiiSquared.times(n);
  const double gamma = sqrt((k.x() * n.x()) +
                            (k.y() * n.y()) +
                            (k.z() * n.z()));
  
  const Vector3D rSurface = k.div(gamma);
  return rSurface.add(n.times(geodetic.height()));
}


Geodetic2D Ellipsoid::toGeodetic2D(const Vector3D& positionOnEllipsoid) const {
  const Vector3D n = geodeticSurfaceNormal(positionOnEllipsoid);
  
  return Geodetic2D(Angle::fromRadians(asin(n.z())),
                    Angle::fromRadians(atan2(n.y(), n.x())));;
}


Geodetic3D Ellipsoid::toGeodetic3D(const Vector3D& position) const {
  Vector3D p = scaleToGeodeticSurface(position);
  Vector3D h = position.sub(p);
  
  double height;
  if (h.dot(position) < 0) {
    height = -1 * h.length();
  } else {
    height = h.length();
  }
  
  return Geodetic3D(toGeodetic2D(p), height);
}


Vector3D Ellipsoid::scaleToGeodeticSurface(const Vector3D& position) const {
  double beta = 1.0 / sqrt(
                           (position.x() * position.x()) * _oneOverRadiiSquared.x() +
                           (position.y() * position.y()) * _oneOverRadiiSquared.y() +
                           (position.z() * position.z()) * _oneOverRadiiSquared.z());
  
  double n = Vector3D(beta * position.x() * _oneOverRadiiSquared.x(),
                      beta * position.y() * _oneOverRadiiSquared.y(),
                      beta * position.z() * _oneOverRadiiSquared.z()).length();
  
  double alpha = (1.0 - beta) * (position.length() / n);
  
  double x2 = position.x() * position.x();
  double y2 = position.y() * position.y();
  double z2 = position.z() * position.z();
  
  double da = 0.0;
  double db = 0.0;
  double dc = 0.0;
  
  double s = 0.0;
  double dSdA = 1.0;
  
  do {
    alpha -= (s / dSdA);
    
    da = 1.0 + (alpha * _oneOverRadiiSquared.x());
    db = 1.0 + (alpha * _oneOverRadiiSquared.y());
    dc = 1.0 + (alpha * _oneOverRadiiSquared.z());
    
    double da2 = da * da;
    double db2 = db * db;
    double dc2 = dc * dc;
    
    double da3 = da * da2;
    double db3 = db * db2;
    double dc3 = dc * dc2;
    
    s = x2 / (_radiiSquared.x() * da2) +
    y2 / (_radiiSquared.y() * db2) +
    z2 / (_radiiSquared.z() * dc2) - 1.0;
    
    dSdA = -2.0 *
    (x2 / (_radiiToTheFourth.x() * da3) +
     y2 / (_radiiToTheFourth.y() * db3) +
     z2 / (_radiiToTheFourth.z() * dc3));
  }
  while (fabs(s) > 1e-10);
  
  return Vector3D(position.x() / da,
                  position.y() / db,
                  position.z() / dc);
}


Vector3D Ellipsoid::scaleToGeocentricSurface(const Vector3D& position) const {
  double beta = 1.0 / sqrt((position.x() * position.x()) * _oneOverRadiiSquared.x() +
                           (position.y() * position.y()) * _oneOverRadiiSquared.y() +
                           (position.z() * position.z()) * _oneOverRadiiSquared.z());
  
  return position.times(beta);
}


Geodetic2D Ellipsoid::getMidPoint (const Geodetic2D& P0, const Geodetic2D& P1) const
{
  Vector3D v0 = toVector3D(P0);
  Vector3D v1 = toVector3D(P1);
  Vector3D normal = v0.cross(v1).normalized();
  Angle theta = v0.angleBetween(v1);
  Vector3D midPoint = scaleToGeocentricSurface(v0.rotateAroundAxis(normal, theta.times(0.5)));
  return toGeodetic2D(midPoint);
}


std::list<Vector3D> Ellipsoid::computeCurve(const Vector3D& start,
                                            const Vector3D& stop,
                                            double granularity) const {
  if (granularity <= 0.0) {
    //throw new ArgumentOutOfRangeException("granularity", "Granularity must be greater than zero.");
    return std::list<Vector3D>();
  }
  
  Vector3D normal = start.cross(stop).normalized();
  double theta = start.angleBetween(stop).radians();
  
  //int n = max((int)(theta / granularity) - 1, 0);
  int n = ((int) (theta / granularity) - 1) > 0 ? (int) (theta / granularity) - 1 : 0;
  
  std::list<Vector3D> positions;
  
  positions.push_back(start);
  
  for (int i = 1; i <= n; ++i) {
    double phi = (i * granularity);
    
    positions.push_back(scaleToGeocentricSurface(start.rotateAroundAxis(normal, Angle::fromRadians(phi))));
  }
  
  positions.push_back(stop);
  
  return positions;
}


// compute distance from two points
double Ellipsoid::computePreciseLatLonDistance(const Geodetic2D& g1,
                                               const Geodetic2D& g2) const {
  const Vector3D radius = _radii;
  double R = (radius.x() + radius.y() + radius.z()) / 3;
  double medLat = g1.latitude().degrees();
  double medLon = g1.longitude().degrees();
  
  // spheric distance from P to Q
  // this is the right form, but it's the most complex
  // theres is a minimum error considering sphere instead of ellipsoid
  double latP = g2.latitude().radians();
  double lonP = g2.longitude().radians();
  double latQ = medLat / 180 * M_PI, lonQ = medLon / 180 * M_PI;
  double coslatP = cos(latP), sinlatP = sin(latP);
  double coslonP = cos(lonP), sinlonP = sin(lonP);
  double coslatQ = cos(latQ), sinlatQ = sin(latQ);
  double coslonQ = cos(lonQ), sinlonQ = sin(lonQ);
  double pq = coslatP * sinlonP * coslatQ * sinlonQ + sinlatP * sinlatQ + coslatP * coslonP * coslatQ * coslonQ;
  return acos(pq) * R;
}


// compute distance from two points
double Ellipsoid::computeFastLatLonDistance(const Geodetic2D& g1,
                                            const Geodetic2D& g2) const {
  const Vector3D radius = _radii;
  double R = (radius.x() + radius.y() + radius.z()) / 3;
  
  double medLat = g1.latitude().degrees();
  double medLon = g1.longitude().degrees();
  
  // this way is faster, and works properly further away from the poles
  //double diflat = fabs(g.latitude()-medLat);
  double diflat = fabs(g2.latitude().degrees() - medLat);
  if (diflat > 180) diflat = 360 - diflat;
  double diflon = fabs(g2.longitude().degrees() - medLon);
  if (diflon > 180) diflon = 360 - diflon;
  double dist = sqrt(diflat * diflat + diflon * diflon);
  return dist * M_PI / 180 * R;
}

Vector3D Ellipsoid::closestIntersection(const Vector3D& pos, const Vector3D& ray) const {
  std::vector<double> distances = intersectionsDistances(pos , ray);
  if (distances.empty()) {
    return Vector3D::nan();
  }
  return pos.add(ray.times(distances[0]));
}



Vector3D Ellipsoid::closestPointToSphere(const Vector3D& pos, const Vector3D& ray) const {
  double t = 0;
  
  // compute radius for the rotation
  double R0 = (_radii.x() + _radii.y() + _radii.y()) /3;
  
  // compute the point in this ray that are to a distance R from the origin.
  double U2 = ray.squaredLength();
  double O2 = pos.squaredLength();
  double OU = pos.dot(ray);
  double a = U2;
  double b = 2 * OU;
  double c = O2 - R0 * R0;
  double rad = b * b - 4 * a * c;
  
  // if there is solution, the ray intersects the sphere
  if (rad > 0) {
    // compute the final point (the smaller positive t value)
    t = (-b - sqrt(rad)) / (2 * a);
    if (t < 1) t = (-b + sqrt(rad)) / (2 * a);
    // if the ideal ray intersects, but not the mesh --> case 2
    if (t < 1) rad = -12345;
  }
  
  // if no solution found, find a point in the contour line
  if (rad < 0) {
    double D = sqrt(O2);
    double co2 = R0 * R0 / (D * D);
    double a_ = OU * OU - co2 * O2 * U2;
    double b_ = 2 * OU * O2 - co2 * 2 * OU * O2;
    double c_ = O2 * O2 - co2 * O2 * O2;
    double rad_ = b_ * b_ - 4 * a_ * c_;
    t = (-b_ - sqrt(rad_)) / (2 * a_);
  }
  
  // compute the final point
  Vector3D p = pos.add(ray.times(t));
  return p;
}

//
//  EllipsoidalPlanet.cpp
//  G3MiOSSDK
//
//  Created by Jose Miguel SN on 17/05/13.
//
//

#include <stdio.h>

#include "EllipsoidalPlanet.hpp"
#include "CameraEffects.hpp"


EllipsoidalPlanet::EllipsoidalPlanet(const Ellipsoid& ellipsoid):
_ellipsoid(ellipsoid)
{

}


Vector3D EllipsoidalPlanet::geodeticSurfaceNormal(const Angle& latitude,
                                                  const Angle& longitude) const {
  const double cosLatitude = latitude.cosinus();

  return Vector3D(cosLatitude * longitude.cosinus(),
                  cosLatitude * longitude.sinus(),
                  latitude.sinus());
}

std::vector<double> EllipsoidalPlanet::intersectionsDistances(const Vector3D& origin,
                                                              const Vector3D& direction) const {
  std::vector<double> intersections;

  Vector3D oneOverRadiiSquared = _ellipsoid.getOneOverRadiiSquared();

  // By laborious algebraic manipulation....
  const double a = (direction._x * direction._x * oneOverRadiiSquared._x +
                    direction._y * direction._y * oneOverRadiiSquared._y +
                    direction._z * direction._z * oneOverRadiiSquared._z);

  const double b = 2.0 * (origin._x * direction._x * oneOverRadiiSquared._x +
                          origin._y * direction._y * oneOverRadiiSquared._y +
                          origin._z * direction._z * oneOverRadiiSquared._z);

  const double c = (origin._x * origin._x * oneOverRadiiSquared._x +
                    origin._y * origin._y * oneOverRadiiSquared._y +
                    origin._z * origin._z * oneOverRadiiSquared._z - 1.0);

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

  const double t = -0.5 * (b + (b > 0.0 ? 1.0 : -1.0) * IMathUtils::instance()->sqrt(discriminant));
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

Vector3D EllipsoidalPlanet::toCartesian(const Angle& latitude,
                                        const Angle& longitude,
                                        const double height) const {
  const Vector3D n = geodeticSurfaceNormal(latitude, longitude);

  const Vector3D k = _ellipsoid.getRadiiSquared().times(n);
  const double gamma = IMathUtils::instance()->sqrt((k._x * n._x) +
                                                    (k._y * n._y) +
                                                    (k._z * n._z));

  const Vector3D rSurface = k.div(gamma);
  return rSurface.add(n.times(height));
}

Geodetic2D EllipsoidalPlanet::toGeodetic2D(const Vector3D& positionOnEllipsoidalPlanet) const {
  const Vector3D n = geodeticSurfaceNormal(positionOnEllipsoidalPlanet);

  const IMathUtils* mu = IMathUtils::instance();
  return Geodetic2D(Angle::fromRadians(mu->asin(n._z)),
                    Angle::fromRadians(mu->atan2(n._y, n._x)));
}


Geodetic3D EllipsoidalPlanet::toGeodetic3D(const Vector3D& position) const {
  const Vector3D p = scaleToGeodeticSurface(position);
  const Vector3D h = position.sub(p);

  const double height = (h.dot(position) < 0) ? -1 * h.length() : h.length();

  return Geodetic3D(toGeodetic2D(p), height);
}


Vector3D EllipsoidalPlanet::scaleToGeodeticSurface(const Vector3D& position) const {
  const IMathUtils* mu = IMathUtils::instance();

  const Vector3D oneOverRadiiSquared = _ellipsoid.getOneOverRadiiSquared();
  const Vector3D radiiSquared        = _ellipsoid.getRadiiSquared();
  const Vector3D radiiToTheFourth    = _ellipsoid.getRadiiToTheFourth();

  const double beta = 1.0 / mu->sqrt((position._x * position._x) * oneOverRadiiSquared._x +
                                     (position._y * position._y) * oneOverRadiiSquared._y +
                                     (position._z * position._z) * oneOverRadiiSquared._z);

  const double n = Vector3D(beta * position._x * oneOverRadiiSquared._x,
                            beta * position._y * oneOverRadiiSquared._y,
                            beta * position._z * oneOverRadiiSquared._z).length();

  double alpha = (1.0 - beta) * (position.length() / n);

  const double x2 = position._x * position._x;
  const double y2 = position._y * position._y;
  const double z2 = position._z * position._z;

  double da = 0.0;
  double db = 0.0;
  double dc = 0.0;

  double s = 0.0;
  double dSdA = 1.0;

  do {
    alpha -= (s / dSdA);

    da = 1.0 + (alpha * oneOverRadiiSquared._x);
    db = 1.0 + (alpha * oneOverRadiiSquared._y);
    dc = 1.0 + (alpha * oneOverRadiiSquared._z);

    const double da2 = da * da;
    const double db2 = db * db;
    const double dc2 = dc * dc;

    const double da3 = da * da2;
    const double db3 = db * db2;
    const double dc3 = dc * dc2;

    s = (x2 / (radiiSquared._x * da2) +
         y2 / (radiiSquared._y * db2) +
         z2 / (radiiSquared._z * dc2) - 1.0);

    dSdA = (-2.0 *
            (x2 / (radiiToTheFourth._x * da3) +
             y2 / (radiiToTheFourth._y * db3) +
             z2 / (radiiToTheFourth._z * dc3)));
  }
  while (mu->abs(s) > 1e-10);

  return Vector3D(position._x / da,
                  position._y / db,
                  position._z / dc);
}


Vector3D EllipsoidalPlanet::scaleToGeocentricSurface(const Vector3D& position) const {
  Vector3D oneOverRadiiSquared = _ellipsoid.getOneOverRadiiSquared();

  const double beta = 1.0 / IMathUtils::instance()->sqrt((position._x * position._x) * oneOverRadiiSquared._x +
                                                         (position._y * position._y) * oneOverRadiiSquared._y +
                                                         (position._z * position._z) * oneOverRadiiSquared._z);

  return position.times(beta);
}


Geodetic2D EllipsoidalPlanet::getMidPoint (const Geodetic2D& P0,
                                           const Geodetic2D& P1) const {
  const Vector3D v0 = toCartesian(P0);
  const Vector3D v1 = toCartesian(P1);
  const Vector3D normal = v0.cross(v1).normalized();
  const Angle theta = v0.angleBetween(v1);
  const Vector3D midPoint = scaleToGeocentricSurface(v0.rotateAroundAxis(normal, theta.times(0.5)));
  return toGeodetic2D(midPoint);
}


std::list<Vector3D> EllipsoidalPlanet::computeCurve(const Vector3D& start,
                                                    const Vector3D& stop,
                                                    double granularity) const {
  if (granularity <= 0.0) {
    //throw new ArgumentOutOfRangeException("granularity", "Granularity must be greater than zero.");
    return std::list<Vector3D>();
  }

  const Vector3D normal = start.cross(stop).normalized();
  const double theta = start.angleBetween(stop)._radians;

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
double EllipsoidalPlanet::computePreciseLatLonDistance(const Geodetic2D& g1,
                                                       const Geodetic2D& g2) const {
  const IMathUtils* mu = IMathUtils::instance();

  const Vector3D radius = _ellipsoid.getRadii();
  const double R = (radius._x + radius._y + radius._z) / 3;

  // spheric distance from P to Q
  // this is the right form, but it's the most complex
  // theres is a minimum error considering sphere instead of EllipsoidalPlanet
  const double latP = g2._latitude._radians;
  const double lonP = g2._longitude._radians;
  const double latQ = g1._latitude._radians;
  const double lonQ = g1._longitude._radians;
  const double coslatP = mu->cos(latP);
  const double sinlatP = mu->sin(latP);
  const double coslonP = mu->cos(lonP);
  const double sinlonP = mu->sin(lonP);
  const double coslatQ = mu->cos(latQ);
  const double sinlatQ = mu->sin(latQ);
  const double coslonQ = mu->cos(lonQ);
  const double sinlonQ = mu->sin(lonQ);
  const double pq = (coslatP * sinlonP * coslatQ * sinlonQ +
                     sinlatP * sinlatQ +
                     coslatP * coslonP * coslatQ * coslonQ);
  return mu->acos(pq) * R;
}


// compute distance from two points
double EllipsoidalPlanet::computeFastLatLonDistance(const Geodetic2D& g1,
                                                    const Geodetic2D& g2) const {
  const IMathUtils* mu = IMathUtils::instance();

  const Vector3D radius = _ellipsoid.getRadii();
  const double R = (radius._x + radius._y + radius._z) / 3;

  const double medLat = g1._latitude._degrees;
  const double medLon = g1._longitude._degrees;

  // this way is faster, and works properly further away from the poles
  //double diflat = fabs(g._latitude-medLat);
  double diflat = mu->abs(g2._latitude._degrees - medLat);
  if (diflat > 180) {
    diflat = 360 - diflat;
  }

  double diflon = mu->abs(g2._longitude._degrees - medLon);
  if (diflon > 180) {
    diflon = 360 - diflon;
  }

  double dist = mu->sqrt(diflat * diflat + diflon * diflon);
  return dist * mu->pi() / 180 * R;
}


Vector3D EllipsoidalPlanet::closestIntersection(const Vector3D& pos,
                                                const Vector3D& ray) const {
  std::vector<double> distances = intersectionsDistances(pos , ray);
  if (distances.empty()) {
    return Vector3D::nan();
  }
  return pos.add(ray.times(distances[0]));
}


Vector3D EllipsoidalPlanet::closestPointToSphere(const Vector3D& pos, const Vector3D& ray) const {
  const IMathUtils* mu = IMathUtils::instance();

  double t = 0;

  // compute radius for the rotation
  const double R0 = _ellipsoid.getMeanRadius();

  // compute the point in this ray that are to a distance R from the origin.
  const double U2 = ray.squaredLength();
  const double O2 = pos.squaredLength();
  const double OU = pos.dot(ray);
  const double a = U2;
  const double b = 2 * OU;
  const double c = O2 - R0 * R0;
  double rad = b * b - 4 * a * c;

  // if there is solution, the ray intersects the sphere
  if (rad > 0) {
    // compute the final point (the smaller positive t value)
    t = (-b - mu->sqrt(rad)) / (2 * a);
    if (t < 1) t = (-b + mu->sqrt(rad)) / (2 * a);
    // if the ideal ray intersects, but not the mesh --> case 2
    if (t < 1) rad = -12345;
  }

  // if no solution found, find a point in the contour line
  if (rad < 0) {
    const double D = mu->sqrt(O2);
    const double co2 = R0 * R0 / (D * D);
    const double a_ = OU * OU - co2 * O2 * U2;
    const double b_ = 2 * OU * O2 - co2 * 2 * OU * O2;
    const double c_ = O2 * O2 - co2 * O2 * O2;
    const double rad_ = b_ * b_ - 4 * a_ * c_;
    t = (-b_ - mu->sqrt(rad_)) / (2 * a_);
  }

  // compute the final point
  Vector3D result = pos.add(ray.times(t));
  return result;
}

MutableMatrix44D EllipsoidalPlanet::createGeodeticTransformMatrix(const Geodetic3D& position) const {
  const MutableMatrix44D translation = MutableMatrix44D::createTranslationMatrix( toCartesian(position) );
  const MutableMatrix44D rotation    = MutableMatrix44D::createGeodeticRotationMatrix( position );

  return translation.multiply(rotation);
}


void EllipsoidalPlanet::beginSingleDrag(const Vector3D& origin, const Vector3D& initialRay) const
{
  _origin = origin.asMutableVector3D();
  _initialPoint = closestIntersection(origin, initialRay).asMutableVector3D();
  _validSingleDrag = false;
}


MutableMatrix44D EllipsoidalPlanet::singleDrag(const Vector3D& finalRay) const
{
  // test if initialPoint is valid
  if (_initialPoint.isNan()) return MutableMatrix44D::invalid();
  
  // compute final point
  const Vector3D origin = _origin.asVector3D();
  MutableVector3D finalPoint = closestIntersection(origin, finalRay).asMutableVector3D();
  if (finalPoint.isNan()) {
    //printf ("--invalid final point in drag!!\n");
    finalPoint = closestPointToSphere(origin, finalRay).asMutableVector3D();
  }
  
  // compute the rotation axis
  const Vector3D rotationAxis = _initialPoint.cross(finalPoint).asVector3D();
  
  // compute the angle
  double sinus = rotationAxis.length()/_initialPoint.length()/finalPoint.length();
  const Angle rotationDelta = Angle::fromRadians(-IMathUtils::instance()->asin(sinus));
  if (rotationDelta.isNan()) return MutableMatrix44D::invalid();
  
  // save params for possible inertial animations
  _lastDragAxis = rotationAxis.asMutableVector3D();
  double radians = rotationDelta.radians();
  _lastDragRadiansStep = radians - _lastDragRadians;
  _lastDragRadians = radians;
  _validSingleDrag = true;
  
  // return rotation matrix
  return MutableMatrix44D::createRotationMatrix(rotationDelta, rotationAxis);
}


Effect* EllipsoidalPlanet::createEffectFromLastSingleDrag() const
{
  if (!_validSingleDrag || _lastDragAxis.isNan()) return NULL;
  return new RotateWithAxisEffect(_lastDragAxis.asVector3D(), Angle::fromRadians(_lastDragRadiansStep));
}


void EllipsoidalPlanet::beginDoubleDrag(const Vector3D& origin,
                                        const Vector3D& centerRay,
                                      const Vector3D& initialRay0,
                                      const Vector3D& initialRay1) const
{
}


MutableMatrix44D EllipsoidalPlanet::doubleDrag(const Vector3D& finalRay0,
                                             const Vector3D& finalRay1) const
{
  
}



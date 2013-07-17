//
//  Angle.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 31/05/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#ifndef G3MiOSSDK_Angle_hpp
#define G3MiOSSDK_Angle_hpp

#define THRESHOLD               1e-5

#include "IMathUtils.hpp"

#include <string>


class Angle {
private:
  Angle(const double degrees) :
  _degrees( degrees ),
  _radians( degrees / 180.0 * 3.14159265358979323846264338327950288 )
  {
  }

public:
  const double _degrees;
  const double _radians;

  static Angle fromDegrees(double degrees) {
    return Angle(degrees);
  }

  static Angle fromDegreesMinutes(double degrees,
                                  double minutes) {
    return Angle( degrees + ( minutes / 60.0) );
  }

  static Angle fromDegreesMinutesSeconds(double degrees,
                                         double minutes,
                                         double seconds) {
    return Angle( degrees + ( minutes / 60.0) + ( seconds / 3600.0 ) );
  }

  static Angle fromRadians(double radians) {
    return Angle(radians / 3.14159265358979323846264338327950288 * 180.0);
  }

  static Angle min(const Angle& a1,
                   const Angle& a2) {
    return (a1._degrees < a2._degrees) ? a1 : a2;
  }

  static Angle max(const Angle& a1,
                   const Angle& a2) {
    return (a1._degrees > a2._degrees) ? a1 : a2;
  }

  static Angle zero() {
    return Angle::fromDegrees(0);
  }

  static Angle pi() {
    return Angle::fromDegrees(180);
  }
  
  static Angle nan() {
    return Angle::fromDegrees(IMathUtils::instance()->NanD());
  }

  static Angle midAngle(const Angle& angle1, const Angle& angle2) {
    //return Angle::fromDegrees((angle1._degrees + angle2._degrees) / 2);
    return Angle::fromRadians((angle1._radians + angle2._radians) / 2);
  }

  static Angle linearInterpolation(const Angle& from,
                                   const Angle& to,
                                   double alpha) {
    //return Angle::fromDegrees( (1.0-alpha) * from._degrees + alpha * to._degrees );
    return Angle::fromRadians( (1.0-alpha) * from._radians + alpha * to._radians );
  }

  bool isNan() const {
    return IMathUtils::instance()->isNan(_degrees);
  }

  Angle(const Angle& angle):
  _degrees(angle._degrees),
  _radians(angle._radians)
  {

  }

  double sinus() const {
    return IMathUtils::instance()->sin( _radians );
  }

  double cosinus() const {
    return IMathUtils::instance()->cos( _radians );
  }
  
  double tangent() const {
    return IMathUtils::instance()->tan( _radians );
  }

  double degrees() const {
    return _degrees;
  }

  double radians() const {
    return _radians;
  }

  bool closeTo(const Angle& other) const {
    return (IMathUtils::instance()->abs(_degrees - other._degrees) < THRESHOLD);
  }

  Angle add(const Angle& a) const {
    return Angle(_degrees + a._degrees);
  }

  Angle sub(const Angle& a) const {
    return Angle(_degrees - a._degrees);
  }

  Angle times(double k) const {
    return Angle(k * _degrees);
  }

  Angle div(double k) const {
    return Angle(_degrees / k);
  }

  double div(const Angle& k) const {
    return _degrees / k._degrees;
  }

  bool greaterThan(const Angle& a) const {
    return (_degrees > a._degrees);
  }

  bool lowerThan(const Angle& a) const {
    return (_degrees < a._degrees);
  }

  Angle clampedTo(const Angle& min,
                  const Angle& max) const;

  bool isBetween(const Angle& min,
                 const Angle& max) const;

  Angle nearestAngleInInterval(const Angle& min, const Angle& max) const;

  Angle distanceTo(const Angle& other) const;

  Angle normalized() const {
    double degrees = _degrees;
    while (degrees < 0) {
      degrees += 360;
    }
    while (degrees >= 360) {
      degrees -= 360;
    }
    return Angle(degrees);
  }

  bool isZero() const {
    return (_degrees == 0);
  }

  bool isEqualsTo(const Angle& that) const {
    const IMathUtils* mu = IMathUtils::instance();
    return mu->isEquals(_degrees, that._degrees) || mu->isEquals(_radians, that._radians);
  }

#ifdef JAVA_CODE
  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    long temp;
    temp = Double.doubleToLongBits(_radians);
    result = (prime * result) + (int) (temp ^ (temp >>> 32));
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
    final Angle other = (Angle) obj;
    if (Double.doubleToLongBits(_radians) != Double.doubleToLongBits(other._radians)) {
      return false;
    }
    return true;
  }
#endif

  ~Angle() {

  }

  const std::string description() const;

};

#endif

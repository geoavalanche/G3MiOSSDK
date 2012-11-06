//
//  MutableMatrix44D.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 05/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "MutableMatrix44D.hpp"

#include "Frustum.hpp"
#include "MutableVector3D.hpp"

#include "Vector2D.hpp"
#include "IFloatBuffer.hpp"
#include "IFactory.hpp"

MutableMatrix44D& MutableMatrix44D::operator=(const MutableMatrix44D &that) {
  if (this != &that){
    _m00 = that._m00;
    _m01 = that._m01;
    _m02 = that._m02;
    _m03 = that._m03;

    _m10 = that._m10;
    _m11 = that._m11;
    _m12 = that._m12;
    _m13 = that._m13;

    _m20 = that._m20;
    _m21 = that._m21;
    _m22 = that._m22;
    _m23 = that._m23;

    _m30 = that._m30;
    _m31 = that._m31;
    _m32 = that._m32;
    _m33 = that._m33;

    _isValid = that._isValid;

    delete _columnMajorFloatBuffer;
    _columnMajorFloatBuffer = NULL;

    delete [] _columnMajorFloatArray;
    _columnMajorFloatArray = NULL;
  }

  return *this;
}

MutableMatrix44D::~MutableMatrix44D() {
  delete _columnMajorFloatBuffer;
  delete [] _columnMajorFloatArray;
}

const IFloatBuffer* MutableMatrix44D::getColumnMajorFloatBuffer() const {
  if (_columnMajorFloatBuffer == NULL){
    //    _columnMajorFloatBuffer = IFactory::instance()->createFloatBuffer(16);
    //
    //    _columnMajorFloatBuffer->rawPut( 0, (float) _m00);
    //    _columnMajorFloatBuffer->rawPut( 1, (float) _m10);
    //    _columnMajorFloatBuffer->rawPut( 2, (float) _m20);
    //    _columnMajorFloatBuffer->rawPut( 3, (float) _m30);
    //
    //    _columnMajorFloatBuffer->rawPut( 4, (float) _m01);
    //    _columnMajorFloatBuffer->rawPut( 5, (float) _m11);
    //    _columnMajorFloatBuffer->rawPut( 6, (float) _m21);
    //    _columnMajorFloatBuffer->rawPut( 7, (float) _m31);
    //
    //    _columnMajorFloatBuffer->rawPut( 8, (float) _m02);
    //    _columnMajorFloatBuffer->rawPut( 9, (float) _m12);
    //    _columnMajorFloatBuffer->rawPut(10, (float) _m22);
    //    _columnMajorFloatBuffer->rawPut(11, (float) _m32);
    //
    //    _columnMajorFloatBuffer->rawPut(12, (float) _m03);
    //    _columnMajorFloatBuffer->rawPut(13, (float) _m13);
    //    _columnMajorFloatBuffer->rawPut(14, (float) _m23);
    //    _columnMajorFloatBuffer->rawPut(15, (float) _m33);

    _columnMajorFloatBuffer = IFactory::instance()->createFloatBuffer(
                                                                      (float) _m00,
                                                                      (float) _m10,
                                                                      (float) _m20,
                                                                      (float) _m30,

                                                                      (float) _m01,
                                                                      (float) _m11,
                                                                      (float) _m21,
                                                                      (float) _m31,

                                                                      (float) _m02,
                                                                      (float) _m12,
                                                                      (float) _m22,
                                                                      (float) _m32,

                                                                      (float) _m03,
                                                                      (float) _m13,
                                                                      (float) _m23,
                                                                      (float) _m33
                                                                      );
  }
  return _columnMajorFloatBuffer;
}

MutableMatrix44D MutableMatrix44D::multiply(const MutableMatrix44D &that) const {
  const double that00 = that._m00;
  const double that10 = that._m10;
  const double that20 = that._m20;
  const double that30 = that._m30;

  const double that01 = that._m01;
  const double that11 = that._m11;
  const double that21 = that._m21;
  const double that31 = that._m31;

  const double that02 = that._m02;
  const double that12 = that._m12;
  const double that22 = that._m22;
  const double that32 = that._m32;

  const double that03 = that._m03;
  const double that13 = that._m13;
  const double that23 = that._m23;
  const double that33 = that._m33;

  //Rows of this X Columns of that
  const double m00 = (_m00 * that00) + (_m01 * that10) + (_m02 * that20) + (_m03 * that30);
  const double m01 = (_m00 * that01) + (_m01 * that11) + (_m02 * that21) + (_m03 * that31);
  const double m02 = (_m00 * that02) + (_m01 * that12) + (_m02 * that22) + (_m03 * that32);
  const double m03 = (_m00 * that03) + (_m01 * that13) + (_m02 * that23) + (_m03 * that33);

  const double m10 = (_m10 * that00) + (_m11 * that10) + (_m12 * that20) + (_m13 * that30);
  const double m11 = (_m10 * that01) + (_m11 * that11) + (_m12 * that21) + (_m13 * that31);
  const double m12 = (_m10 * that02) + (_m11 * that12) + (_m12 * that22) + (_m13 * that32);
  const double m13 = (_m10 * that03) + (_m11 * that13) + (_m12 * that23) + (_m13 * that33);

  const double m20 = (_m20 * that00) + (_m21 * that10) + (_m22 * that20) + (_m23 * that30);
  const double m21 = (_m20 * that01) + (_m21 * that11) + (_m22 * that21) + (_m23 * that31);
  const double m22 = (_m20 * that02) + (_m21 * that12) + (_m22 * that22) + (_m23 * that32);
  const double m23 = (_m20 * that03) + (_m21 * that13) + (_m22 * that23) + (_m23 * that33);

  const double m30 = (_m30 * that00) + (_m31 * that10) + (_m32 * that20) + (_m33 * that30);
  const double m31 = (_m30 * that01) + (_m31 * that11) + (_m32 * that21) + (_m33 * that31);
  const double m32 = (_m30 * that02) + (_m31 * that12) + (_m32 * that22) + (_m33 * that32);
  const double m33 = (_m30 * that03) + (_m31 * that13) + (_m32 * that23) + (_m33 * that33);

  return MutableMatrix44D(m00, m10, m20, m30,
                          m01, m11, m21, m31,
                          m02, m12, m22, m32,
                          m03, m13, m23, m33);
}

MutableMatrix44D MutableMatrix44D::createProjectionMatrix(const FrustumData& data) {
  return createProjectionMatrix(data._left, data._right,
                                data._bottom, data._top,
                                data._znear, data._zfar);
}

MutableMatrix44D MutableMatrix44D::createProjectionMatrix(double left, double right,
                                                          double bottom, double top,
                                                          double znear, double zfar) {
  // set frustum MutableMatrix44D in double
  const double rl = right - left;
  const double tb = top - bottom;
  const double fn = zfar - znear;

  return MutableMatrix44D(2 * znear / rl, 0, 0, 0,
                          0, 2 * znear / tb, 0, 0,
                          (right + left) / rl, (top + bottom) / tb, -(zfar + znear) / fn, -1,
                          0, 0, -2 * zfar / fn * znear, 0);

}

MutableMatrix44D MutableMatrix44D::transposed() const {
  return MutableMatrix44D(_m00, _m01, _m02, _m03,
                          _m10, _m11, _m12, _m13,
                          _m20, _m21, _m22, _m23,
                          _m30, _m31, _m32, _m33);
}

MutableMatrix44D MutableMatrix44D::inversed() const {
  const double a0 = (_m00 * _m11) - (_m01 * _m10);
  const double a1 = (_m00 * _m12) - (_m02 * _m10);
  const double a2 = (_m00 * _m13) - (_m03 * _m10);
  const double a3 = (_m01 * _m12) - (_m02 * _m11);
  const double a4 = (_m01 * _m13) - (_m03 * _m11);
  const double a5 = (_m02 * _m13) - (_m03 * _m12);

  const double b0 = (_m20 * _m31) - (_m21 * _m30);
  const double b1 = (_m20 * _m32) - (_m22 * _m30);
  const double b2 = (_m20 * _m33) - (_m23 * _m30);
  const double b3 = (_m21 * _m32) - (_m22 * _m31);
  const double b4 = (_m21 * _m33) - (_m23 * _m31);
  const double b5 = (_m22 * _m33) - (_m23 * _m32);

  const double determinant = ((((a0 * b5) - (a1 * b4)) + (a2 * b3) + (a3 * b2)) - (a4 * b1)) + (a5 * b0);

  if (determinant == 0.0) {
    return MutableMatrix44D::invalid();
  }

  const double m00 = (((+_m11 * b5) - (_m12 * b4)) + (_m13 * b3)) / determinant;
  const double m10 = (((-_m10 * b5) + (_m12 * b2)) - (_m13 * b1)) / determinant;
  const double m20 = (((+_m10 * b4) - (_m11 * b2)) + (_m13 * b0)) / determinant;
  const double m30 = (((-_m10 * b3) + (_m11 * b1)) - (_m12 * b0)) / determinant;
  const double m01 = (((-_m01 * b5) + (_m02 * b4)) - (_m03 * b3)) / determinant;
  const double m11 = (((+_m00 * b5) - (_m02 * b2)) + (_m03 * b1)) / determinant;
  const double m21 = (((-_m00 * b4) + (_m01 * b2)) - (_m03 * b0)) / determinant;
  const double m31 = (((+_m00 * b3) - (_m01 * b1)) + (_m02 * b0)) / determinant;
  const double m02 = (((+_m31 * a5) - (_m32 * a4)) + (_m33 * a3)) / determinant;
  const double m12 = (((-_m30 * a5) + (_m32 * a2)) - (_m33 * a1)) / determinant;
  const double m22 = (((+_m30 * a4) - (_m31 * a2)) + (_m33 * a0)) / determinant;
  const double m32 = (((-_m30 * a3) + (_m31 * a1)) - (_m32 * a0)) / determinant;
  const double m03 = (((-_m21 * a5) + (_m22 * a4)) - (_m23 * a3)) / determinant;
  const double m13 = (((+_m20 * a5) - (_m22 * a2)) + (_m23 * a1)) / determinant;
  const double m23 = (((-_m20 * a4) + (_m21 * a2)) - (_m23 * a0)) / determinant;
  const double m33 = (((+_m20 * a3) - (_m21 * a1)) + (_m22 * a0)) / determinant;

  return MutableMatrix44D(m00, m10, m20, m30,
                          m01, m11, m21, m31,
                          m02, m12, m22, m32,
                          m03, m13, m23, m33);
}

void MutableMatrix44D::print(const std::string& name, const ILogger* log) const {
  log->logInfo("MutableMatrix44D %s:\n", name.c_str());
  log->logInfo("%.2f  %.2f %.2f %.2f\n", _m00, _m01,_m02, _m03 );
  log->logInfo("%.2f  %.2f %.2f %.2f\n", _m10, _m11,_m12, _m13 );
  log->logInfo("%.2f  %.2f %.2f %.2f\n", _m20, _m21,_m22, _m23 );
  log->logInfo("%.2f  %.2f %.2f %.2f\n", _m30, _m31,_m32, _m33 );
  log->logInfo("\n");
}

/*
 This function is intended to be used on a ModelView MutableMatrix44D. ModelView = Projection * Model
 */
Vector3D MutableMatrix44D::unproject(const Vector3D& pixel3D,
                                     const int vpLeft,
                                     const int vpTop,
                                     const int vpWidth,
                                     const int vpHeight) const {

  int TODO_Remove_UNPROJECT;//!!!!

  const double winx = pixel3D._x;
  const double winy = pixel3D._y;
  const double winz = pixel3D._z;

  const double in0 = (winx - vpLeft) * 2 / vpWidth - 1.0;
  const double in1 = (winy - vpTop) * 2 / vpHeight - 1.0;
  const double in2 = 2 * winz - 1.0;
  const double in3 = 1.0;

  //Inverse
  MutableMatrix44D m = inversed();

  //To object coordinates

  //Transformating point
  const double out0 = m._m00 * in0 + m._m01 * in1 + m._m02 * in2 + m._m03 * in3;
  const double out1 = m._m10 * in0 + m._m11 * in1 + m._m12 * in2 + m._m13 * in3;
  const double out2 = m._m20 * in0 + m._m21 * in1 + m._m22 * in2 + m._m23 * in3;
  const double out3 = m._m30 * in0 + m._m31 * in1 + m._m32 * in2 + m._m33 * in3;

  if (out3 == 0.0) {
    return Vector3D::nan();
  }

  const double objx = out0 / out3;
  const double objy = out1 / out3;
  const double objz = out2 / out3;

  return Vector3D(objx, objy, objz);
}

Vector2D MutableMatrix44D::project(const Vector3D& point,
                                   const int vpLeft,
                                   const int vpTop,
                                   const int vpWidth,
                                   const int vpHeight) const {
  const double in0 = point._x;
  const double in1 = point._y;
  const double in2 = point._z;
  const double in3 = 1.0;

  //Transformating point
  double out0 = _m00 * in0 + _m01 * in1 + _m02 * in2 + _m03 * in3;
  double out1 = _m10 * in0 + _m11 * in1 + _m12 * in2 + _m13 * in3;
  //double out2 = _m20 * in0 + _m21 * in1 + _m22 * in2 + _m23 * in3;
  const double out3 = _m30 * in0 + _m31 * in1 + _m32 * in2 + _m33 * in3;

  if (out3 == 0.0) {
    return Vector2D::nan();
  }

  out0 /= out3;
  out1 /= out3;
  //out2 /= out3;

  const double winx = vpLeft + (1.0 + out0) * vpWidth / 2.0;
  const double winy = vpTop  + (1.0 + out1) * vpHeight / 2.0;
  //double winz = (1.0 + in2) / 2.0;
  return Vector2D(winx, winy);
}

MutableMatrix44D MutableMatrix44D::createTranslationMatrix(const Vector3D& t) {
  return MutableMatrix44D(1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          t._x, t._y, t._z, 1);

}

MutableMatrix44D MutableMatrix44D::createRotationMatrix(const Angle& angle,
                                                        const Vector3D& axis) {
  const Vector3D a = axis.normalized();

  const double c = angle.cosinus();
  const double s = angle.sinus();

  return MutableMatrix44D(a._x * a._x * (1 - c) + c, a._x * a._y * (1 - c) + a._z * s, a._x * a._z * (1 - c) - a._y * s, 0,
                          a._y * a._x * (1 - c) - a._z * s, a._y * a._y * (1 - c) + c, a._y * a._z * (1 - c) + a._x * s, 0,
                          a._x * a._z * (1 - c) + a._y * s, a._y * a._z * (1 - c) - a._x * s, a._z * a._z * (1 - c) + c, 0,
                          0, 0, 0, 1);
}

MutableMatrix44D MutableMatrix44D::createGeneralRotationMatrix(const Angle& angle,
                                                               const Vector3D& axis, const Vector3D& point) {
  const MutableMatrix44D T1 = MutableMatrix44D::createTranslationMatrix(point.times(-1.0));
  const MutableMatrix44D R  = MutableMatrix44D::createRotationMatrix(angle, axis);
  const MutableMatrix44D T2 = MutableMatrix44D::createTranslationMatrix(point);
  return T2.multiply(R).multiply(T1);
}

MutableMatrix44D MutableMatrix44D::createModelMatrix(const MutableVector3D& pos,
                                                     const MutableVector3D& center,
                                                     const MutableVector3D& up) {
  const MutableVector3D w = center.sub(pos).normalized();
  const double pe = w.dot(up);
  const MutableVector3D v = up.sub(w.times(pe)).normalized();
  const MutableVector3D u = w.cross(v);
  return MutableMatrix44D(u.x(), v.x(), -w.x(), 0,
                          u.y(), v.y(), -w.y(), 0,
                          u.z(), v.z(), -w.z(), 0,
                          -pos.dot(u), -pos.dot(v), pos.dot(w), 1);

}

MutableMatrix44D MutableMatrix44D::createOrthographicProjectionMatrix(double left, double right,
                                                                      double bottom, double top,
                                                                      double znear, double zfar) {
  // set frustum MutableMatrix44D in double
  const double rl = right - left;
  const double tb = top - bottom;
  const double fn = zfar - znear;

  return MutableMatrix44D(2 / rl, 0, 0, 0,
                          0, 2 / tb, 0, 0,
                          0, 0, -2 / fn, 0,
                          -(right+left) / rl, -(top+bottom) / tb, -(zfar+znear) / fn, 1 );
}

MutableMatrix44D MutableMatrix44D::createScaleMatrix(const Vector3D& scale) {
  return MutableMatrix44D(scale._x, 0, 0, 0,
                          0, scale._y, 0, 0,
                          0, 0, scale._z, 0,
                          0, 0, 0, 1);

}

MutableMatrix44D MutableMatrix44D::createGeodeticRotationMatrix(const Angle& latitude,
                                                                const Angle& longitude) {
  // change the reference coordinates system from x-front/y-left/z-up to x-right/y-up/z-back
  const MutableMatrix44D changeReferenceCoordinatesSystem(0, 1, 0, 0,
                                                          0, 0, 1, 0,
                                                          1, 0, 0, 0,
                                                          0, 0, 0, 1);

  // orbit reference system to geodetic position
  const MutableMatrix44D longitudeRotation = MutableMatrix44D::createRotationMatrix(longitude, Vector3D::upY());
  const MutableMatrix44D latitudeRotation  = MutableMatrix44D::createRotationMatrix(latitude,  Vector3D::downX());
  
  return changeReferenceCoordinatesSystem.multiply(longitudeRotation).multiply(latitudeRotation);
}


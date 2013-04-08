//
//  Box.cpp
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 17/07/12.
//  Copyright (c) 2012 Universidad de Las Palmas. All rights reserved.
//

#include "Box.hpp"
#include "Vector2D.hpp"
#include "Camera.hpp"
#include "FloatBufferBuilderFromCartesian3D.hpp"
#include "ShortBufferBuilder.hpp"
#include "IndexedMesh.hpp"
#include "GLConstants.hpp"
#include "Color.hpp"

Box::~Box() {
  delete _mesh;
};

const std::vector<Vector3D> Box::getCorners() const {
#ifdef C_CODE
  const Vector3D c[8] = {
    _lower,
    Vector3D(_lower._x, _lower._y, _upper._z),
    Vector3D(_lower._x, _upper._y, _lower._z),
    Vector3D(_lower._x, _upper._y, _upper._z),
    Vector3D(_upper._x, _lower._y, _lower._z),
    Vector3D(_upper._x, _lower._y, _upper._z),
    Vector3D(_upper._x, _upper._y, _lower._z),
    _upper
  };
  
  return std::vector<Vector3D>(c, c+8);
#endif
#ifdef JAVA_CODE
  if (_cornersD == null) {
    _cornersD = new java.util.ArrayList<Vector3D>(8);
    
    _cornersD.add(_lower);
    _cornersD.add(new Vector3D(_lower._x, _lower._y, _upper._z));
    _cornersD.add(new Vector3D(_lower._x, _upper._y, _lower._z));
    _cornersD.add(new Vector3D(_lower._x, _upper._y, _upper._z));
    _cornersD.add(new Vector3D(_upper._x, _lower._y, _lower._z));
    _cornersD.add(new Vector3D(_upper._x, _lower._y, _upper._z));
    _cornersD.add(new Vector3D(_upper._x, _upper._y, _lower._z));
    _cornersD.add(_upper);
  }
  return _cornersD;
#endif
}

const std::vector<Vector3F> Box::getCornersF() const {
#ifdef C_CODE
  const Vector3F c[8] = {
    Vector3F((float) _lower._x, (float) _lower._y, (float) _lower._z),
    Vector3F((float) _lower._x, (float) _lower._y, (float) _upper._z),
    Vector3F((float) _lower._x, (float) _upper._y, (float) _lower._z),
    Vector3F((float) _lower._x, (float) _upper._y, (float) _upper._z),
    Vector3F((float) _upper._x, (float) _lower._y, (float) _lower._z),
    Vector3F((float) _upper._x, (float) _lower._y, (float) _upper._z),
    Vector3F((float) _upper._x, (float) _upper._y, (float) _lower._z),
    Vector3F((float) _upper._x, (float) _upper._y, (float) _upper._z)
  };

  return std::vector<Vector3F>(c, c+8);
#endif
#ifdef JAVA_CODE
  if (_cornersF == null) {
    _cornersF = new java.util.ArrayList<Vector3F>(8);

    _cornersF.add(new Vector3F((float) _lower._x, (float) _lower._y, (float) _lower._z));
    _cornersF.add(new Vector3F((float) _lower._x, (float) _lower._y, (float) _upper._z));
    _cornersF.add(new Vector3F((float) _lower._x, (float) _upper._y, (float) _lower._z));
    _cornersF.add(new Vector3F((float) _lower._x, (float) _upper._y, (float) _upper._z));
    _cornersF.add(new Vector3F((float) _upper._x, (float) _lower._y, (float) _lower._z));
    _cornersF.add(new Vector3F((float) _upper._x, (float) _lower._y, (float) _upper._z));
    _cornersF.add(new Vector3F((float) _upper._x, (float) _upper._y, (float) _lower._z));
    _cornersF.add(new Vector3F((float) _upper._x, (float) _upper._y, (float) _upper._z));
  }
  return _cornersF;
#endif
}

Vector2I Box::projectedExtent(const G3MRenderContext *rc) const {
  const std::vector<Vector3F> corners = getCornersF();

  const Camera* currentCamera = rc->getCurrentCamera();
  
  const Vector2I pixel0 = currentCamera->point2Pixel(corners[0]);

  int lowerX = pixel0._x;
  int upperX = pixel0._x;
  int lowerY = pixel0._y;
  int upperY = pixel0._y;
  
  const int cornersSize = corners.size();
  for (int i = 1; i < cornersSize; i++) {
    const Vector2I pixel = currentCamera->point2Pixel(corners[i]);
    
    const int x = pixel._x;
    const int y = pixel._y;
    
    if (x < lowerX) { lowerX = x; }
    if (y < lowerY) { lowerY = y; }
    
    if (x > upperX) { upperX = x; }
    if (y > upperY) { upperY = y; }
  }
  
  const int width = upperX - lowerX;
  const int height = upperY - lowerY;
  
  return Vector2I(width, height);
}

double Box::squaredProjectedArea(const G3MRenderContext* rc) const {
  const Vector2I extent = projectedExtent(rc);
  return extent._x * extent._y;
}

bool Box::contains(const Vector3D& p) const {
  const static double margin = 1e-3;
  if (p._x < _lower._x - margin) return false;
  if (p._x > _upper._x + margin) return false;
  
  if (p._y < _lower._y - margin) return false;
  if (p._y > _upper._y + margin) return false;
  
  if (p._z < _lower._z - margin) return false;
  if (p._z > _upper._z + margin) return false;
  
  return true;
}

Vector3D Box::intersectionWithRay(const Vector3D& origin, const Vector3D& direction) const {
  //MIN X
  {
    Plane p( Vector3D(1.0, 0.0, 0.0), _lower._x);
    Vector3D inter = p.intersectionWithRay(origin, direction);
    if (!inter.isNan() && contains(inter)) return inter;
  }
  
  //MAX X
  {
    Plane p( Vector3D(1.0, 0.0, 0.0), _upper._x);
    Vector3D inter = p.intersectionWithRay(origin, direction);
    if (!inter.isNan() && contains(inter)) return inter;
  }
  
  //MIN Y
  {
    Plane p( Vector3D(0.0, 1.0, 0.0), _lower._y);
    Vector3D inter = p.intersectionWithRay(origin, direction);
    if (!inter.isNan() && contains(inter)) return inter;
  }
  
  //MAX Y
  {
    Plane p( Vector3D(0.0, 1.0, 0.0), _upper._y);
    Vector3D inter = p.intersectionWithRay(origin, direction);
    if (!inter.isNan() && contains(inter)) return inter;
  }
  
  //MIN Z
  {
    Plane p( Vector3D(0.0, 0.0, 1.0), _lower._z);
    Vector3D inter = p.intersectionWithRay(origin, direction);
    if (!inter.isNan() && contains(inter)) return inter;
  }
  
  //MAX Z
  {
    Plane p( Vector3D(0.0, 0.0, 1.0), _upper._z);
    Vector3D inter = p.intersectionWithRay(origin, direction);
    if (!inter.isNan() && contains(inter)) return inter;
  }
  
  return Vector3D::nan();  
}


void Box::createMesh(Color* color) {
  
  float v[] = {
    (float) _lower._x, (float) _lower._y, (float) _lower._z,
    (float) _lower._x, (float) _upper._y, (float) _lower._z,
    (float) _lower._x, (float) _upper._y, (float) _upper._z,
    (float) _lower._x, (float) _lower._y, (float) _upper._z,
    (float) _upper._x, (float) _lower._y, (float) _lower._z,
    (float) _upper._x, (float) _upper._y, (float) _lower._z,
    (float) _upper._x, (float) _upper._y, (float) _upper._z,
    (float) _upper._x, (float) _lower._y, (float) _upper._z
  };
  
  short i[] = {
    0, 1, 1, 2, 2, 3, 3, 0,
    1, 5, 5, 6, 6, 2, 2, 1,
    5, 4, 4, 7, 7, 6, 6, 5,
    4, 0, 0, 3, 3, 7, 7, 4,
    3, 2, 2, 6, 6, 7, 7, 3,
    0, 1, 1, 5, 5, 4, 4, 0
  };
  
  FloatBufferBuilderFromCartesian3D vertices(CenterStrategy::firstVertex(),
                                             Vector3D::zero());
  ShortBufferBuilder indices;
  
  const unsigned int numVertices = 8;
  for (unsigned int n=0; n<numVertices; n++) {
    vertices.add(v[n*3], v[n*3+1], v[n*3+2]);
  }
  
  const int numIndices = 48;
  for (unsigned int n=0; n<numIndices; n++) {
    indices.add(i[n]);
  }
  
  _mesh = new IndexedMesh(GLPrimitive::lines(),
                          true,
                          vertices.getCenter(),
                          vertices.create(),
                          indices.create(),
                          1,
                          1,
                          color);
}

void Box::render(const G3MRenderContext* rc,
                 const GLState& parentState, const GPUProgramState* gpuParentProgramState) {
  if (_mesh == NULL) {
    createMesh(Color::newFromRGBA(1.0f, 0.0f, 1.0f, 1.0f));
  }
  _mesh->render(rc, parentState, gpuParentProgramState);
}

bool Box::touchesBox(const Box* box) const {
  if (_lower._x > box->_upper._x) { return false; }
  if (_upper._x < box->_lower._x) { return false; }
  if (_lower._y > box->_upper._y) { return false; }
  if (_upper._y < box->_lower._y) { return false; }
  if (_lower._z > box->_upper._z) { return false; }
  if (_upper._z < box->_lower._z) { return false; }
  return true;
}

Extent* Box::mergedWithBox(const Box* that) const {
  const IMathUtils* mu = IMathUtils::instance();

  const double lowerX = mu->min(_lower._x, that->_lower._x);
  const double lowerY = mu->min(_lower._y, that->_lower._y);
  const double lowerZ = mu->min(_lower._z, that->_lower._z);

  const double upperX = mu->max(_upper._x, that->_upper._x);
  const double upperY = mu->max(_upper._y, that->_upper._y);
  const double upperZ = mu->max(_upper._z, that->_upper._z);

  return new Box(Vector3D(lowerX, lowerY, lowerZ),
                 Vector3D(upperX, upperY, upperZ));
}

bool Box::fullContains(const Extent* that) const {
  return that->fullContainedInBox(this);
}

bool Box::fullContainedInBox(const Box* box) const {
//  return contains(box->_upper) && contains(box->_lower);
  return box->contains(_upper) && box->contains(_lower);
}

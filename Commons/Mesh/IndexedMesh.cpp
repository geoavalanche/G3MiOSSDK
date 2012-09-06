//
//  IndexedMesh.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 22/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>

#include "IndexedMesh.hpp"
#include "Box.hpp"
#include "GL.hpp"
#include "INativeGL.hpp"
#include "IFloatBuffer.hpp"
#include "IIntBuffer.hpp"

IndexedMesh::~IndexedMesh()
{
#ifdef C_CODE
  if (_owner){
    delete _vertices;
    delete _indices;
    if (_colors != NULL) delete _colors;
    if (_flatColor != NULL) delete _flatColor;
  }
  
  if (_extent != NULL) delete _extent;
  
#endif
}

IndexedMesh::IndexedMesh(const GLPrimitive primitive,
                         bool owner,
                         CenterStrategy centerStrategy,
                         Vector3D center,
                         IFloatBuffer* vertices,
                         IIntBuffer* indices,
                         const Color* flatColor,
                         IFloatBuffer* colors,
                         const float colorsIntensity) :
_primitive(primitive),
_owner(owner),
_centerStrategy(centerStrategy),
_center(center),
_vertices(vertices),
_indices(indices),
_flatColor(flatColor),
_colors(colors),
_colorsIntensity(colorsIntensity),
_extent(NULL)
{
  if (centerStrategy != NoCenter) {
    printf ("IndexedMesh array constructor: this center Strategy is not yet implemented\n");
  }
}

void IndexedMesh::render(const RenderContext* rc) const {
  GL *gl = rc->getGL();
  
  gl->enableVerticesPosition();
  
  if (_colors == NULL) {
    gl->disableVertexColor();
  }
  else {
    gl->enableVertexColor(_colors, _colorsIntensity);
  }
  
  if (_flatColor == NULL) {
    gl->disableVertexFlatColor();
  }
  else {
    gl->enableVertexFlatColor(*_flatColor, _colorsIntensity);
  }
  
  gl->vertexPointer(3, 0, _vertices);
  
  if (_centerStrategy != NoCenter) {
    gl->pushMatrix();
    gl->multMatrixf(MutableMatrix44D::createTranslationMatrix(_center));
  }
  
  switch (_primitive) {
    case TriangleStrip:
      gl->drawTriangleStrip(_indices);
      break;
    case Lines:
      gl->drawLines(_indices);
      break;
    case LineLoop:
      gl->drawLineLoop(_indices);
      break;
    case Points:
      gl->drawPoints(_indices);
      break;
    default:
      break;
  }
  
  if (_centerStrategy != NoCenter) {
    gl->popMatrix();
  }
  
  gl->disableVerticesPosition();
}


Extent* IndexedMesh::computeExtent() const {
  
  const int vertexCount = getVertexCount();
  
  if (vertexCount <= 0) {
    return NULL;
  }
  
  double minx=1e10, miny=1e10, minz=1e10;
  double maxx=-1e10, maxy=-1e10, maxz=-1e10;
  
  for (int i=0; i < vertexCount; i++) {
    const int p = i * 3;
    
    const double x = _vertices->get(p  ) + _center.x();
    const double y = _vertices->get(p+1) + _center.y();
    const double z = _vertices->get(p+2) + _center.z();
    
    if (x < minx) minx = x;
    if (x > maxx) maxx = x;
    
    if (y < miny) miny = y;
    if (y > maxy) maxy = y;
    
    if (z < minz) minz = z;
    if (z > maxz) maxz = z;
  }
  
  return new Box(Vector3D(minx, miny, minz), Vector3D(maxx, maxy, maxz));
}

Extent* IndexedMesh::getExtent() const {
  if (_extent == NULL) {
    _extent = computeExtent();
  }
  return _extent;
}

const Vector3D IndexedMesh::getVertex(int i) const {
  const int p = i * 3;
  return Vector3D(_vertices->get(p  ) + _center.x(),
                  _vertices->get(p+1) + _center.y(),
                  _vertices->get(p+2) + _center.z());
}

int IndexedMesh::getVertexCount() const {
  return _vertices->size() / 3;
}

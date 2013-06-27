//
//  IndexedMesh.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 22/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdlib.h>

#include "IndexedMesh.hpp"
#include "GL.hpp"
#include "IShortBuffer.hpp"

IndexedMesh::~IndexedMesh() {
  if (_owner) {
    delete _indices;
  }
}

IndexedMesh::IndexedMesh(const int primitive,
                         bool owner,
                         const Vector3D& center,
                         IFloatBuffer* vertices,
                         IShortBuffer* indices,
                         float lineWidth,
                         float pointSize,
                         Color* flatColor,
                         IFloatBuffer* colors,
                         const float colorsIntensity,
                         bool depthTest) :
AbstractMesh(primitive,
             owner,
             center,
             vertices,
             lineWidth,
             pointSize,
             flatColor,
             colors,
             colorsIntensity,
             depthTest),
_indices(indices)
{

}

//void IndexedMesh::rawRender(const G3MRenderContext* rc) const {
//  GL* gl = rc->getGL();
////  gl->drawElements(_primitive, _indices, _GLGlobalState, *rc->getGPUProgramManager(), &_progState);
//}

<<<<<<< HEAD
void IndexedMesh::rawRender(const G3MRenderContext* rc) const{
=======
void IndexedMesh::rawRender(const G3MRenderContext* rc,
                            const GLState* parentGLState) const{
>>>>>>> 78fb281b001904ef2c62c2a67a6d0dd8b6db004f
  GL* gl = rc->getGL();
  gl->drawElements(_primitive, _indices, &_glState, *rc->getGPUProgramManager());
}

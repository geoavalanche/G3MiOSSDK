//
//  DirectMesh.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 12/1/12.
//
//

#include "DirectMesh.hpp"
#include "GL.hpp"

DirectMesh::DirectMesh(const int primitive,
                       bool owner,
                       const Vector3D& center,
                       IFloatBuffer* vertices,
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
             depthTest)
{
}

void DirectMesh::rawRender(const G3MRenderContext* rc) const {
//  GL* gl = rc->getGL();

//  const int verticesCount = getVertexCount();
//  gl->drawArrays(_primitive, 0, verticesCount, _GLGlobalState, *rc->getGPUProgramManager(), &_progState);
}

void DirectMesh::rawRender(const G3MRenderContext* rc, GLState* parentGLState) const{
  GL* gl = rc->getGL();
  
  const int verticesCount = getVertexCount();
  gl->drawArrays(_primitive, 0, verticesCount, parentGLState, *rc->getGPUProgramManager());
}


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
                       const float colorsIntensity) :
AbstractMesh(primitive,
             owner,
             center,
             vertices,
             lineWidth,
             pointSize,
             flatColor,
             colors,
             colorsIntensity)
{
}

void DirectMesh::rawRender(const G3MRenderContext* rc,
                           const GLState& parentState, const GPUProgramState* gpuParentProgramState) const {
  GL* gl = rc->getGL();

  const int verticesCount = getVertexCount();
  gl->drawArrays(_primitive, 0, verticesCount, parentState, *rc->getGPUProgramManager(), gpuParentProgramState);
}

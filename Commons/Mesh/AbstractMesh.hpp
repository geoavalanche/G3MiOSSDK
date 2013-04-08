//
//  AbstractMesh.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 12/1/12.
//
//

#ifndef __G3MiOSSDK__AbstractMesh__
#define __G3MiOSSDK__AbstractMesh__

#include "Mesh.hpp"

class MutableMatrix44D;
class IFloatBuffer;
class Color;
#include "Vector3D.hpp"

class AbstractMesh : public Mesh {
protected:
  const int                _primitive;
  const bool               _owner;
  Vector3D                 _center;
  const MutableMatrix44D*  _translationMatrix;
  IFloatBuffer*            _vertices;
  Color*                   _flatColor;
  IFloatBuffer*            _colors;
  const float              _colorsIntensity;
  const float              _lineWidth;
  const float              _pointSize;

  mutable Extent* _extent;
  Extent* computeExtent() const;

  AbstractMesh(const int primitive,
               bool owner,
               const Vector3D& center,
               IFloatBuffer* vertices,
               float lineWidth,
               float pointSize,
               Color* flatColor,
               IFloatBuffer* colors,
               const float colorsIntensity);

  virtual void rawRender(const G3MRenderContext* rc,
                         const GLState& parentState,
                         const GPUProgramState* gpuParentProgramState) const = 0;

public:
  ~AbstractMesh();

  void render(const G3MRenderContext* rc,
              const GLState& parentState,
              const GPUProgramState* gpuParentProgramState) const;

  Extent* getExtent() const;

  int getVertexCount() const;

  const Vector3D getVertex(int i) const;

  bool isTransparent(const G3MRenderContext* rc) const;
  
};

#endif

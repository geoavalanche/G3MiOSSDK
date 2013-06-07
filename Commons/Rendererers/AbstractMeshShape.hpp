//
//  AbstractMeshShape.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/5/12.
//
//

#ifndef __G3MiOSSDK__AbstractMeshShape__
#define __G3MiOSSDK__AbstractMeshShape__

#include "Shape.hpp"

#include "GLClient.hpp"

class Mesh;

class AbstractMeshShape : public Shape {
private:
  Mesh* _mesh;

protected:
  virtual Mesh* createMesh(const G3MRenderContext* rc) = 0;

  Mesh* getMesh(const G3MRenderContext* rc);

  void cleanMesh();

  AbstractMeshShape(Geodetic3D* position) :
  Shape(position),
  _mesh(NULL) {

  }

  AbstractMeshShape(Geodetic3D* position,
                    Mesh* mesh) :
  Shape(position),
  _mesh(mesh) {

  }

public:

  bool isReadyToRender(const G3MRenderContext* rc);

  void rawRender(const G3MRenderContext* rc, GLState* parentGLState);

  virtual ~AbstractMeshShape();

  bool isTransparent(const G3MRenderContext* rc);
  
  void notifyGLClientChildrenParentHasChanged();
};

#endif

//
//  MeshShape.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/5/12.
//
//

#ifndef __G3MiOSSDK__MeshShape__
#define __G3MiOSSDK__MeshShape__

#include "Shape.hpp"
class Mesh;

class MeshShape : public Shape {
private:
  Mesh* _mesh;

protected:
  virtual Mesh* createMesh(const RenderContext* rc) {
    return NULL;
  }

  Mesh* getMesh(const RenderContext* rc);

  void cleanMesh();

public:
  MeshShape(Geodetic3D* position) :
  Shape(position),
  _mesh(NULL) {

  }
  
  MeshShape(Geodetic3D* position,
            Mesh* mesh) :
  Shape(position),
  _mesh(mesh) {

  }

  bool isReadyToRender(const RenderContext* rc);

  void rawRender(const RenderContext* rc);

  virtual ~MeshShape();

};

#endif

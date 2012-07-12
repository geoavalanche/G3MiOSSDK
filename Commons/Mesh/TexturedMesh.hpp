//
//  TexturedMesh.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 12/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_TexturedMesh_hpp
#define G3MiOSSDK_TexturedMesh_hpp

#include "Mesh.hpp"

#include "TextureMapping.hpp"

class TexturedMesh: public Mesh
{
private:
  const Mesh* _mesh;
  const TextureMapping* _texMap;
  
public:
  
  TexturedMesh(const Mesh* mesh, const TextureMapping* tm): _mesh(mesh), _texMap(tm){}
  
  ~TexturedMesh(){
    delete _mesh;
    delete _texMap;
  }
  
  void render(const RenderContext* rc) const;
};

#endif

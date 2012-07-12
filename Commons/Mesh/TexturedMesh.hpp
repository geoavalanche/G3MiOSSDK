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
  const Mesh*           _mesh;
  const TextureMapping* _textureMapping;
  const bool            _ownedMesh;
  
public:
  
  TexturedMesh(const Mesh* mesh,
               bool ownedMesh,
               const TextureMapping* textureMapping) :
  _mesh(mesh),
  _ownedMesh(ownedMesh),
  _textureMapping(textureMapping)
  {
    
  }
  
  ~TexturedMesh(){
    if (_ownedMesh) {
      delete _mesh;
    }
    delete _textureMapping;
  }
  
  void render(const RenderContext* rc) const;
};

#endif

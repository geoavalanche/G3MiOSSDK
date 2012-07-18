//
//  TextureMapping.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 12/07/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#include "TextureMapping.hpp"

#include "Context.hpp"


TextureMapping::TextureMapping(int textureId, std::vector<MutableVector2D> texCoords) :
_textureId(textureId)
{
  float* texCoordsA = new float[2 * texCoords.size()];
  int p = 0;
  for (int i = 0; i < texCoords.size(); i++) {
    texCoordsA[p++] = texCoords[i].x();
    texCoordsA[p++] = texCoords[i].y();
  }
  _texCoords = texCoordsA;
}

void TextureMapping::bind(const RenderContext* rc) const {
  IGL *gl = rc->getGL();
  
  gl->bindTexture(_textureId);
  gl->setTextureCoordinates(2, 0, _texCoords);
}
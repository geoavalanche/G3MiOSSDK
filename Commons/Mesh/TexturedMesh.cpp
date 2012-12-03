//
//  TexturedMesh.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 12/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "TexturedMesh.hpp"

#include "GL.hpp"

void TexturedMesh::render(const G3MRenderContext* rc,
                          const GLState& parentState) const {
  //GL* gl = rc->getGL();

  //gl->enableTextures();
  //gl->enableTexture2D();

  GLState state(parentState);
  state.enableTextures();
  state.enableTexture2D();
  if (_transparent) {
    state.enableBlend();
  }

  _textureMapping->bind(rc);

  _mesh->render(rc, state);

  //gl->disableTexture2D();
  //gl->disableTextures();
}

//
//  MultiTextureMapping.cpp
//  G3MiOSSDK
//
//  Created by Jose Miguel SN on 24/12/13.
//
//

#include "MultiTextureMapping.hpp"

#include "ILogger.hpp"
#include "IFloatBuffer.hpp"
#include "GLState.hpp"


void MultiTextureMapping::releaseGLTextureId() {

  if (_glTextureId != NULL) {
#ifdef C_CODE
    delete _glTextureId;
#endif
#ifdef JAVA_CODE
    _glTextureId.dispose();
#endif
    _glTextureId = NULL;
  } else{
    ILogger::instance()->logError("Releasing invalid Multi texture mapping");
  }
}

MultiTextureMapping::~MultiTextureMapping() {
  if (_ownedTexCoords) {
    delete _texCoords;
  }

  releaseGLTextureId();

#ifdef JAVA_CODE
  super.dispose();
#endif
}

void MultiTextureMapping::modifyGLState(GLState& state) const{
  if (_texCoords == NULL) {
    ILogger::instance()->logError("MultiTextureMapping::bind() with _texCoords == NULL");
  }
  else {
    state.clearGLFeatureGroup(COLOR_GROUP);

    if ((_scaleU != 1) ||
        (_scaleV != 1) ||
        (_translationU != 0) ||
        (_translationV != 0) ||
        (_rotationInRadians != 0)) {
      state.addGLFeature(new TextureGLFeature(_glTextureId->getID(),
                                              _texCoords,
                                              2,
                                              0,
                                              false,
                                              0,
                                              _transparent,
                                              GLBlendFactor::srcAlpha(),
                                              GLBlendFactor::oneMinusSrcAlpha(),
                                              _translationU,
                                              _translationV,
                                              _scaleU,
                                              _scaleV,
                                              _rotationInRadians,
                                              _rotationCenterU,
                                              _rotationCenterV),
                         false);
    }
    else {
      state.addGLFeature(new TextureGLFeature(_glTextureId->getID(),
                                              _texCoords,
                                              2,
                                              0,
                                              false,
                                              0,
                                              _transparent,
                                              GLBlendFactor::srcAlpha(),
                                              GLBlendFactor::oneMinusSrcAlpha()),
                         false);
    }
  }
}

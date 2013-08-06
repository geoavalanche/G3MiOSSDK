//
//  BusyQuadRenderer.cpp
//  G3MiOSSDK
//
//  Created by Agustin Trujillo Pino on 13/08/12.
//  Copyright (c) 2012 Universidad de Las Palmas. All rights reserved.
//

#include <OpenGLES/ES2/gl.h>


#include "BusyQuadRenderer.hpp"

#include "Context.hpp"
#include "GL.hpp"
#include "MutableMatrix44D.hpp"
#include "TexturesHandler.hpp"
#include "TextureMapping.hpp"
#include "TexturedMesh.hpp"

#include "FloatBufferBuilderFromCartesian3D.hpp"
#include "FloatBufferBuilderFromCartesian2D.hpp"
#include "ShortBufferBuilder.hpp"

#include "GLConstants.hpp"
#include "GPUProgram.hpp"
#include "Camera.hpp"

void BusyQuadRenderer::start(const G3MRenderContext* rc) {
  if (_animated) {
    Effect *effect = new BusyEffect(this);
    rc->getEffectsScheduler()->startEffect(effect, this);
  }
}

void BusyQuadRenderer::stop(const G3MRenderContext* rc) {
  if (_animated) {
    rc->getEffectsScheduler()->cancelAllEffectsFor(this);
  }
}


bool BusyQuadRenderer::initMesh(const G3MRenderContext* rc) {
  //TEXTURED
#ifdef C_CODE
  const IGLTextureId* texId = NULL;
#endif
#ifdef JAVA_CODE
  IGLTextureId texId = null;
#endif
//  IImage* image = rc->getFactory()->createImageFromFileName(_textureFilename);

  texId = rc->getTexturesHandler()->getGLTextureId(_image,
                                                   GLFormat::rgba(),
                                                   "BusyQuadRenderer-Texture",
                                                   false);

  rc->getFactory()->deleteImage(_image);
  _image = NULL;

  if (texId == NULL) {
    rc->getLogger()->logError("Can't upload texture to GPU");
    return false;
  }

  const double halfWidth = _size._x / 2;
  const double hadfHeight = _size._y / 2;
  FloatBufferBuilderFromCartesian3D vertices(CenterStrategy::noCenter(), Vector3D::zero());
  vertices.add(-halfWidth, +hadfHeight, 0);
  vertices.add(-halfWidth, -hadfHeight, 0);
  vertices.add(+halfWidth, +hadfHeight, 0);
  vertices.add(+halfWidth, -hadfHeight, 0);

  FloatBufferBuilderFromCartesian2D texCoords;
  texCoords.add(0, 0);
  texCoords.add(0, 1);
  texCoords.add(1, 0);
  texCoords.add(1, 1);

  DirectMesh *im = new DirectMesh(GLPrimitive::triangleStrip(),
                                  true,
                                  vertices.getCenter(),
                                  vertices.create(),
                                  1,
                                  1);

  TextureMapping* texMap = new SimpleTextureMapping(texId,
                                                    texCoords.create(),
                                                    true,
                                                    false);

  _quadMesh = new TexturedMesh(im, true, texMap, true, true);

  return true;
}

//TODO: REMOVE???
void BusyQuadRenderer::render(const G3MRenderContext* rc) {
  GL* gl = rc->getGL();

  if (_quadMesh == NULL){
    if (!initMesh(rc)) {
      return;
    }
  }

  createGLState();
  
  // clear screen
  gl->clearScreen(*_backgroundColor);

  // draw mesh
  _quadMesh->render(rc, &_glState);
}

void BusyQuadRenderer::createGLState(){
  
  //Modelview and projection
  _modelviewMatrix = MutableMatrix44D::createRotationMatrix(Angle::fromDegrees(_degrees), Vector3D(0, 0, 1));
  _glState.clearGLFeatureGroup(CAMERA_GROUP);
  _glState.addGLFeature(new ProjectionGLFeature(_projectionMatrix.asMatrix44D()), false);
  _glState.addGLFeature(new ModelGLFeature(_modelviewMatrix.asMatrix44D()), false);
}

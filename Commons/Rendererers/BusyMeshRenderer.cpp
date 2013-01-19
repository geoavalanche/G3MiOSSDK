//
//  BusyMeshRenderer.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 20/07/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//


#include <OpenGLES/ES2/gl.h>


#include "BusyMeshRenderer.hpp"

#include "Context.hpp"
#include "GL.hpp"
#include "MutableMatrix44D.hpp"
#include "IMathUtils.hpp"
#include "FloatBufferBuilderFromColor.hpp"
#include "FloatBufferBuilderFromCartesian3D.hpp"
#include "ShortBufferBuilder.hpp"
#include "GLConstants.hpp"

void BusyMeshRenderer::initialize(const G3MContext* context)
{
  unsigned int numStrides = 60;

  FloatBufferBuilderFromCartesian3D vertices(CenterStrategy::noCenter(), Vector3D::zero());
  FloatBufferBuilderFromColor colors;
  ShortBufferBuilder indices;

  int indicesCounter=0;
  const float r1=12;
  const float r2=18;
  for (int step=0; step<=numStrides; step++) {
    const double angle = (double) step * 2 * GMath.pi() / numStrides;
    const double c = GMath.cos(angle);
    const double s = GMath.sin(angle);

    vertices.add( (r1 * c), (r1 * s), 0);
    vertices.add( (r2 * c), (r2 * s), 0);

    indices.add((short) (indicesCounter++));
    indices.add((short) (indicesCounter++));

    float col = (float) (1.1 * step / numStrides);
    if (col>1) {
      colors.add(255, 255, 255, 0);
      colors.add(255, 255, 255, 0);
    } else {
      colors.add(255, 255, 255, 1 - col);
      colors.add(255, 255, 255, 1 - col);
    }
  }

  // the two last indices
  indices.add(0);
  indices.add(1);

  // create mesh
  _mesh = new IndexedMesh(GLPrimitive::triangleStrip(),
                          true,
                          vertices.getCenter(),
                          vertices.create(),
                          indices.create(),
                          1,
                          1,
                          NULL,
                          colors.create());

}

void BusyMeshRenderer::start() {
  //int _TODO_start_effects;
}

void BusyMeshRenderer::stop() {
  //int _TODO_stop_effects;
}

void BusyMeshRenderer::render(const G3MRenderContext* rc,
                              const GLState& parentState)
{
  GL* gl = rc->getGL();

  // set mesh glstate
  GLState state(parentState);
  state.enableBlend();
  gl->setBlendFuncSrcAlpha();

  // init effect in the first render
  static bool firstTime = true;
  if (firstTime) {
    firstTime = false;
    Effect *effect = new BusyMeshEffect(this);
    rc->getEffectsScheduler()->startEffect(effect, this);
  }

  // init modelview matrix
  int currentViewport[4];
  gl->getViewport(currentViewport);
  const int halfWidth = currentViewport[2] / 2;
  const int halfHeight = currentViewport[3] / 2;
  MutableMatrix44D M = MutableMatrix44D::createOrthographicProjectionMatrix(-halfWidth, halfWidth,
                                                                            -halfHeight, halfHeight,
                                                                            -halfWidth, halfWidth);
  gl->setProjection(M);
  gl->loadMatrixf(MutableMatrix44D::identity());

  // clear screen
  gl->clearScreen(0.0f, 0.0f, 0.0f, 1.0f);

  gl->pushMatrix();
  MutableMatrix44D R1 = MutableMatrix44D::createRotationMatrix(Angle::zero(), Vector3D(-1, 0, 0));
  MutableMatrix44D R2 = MutableMatrix44D::createRotationMatrix(Angle::fromDegrees(_degrees), Vector3D(0, 0, -1));
  gl->multMatrixf(R1.multiply(R2));

  // draw mesh
  _mesh->render(rc, state);
  
  gl->popMatrix();
}

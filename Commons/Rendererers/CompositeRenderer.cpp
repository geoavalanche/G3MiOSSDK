//
//  CompositeRenderer.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "CompositeRenderer.hpp"

void CompositeRenderer::initialize(const Context* context) {
  _context = context;
  
  const int rendersSize = _renderers.size();
  for (int i = 0; i < rendersSize; i++) {
    _renderers[i]->initialize(context);
  }
}

void CompositeRenderer::addRenderer(Renderer *renderer) {
  _renderers.push_back(renderer);
  if (_context != NULL) {
    renderer->initialize(_context);
  }
}

void CompositeRenderer::render(const RenderContext* rc) {
  //rc->getLogger()->logInfo("CompositeRenderer::render()");
  
  const int rendersSize = _renderers.size();
  for (int i = 0; i < rendersSize; i++) {
    Renderer* renderer = _renderers[i];
    if (renderer->isEnable()) {
      renderer->render(rc);
    }
  }
}

bool CompositeRenderer::onTouchEvent(const EventContext* ec,
                                     const TouchEvent* touchEvent) {
  // the events are processed bottom to top
  const int rendersSize = _renderers.size();
  for (int i = rendersSize - 1; i >= 0; i--) {
    Renderer* renderer = _renderers[i];
    if (renderer->isEnable()) {
      if (renderer->onTouchEvent(ec, touchEvent)) {
        return true;
      }
    }
  }
  return false;
}

void CompositeRenderer::onResizeViewportEvent(const EventContext* ec,
                                              int width, int height)
{
  // the events are processed bottom to top
  const int rendersSize = _renderers.size();
  for (int i = rendersSize - 1; i >= 0; i--) {
    _renderers[i]->onResizeViewportEvent(ec, width, height);
  }
}

bool CompositeRenderer::isReadyToRender(const RenderContext *rc) {
  const int rendersSize = _renderers.size();
  for (int i = 0; i < rendersSize; i++) {
    Renderer* renderer = _renderers[i];
    if (renderer->isEnable()) {
      if (!renderer->isReadyToRender(rc)) {
        return false;
      }
    }
  }
  
  return true;
}

void CompositeRenderer::start() {
  const int rendersSize = _renderers.size();
  for (int i = 0; i < rendersSize; i++) {
    _renderers[i]->start();
  }
}

void CompositeRenderer::stop() {
  const int rendersSize = _renderers.size();
  for (int i = 0; i < rendersSize; i++) {
    _renderers[i]->stop();
  }
}

void CompositeRenderer::onResume(const Context* context) {
  const int rendersSize = _renderers.size();
  for (int i = 0; i < rendersSize; i++) {
    _renderers[i]->onResume(context);
  }
}

void CompositeRenderer::onPause(const Context* context) {
  const int rendersSize = _renderers.size();
  for (int i = 0; i < rendersSize; i++) {
    _renderers[i]->onPause(context);
  }
}

void CompositeRenderer::onDestroy(const Context* context) {
  const int rendersSize = _renderers.size();
  for (int i = 0; i < rendersSize; i++) {
    _renderers[i]->onDestroy(context);
  }
}

bool CompositeRenderer::isEnable() const {
  if (!_enable) {
    return false;
  }
  
  const int rendersSize = _renderers.size();
  for (int i = 0; i < rendersSize; i++) {
    if (_renderers[i]->isEnable()) {
      return true;
    }
  }
  return false;
}

void CompositeRenderer::setEnable(bool enable) {
  _enable = enable;
}

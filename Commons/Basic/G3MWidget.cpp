//
//  G3MWidget.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "G3MWidget.hpp"

#include "ITimer.hpp"

G3MWidget* G3MWidget::create(IFactory* factory,
                             ILogger *logger,
                             IGL* gl,
                             TexturesHandler* texturesHandler,
                             Downloader* downloader,
                             const Planet* planet,
                             Renderer* renderer,
                             int width, int height,
                             Color backgroundColor,
                             const bool logFPS) {
  if (logger != NULL) {
    logger->logInfo("Creating G3MWidget...");
  }
  
  return new G3MWidget(factory,
                       logger,
                       gl,
                       texturesHandler,
                       downloader,
                       planet,
                       renderer,
                       width, height,
                       backgroundColor,
                       logFPS);
}

void G3MWidget::initializeGL() {
  _gl->depthTest(true);
#ifdef C_CODE
  _gl->cullFace(true, BACK);
#endif
#ifdef JAVA_CODE
  _gl.cullFace(true, CullFace.BACK);
#endif
}

G3MWidget::~G3MWidget() { 
  delete _factory;
  delete _logger;
  delete _gl;
  delete _planet;
  delete _renderer;
  delete _camera;
  delete _texturesHandler;
  delete _timer;
}

void G3MWidget::onTouchEvent(const TouchEvent* myEvent) {
  _renderer->onTouchEvent(myEvent);
}

void G3MWidget::onResizeViewportEvent(int width, int height) {
  _renderer->onResizeViewportEvent(width, height);
}

int G3MWidget::render() {
  _timer->start();
  _renderCounter++;
  
  RenderContext rc(_factory, _logger, _planet, _gl, _camera, _texturesHandler, _downloader);
  
  // Clear the scene
  _gl->clearScreen(_backgroundColor);
  
  int timeToRedraw = _renderer->render(&rc);

  
  const TimeInterval elapsedTime = _timer->elapsedTime();
  if (elapsedTime.milliseconds() > 50) {
    _logger->logWarning("Frame took too much time: %dms" , elapsedTime.milliseconds());
  }
  _totalRenderTime += elapsedTime.milliseconds();
  
  if ((_renderCounter % 60) == 0) {
    if (_logFPS) {
      const double averageTimePerRender = (double) _totalRenderTime / _renderCounter;
      const double fps = 1000.0 / averageTimePerRender;
      _logger->logInfo("FPS=%f" , fps);
    }
    
    _renderCounter = 0;
    _totalRenderTime = 0;
  }
  
  return timeToRedraw;
}

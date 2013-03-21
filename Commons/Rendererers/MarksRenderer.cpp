//
//  MarksRenderer.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 05/06/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#include "MarksRenderer.hpp"
#include "Camera.hpp"
#include "GL.hpp"
#include "TouchEvent.hpp"
#include "RectangleI.hpp"
#include "Mark.hpp"
#include "MarkTouchListener.hpp"
#include "DownloadPriority.hpp"
#include "FloatBufferBuilderFromCartesian2D.hpp"

void MarksRenderer::setMarkTouchListener(MarkTouchListener* markTouchListener,
                                         bool autoDelete) {
  if ( _autoDeleteMarkTouchListener ) {
    delete _markTouchListener;
  }

  _markTouchListener = markTouchListener;
  _autoDeleteMarkTouchListener = autoDelete;
}

MarksRenderer::MarksRenderer(bool readyWhenMarksReady) :
_readyWhenMarksReady(readyWhenMarksReady),
_context(NULL),
_lastCamera(NULL),
_markTouchListener(NULL),
_autoDeleteMarkTouchListener(false),
_downloadPriority(DownloadPriority::MEDIUM)
{
}


MarksRenderer::~MarksRenderer() {
  int marksSize = _marks.size();
  for (int i = 0; i < marksSize; i++) {
    delete _marks[i];
  }

  if ( _autoDeleteMarkTouchListener ) {
    delete _markTouchListener;
  }
  _markTouchListener = NULL;
};


void MarksRenderer::initialize(const G3MContext* context) {
  _context = context;

  int marksSize = _marks.size();
  for (int i = 0; i < marksSize; i++) {
    Mark* mark = _marks[i];
    mark->initialize(context, _downloadPriority);
  }
}

void MarksRenderer::addMark(Mark* mark) {
  _marks.push_back(mark);
  if (_context != NULL) {
    mark->initialize(_context, _downloadPriority);
  }
}

void MarksRenderer::removeMark(Mark* mark){
  int pos = -1;
  const int marksSize = _marks.size();
  for (int i = 0; i < marksSize; i++) {
    if (_marks[i] == mark) {
      pos = i;
      break;
    }
  }
  if (pos != -1) {
#ifdef C_CODE
    _marks.erase(_marks.begin() + pos);
#endif
#ifdef JAVA_CODE
    _marks.remove(pos);
#endif
  }
}

void MarksRenderer::removeAllMarks() {
  const int marksSize = _marks.size();
  for (int i = 0; i < marksSize; i++) {
    delete _marks[i];
  }
  _marks.clear();
}

bool MarksRenderer::onTouchEvent(const G3MEventContext* ec,
                                 const TouchEvent* touchEvent) {

  bool handled = false;

//  if ( (touchEvent->getType() == Down) && (touchEvent->getTouchCount() == 1) ) {
  if ( touchEvent->getType() == DownUp ) {

    if (_lastCamera != NULL) {
      const Vector2I touchedPixel = touchEvent->getTouch(0)->getPos();
      const Planet* planet = ec->getPlanet();

      double minSqDistance = IMathUtils::instance()->maxDouble();
      Mark* nearestMark = NULL;

      const int marksSize = _marks.size();
      for (int i = 0; i < marksSize; i++) {
        Mark* mark = _marks[i];

        if (!mark->isReady()) {
          continue;
        }
        if (!mark->isRendered()) {
          continue;
        }

        const int textureWidth = mark->getTextureWidth();
        if (textureWidth <= 0) {
          continue;
        }

        const int textureHeight = mark->getTextureHeight();
        if (textureHeight <= 0) {
          continue;
        }

        const Vector3D* cartesianMarkPosition = mark->getCartesianPosition(planet);
        const Vector2I markPixel = _lastCamera->point2Pixel(*cartesianMarkPosition);

        const RectangleI markPixelBounds(markPixel._x - (textureWidth / 2),
                                         markPixel._y - (textureHeight / 2),
                                         textureWidth,
                                         textureHeight);

        if (markPixelBounds.contains(touchedPixel._x, touchedPixel._y)) {
          const double distance = markPixel.sub(touchedPixel).squaredLength();
          if (distance < minSqDistance) {
            nearestMark = mark;
            minSqDistance = distance;
          }
        }
      }

      if (nearestMark != NULL) {
        handled = nearestMark->touched();
        if (!handled) {
          if (_markTouchListener != NULL) {
            handled = _markTouchListener->touchedMark(nearestMark);
          }
        }
      }
    }
  }

  return handled;
}

bool MarksRenderer::isReadyToRender(const G3MRenderContext* rc) {
  if (_readyWhenMarksReady) {
    int marksSize = _marks.size();
    for (int i = 0; i < marksSize; i++) {
      if (!_marks[i]->isReady()) {
        return false;
      }
    }
  }

  return true;
}

void MarksRenderer::render(const G3MRenderContext* rc,
                           const GLState& parentState) {
  //  rc.getLogger()->logInfo("MarksRenderer::render()");

  // Saving camera for use in onTouchEvent
  _lastCamera = rc->getCurrentCamera();

  GL* gl = rc->getGL();

  GLState state(parentState);
  state.disableDepthTest();
  state.enableBlend();
  state.enableTextures();
  state.enableTexture2D();
  state.enableVerticesPosition();
  
  Vector2D textureTranslation(0.0, 0.0);
  Vector2D textureScale(1.0, 1.0);
  
//  state.translateTextureCoordinates(textureTranslation);
//  state.scaleTextureCoordinates(textureScale);
  
  gl->transformTexCoords(textureScale, textureTranslation);

  state.setBlendFactors(GLBlendFactor::srcAlpha(), GLBlendFactor::oneMinusSrcAlpha());

  const Camera* camera = rc->getCurrentCamera();
  const Vector3D cameraPosition = camera->getCartesianPosition();

  gl->startBillBoardDrawing(camera->getWidth(),
                            camera->getHeight());
  
  state.setTextureCoordinates(gl->getBillboardTexCoord(), 2, 0);

  const int marksSize = _marks.size();
  for (int i = 0; i < marksSize; i++) {
    Mark* mark = _marks[i];
    //rc->getLogger()->logInfo("Rendering Mark: \"%s\"", mark->getName().c_str());
    
    if (mark->isReady()) {
      mark->render(rc, cameraPosition, state);
    }
  }
  
  gl->stopBillBoardDrawing();
}

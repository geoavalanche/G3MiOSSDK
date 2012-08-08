//
//  CameraSingleDragHandler.cpp
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 28/07/12.
//  Copyright (c) 2012 Universidad de Las Palmas. All rights reserved.
//

#include <iostream>

#include "CameraSingleDragHandler.hpp"
#include "MutableVector2D.hpp"
#include "GL.hpp"
#include "TouchEvent.hpp"
#include "CameraRenderer.hpp"


bool CameraSingleDragHandler::onTouchEvent(const TouchEvent* touchEvent, CameraContext *cameraContext) 
{
  // only one finger needed
  if (touchEvent->getTouchCount()!=1) return false;
  if (touchEvent->getTapCount()>1) return false;

  switch (touchEvent->getType()) {
    case Down:
      onDown(*touchEvent, cameraContext);
      break;
    case Move:
      onMove(*touchEvent, cameraContext);
      break;
    case Up:
      onUp(*touchEvent, cameraContext);
    default:
      break;
  }
  
  return true;
}


void CameraSingleDragHandler::onDown(const TouchEvent& touchEvent, CameraContext *cameraContext) 
{  
  _camera0 = Camera(*_camera);
  cameraContext->setCurrentGesture(Drag); 
  
  // dragging
  Vector2D pixel = touchEvent.getTouch(0)->getPos();  
  _initialPoint = _camera0.pixel2PlanetPoint(pixel).asMutableVector3D();
  
  //printf ("down 1 finger\n");
}


void CameraSingleDragHandler::onMove(const TouchEvent& touchEvent, CameraContext *cameraContext) 
{
  if (cameraContext->getCurrentGesture()!=Drag) return;
  if (_initialPoint.isNan()) return;
      
  const Vector2D pixel = touchEvent.getTouch(0)->getPos();  
  MutableVector3D finalPoint = _camera0.pixel2PlanetPoint(pixel).asMutableVector3D();
  if (finalPoint.isNan()) {
    //INVALID FINAL POINT
    Vector3D ray = _camera0.pixel2Ray(pixel);
    Vector3D pos = _camera0.getPosition();
    finalPoint = _planet->closestPointToSphere(pos, ray).asMutableVector3D();
  }
  
  _camera->copyFrom(_camera0);
  _camera->dragCamera(_initialPoint.asVector3D(), finalPoint.asVector3D());
  
  //printf ("Moving 1 finger.  gesture=%d\n", _currentGesture);
}


void CameraSingleDragHandler::onUp(const TouchEvent& touchEvent, CameraContext *cameraContext) 
{
  cameraContext->setCurrentGesture(None);
  _initialPixel = Vector3D::nan().asMutableVector3D();
  
  //printf ("end 1 finger\n");
}

int CameraSingleDragHandler::render(const RenderContext* rc, CameraContext *cameraContext) {
  _planet = rc->getPlanet();
  _camera = rc->getCamera();
  _gl = rc->getGL();

  // TEMP TO DRAW A POINT WHERE USER PRESS
  if (false) {
    if (cameraContext->getCurrentGesture() == Drag) {
      float vertices[] = { 0,0,0};
      int indices[] = {0};
      _gl->enableVerticesPosition();
      _gl->disableTexture2D();
      _gl->disableTextures();
      _gl->vertexPointer(3, 0, vertices);
      _gl->color((float) 0, (float) 1, (float) 0, 1);
      _gl->pointSize(60);
      _gl->pushMatrix();
      
//      double height = _planet->toGeodetic3D(_camera->getPosition()).height();
      //printf ("altura camara = %f\n", height);
      
      MutableMatrix44D T = MutableMatrix44D::createTranslationMatrix(_initialPoint.asVector3D());
      _gl->multMatrixf(T);
      _gl->drawPoints(1, indices);
      _gl->popMatrix();
            
      //Geodetic2D g = _planet->toGeodetic2D(_initialPoint.asVector3D());
      //printf ("zoom with initial point = (%f, %f)\n", g.latitude().degrees(), g.longitude().degrees());
    }
  }

  return MAX_TIME_TO_RENDER;
}




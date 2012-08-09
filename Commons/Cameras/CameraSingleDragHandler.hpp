//
//  CameraSingleDragHandler.hpp
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 28/07/12.
//  Copyright (c) 2012 Universidad de Las Palmas. All rights reserved.
//

#ifndef G3MiOSSDK_CameraSingleDragHandler_h
#define G3MiOSSDK_CameraSingleDragHandler_h


#include "CameraEventHandler.hpp"
#include "Camera.hpp"


class CameraSingleDragHandler: public CameraEventHandler {
  
public:
  CameraSingleDragHandler():
  _camera0(Camera(NULL, 0, 0)),
  _initialPoint(0,0,0),
  _initialPixel(0,0,0)
  {}
  
  ~CameraSingleDragHandler() {}


  bool onTouchEvent(const EventContext *eventContext,
                    const TouchEvent* touchEvent, 
                    CameraContext *cameraContext);
  int render(const RenderContext* rc, CameraContext *cameraContext);
  
  void onDown(const EventContext *eventContext,
              const TouchEvent& touchEvent, 
              CameraContext *cameraContext);
  void onMove(const EventContext *eventContext,
              const TouchEvent& touchEvent, 
              CameraContext *cameraContext);
  void onUp(const EventContext *eventContext,
            const TouchEvent& touchEvent, 
            CameraContext *cameraContext);
  
  
  Camera _camera0;         //Initial Camera saved on Down event
  
  MutableVector3D _initialPoint;  //Initial point at dragging
  MutableVector3D _initialPixel;  //Initial pixel at start of gesture
  
};


#endif

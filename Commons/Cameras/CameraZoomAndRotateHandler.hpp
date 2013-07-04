//
//  CameraZoomAndRotateHandler.h
//  G3MiOSSDK
//
//  Created by Agustín Trujillo on 26/06/13.
//
//

#ifndef __G3MiOSSDK__CameraZoomAndRotateHandler__
#define __G3MiOSSDK__CameraZoomAndRotateHandler__


#include "CameraEventHandler.hpp"
#include "Camera.hpp"


class CameraZoomAndRotateHandler: public CameraEventHandler {
private:
  const bool _processRotation;
  const bool _processZoom;
  
  void zoom();
  void rotate();
  
  
public:
  CameraZoomAndRotateHandler(bool processRotation,
                          bool processZoom):
  _camera0(Camera(0, 0)),
  //_initialPoint(0,0,0),
  //_initialPixel(0,0,0),
  _processRotation(processRotation),
  _processZoom(processZoom)
  {
  }
  
  ~CameraZoomAndRotateHandler() {}
  
  
  bool onTouchEvent(const G3MEventContext *eventContext,
                    const TouchEvent* touchEvent,
                    CameraContext *cameraContext);
  
  void render(const G3MRenderContext* rc,
              CameraContext *cameraContext);
  
  void onDown(const G3MEventContext *eventContext,
              const TouchEvent& touchEvent,
              CameraContext *cameraContext);
  void onMove(const G3MEventContext *eventContext,
              const TouchEvent& touchEvent,
              CameraContext *cameraContext);
  void onUp(const G3MEventContext *eventContext,
            const TouchEvent& touchEvent,
            CameraContext *cameraContext);
  
  //MutableVector3D _initialPoint;  //Initial point at dragging
  MutableVector2I _initialPixel0, _initialPixel1;  //Initial pixels at start of gesture
  //MutableVector3D _initialPoint0, _initialPoint1;
  double _initialFingerSeparation;
  double _initialFingerInclination;
  
  Camera _camera0;         //Initial Camera saved on Down event
  
};


#endif /* defined(__G3MiOSSDK__CameraZoomAndRotateHandler__) */

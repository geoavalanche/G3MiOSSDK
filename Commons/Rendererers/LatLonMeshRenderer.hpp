//
//  LatLonMeshRenderer.h
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 02/08/12.
//  Copyright (c) 2012 Universidad de Las Palmas. All rights reserved.
//

#ifndef G3MiOSSDK_LatLonMeshRenderer_h
#define G3MiOSSDK_LatLonMeshRenderer_h

#include "LeafRenderer.hpp"
#include "IndexedMesh.hpp"


class LatLonMeshRenderer: public LeafRenderer {
  
private:
  Mesh * _mesh;
  
public:
  ~LatLonMeshRenderer();
  
  void initialize(const InitializationContext* ic);  
  
  void render(const RenderContext* rc);
  
  bool onTouchEvent(const EventContext* ec,
                    const TouchEvent* touchEvent) {
    return false;
  };
  
  void onResizeViewportEvent(const EventContext* ec,
                             int width, int height) {}
  
  bool isReadyToRender(const RenderContext* rc) {
    return true;
  }
  
  void start() {
    
  }
  
  void stop() {
    
  }
  
  void onResume(const InitializationContext* ic) {
    
  }
  
  void onPause(const InitializationContext* ic) {
    
  }
  
  void onDestroy(const InitializationContext* ic) {

  }

};

#endif

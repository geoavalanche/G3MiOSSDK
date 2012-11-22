//
//  GLErrorRenderer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 20/06/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#ifndef G3MiOSSDK_GLErrorRenderer_hpp
#define G3MiOSSDK_GLErrorRenderer_hpp

#include "LeafRenderer.hpp"

class GLErrorRenderer : public LeafRenderer {
public:
  
  void initialize(const G3MContext* context);
  
  void render(const G3MRenderContext* rc);
  
  bool onTouchEvent(const G3MEventContext* ec,
                            const TouchEvent* touchEvent);
  
  virtual ~GLErrorRenderer();
  
  void onResizeViewportEvent(const G3MEventContext* ec,
                             int width, int height);
  
  bool isReadyToRender(const G3MRenderContext* rc) {
    return true;
  }
  
  void start() {
    
  }
  
  void stop() {
    
  }
  
  void onResume(const G3MContext* context) {
    
  }
  
  void onPause(const G3MContext* context) {
    
  }
  
  void onDestroy(const G3MContext* context) {

  }

};

#endif

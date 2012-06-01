//
//  CompositeRenderer.h
//  G3MiOSSDK
//
//  Created by José Miguel S N on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_CompositeRenderer_h
#define G3MiOSSDK_CompositeRenderer_h

#include "Renderer.h"

#include <vector>

class CompositeRenderer: public Renderer
{
private:
  std::vector<Renderer*> _renderers;
  
  const InitializationContext* _ic;
  
public:
  CompositeRenderer(): _ic(NULL) {
    _renderers = std::vector<Renderer*>();
  }
  
  virtual ~CompositeRenderer() {
  }
  
  void initialize(const InitializationContext* ic);  
  
  int render(const RenderContext* rc);
  
  bool onTouchEvent(const TouchEvent& event);
  
  void addRenderer(Renderer* renderer);
};

#endif

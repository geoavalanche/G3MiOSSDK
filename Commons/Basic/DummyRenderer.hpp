//
//  DummyRenderer.hpp
//  Glob3 Mobile
//
//  Created by Agustín Trujillo Pino on 02/05/11.
//  Copyright 2011 Universidad de Las Palmas. All rights reserved.
//

#ifndef DUMMYRENDERER
#define DUMMYRENDERER

#include "Renderer.hpp"

class DummyRenderer: public Renderer {

private:
  int numIndices;
  double halfSize;
  
  unsigned char * index;
  float * vertices;

public:
  ~DummyRenderer();
  
  void initialize(const InitializationContext* ic);  
  
  int render(const RenderContext* rc);
  
  bool onTouchEvent(const TouchEvent* touchEvent);
  
  bool onResizeViewportEvent(int width, int height){ return false;}
  
  
};

#endif
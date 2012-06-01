//
//  G3MWidget.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "G3MWidget.h"

G3MWidget::~G3MWidget()
{
  delete _factory;
  delete _gl;
}

bool G3MWidget::render()
{
  RenderContext rc(_factory, _gl);
  
  _renderer.render(rc);
  
  return true;
}

G3MWidget G3MWidget::create(const Planet &planet, Renderer &renderer) {
  int __TODO_create_factory_and_gl;
  IFactory* factory = NULL;
  IGL* gl  = NULL;
  return G3MWidget(planet, renderer, factory, gl);
}

void G3MWidget::onTapEvent(TapEvent &event)
{
    
}


void G3MWidget::onTouchEvent(TouchEvent &event)
{
    
}

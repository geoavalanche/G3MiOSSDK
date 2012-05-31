//
//  IRenderer.h
//  G3MiOSSDK
//
//  Created by José Miguel S N on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_IRenderer_h
#define G3MiOSSDK_IRenderer_h

#include "Context.h"

#include "TouchEvent.h"

class Renderer {
public:
    virtual void initialize(InitializationContext& ic) = 0;  
    
    virtual int render(RenderContext &rc) = 0;
    
    virtual bool onTapEvent(TapEvent& event) = 0;
    virtual bool onTouchEvent(TouchEvent &event) = 0;
};


#endif

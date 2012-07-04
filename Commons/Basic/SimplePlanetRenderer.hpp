//
//  SimplePlanetRenderer.h
//  G3MiOSSDK
//
//  Created by José Miguel S N on 12/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_SimplePlanetRenderer_h
#define G3MiOSSDK_SimplePlanetRenderer_h

#include "Renderer.hpp"

#include "IImage.hpp"

#include "IndexedTriangleStripMesh.hpp"

class SimplePlanetRenderer: public Renderer {
private:
  
  const std::string _textureFilename;
  
  const int _latRes;
  const int _lonRes;
  
  IndexedTriangleStripMesh * _mesh;
  
  
  float * createVertices(const Planet& planet);
  unsigned int * createMeshIndex();
  float * createTextureCoordinates();
  
  bool initializeMesh(const RenderContext* rc);
  
public:
  SimplePlanetRenderer(const std::string textureFilename);
  ~SimplePlanetRenderer();
  
  void initialize(const InitializationContext* ic);  
  
  int render(const RenderContext* rc);
  
  bool onTouchEvent(const TouchEvent* touchEvent) {
    return false;
  }
  
  void onResizeViewportEvent(int width, int height) {
    
  }
  
  
};



#endif

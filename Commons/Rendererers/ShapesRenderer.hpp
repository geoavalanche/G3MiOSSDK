//
//  ShapesRenderer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 10/28/12.
//
//

#ifndef __G3MiOSSDK__ShapesRenderer__
#define __G3MiOSSDK__ShapesRenderer__

#include "LeafRenderer.hpp"
#include "Shape.hpp"
#include <vector>

class ShapesRenderer : public LeafRenderer {
private:
  std::vector<Shape*> _shapes;

#ifdef C_CODE
  const G3MContext* _context;
#endif
#ifdef JAVA_CODE
  private G3MContext _context;
#endif

public:

  ShapesRenderer() :
  _context(NULL)
  {

  }

  ~ShapesRenderer() {
    const int shapesCount = _shapes.size();
    for (int i = 0; i < shapesCount; i++) {
      Shape* shape = _shapes[i];
      delete shape;
    }
  }

  void addShape(Shape* shape) {
    _shapes.push_back(shape);
    if (_context != NULL) {
      shape->initialize(_context);
    }
  }

  void onResume(const G3MContext* context) {
    _context = context;
  }

  void onPause(const G3MContext* context) {

  }

  void onDestroy(const G3MContext* context) {

  }

  void initialize(const G3MContext* context) {
    _context = context;

    const int shapesCount = _shapes.size();
    for (int i = 0; i < shapesCount; i++) {
      Shape* shape = _shapes[i];
      shape->initialize(context);
    }
  }
  
  bool isReadyToRender(const G3MRenderContext* rc) {
    return true;
  }

  bool onTouchEvent(const G3MEventContext* ec,
                    const TouchEvent* touchEvent) {
    return false;
  }

  void onResizeViewportEvent(const G3MEventContext* ec,
                             int width, int height) {
  }

  void start(const G3MRenderContext* rc) {
  }
  
  void stop(const G3MRenderContext* rc) {
  }

  void render(const G3MRenderContext* rc,
              const GLState& parentState, const GPUProgramState* gpuParentProgramState);

};

#endif

//
//  BusyQuadRenderer.hpp
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 13/08/12.
//  Copyright (c) 2012 Universidad de Las Palmas. All rights reserved.
//

#ifndef G3MiOSSDK_BusyQuadRenderer_hpp
#define G3MiOSSDK_BusyQuadRenderer_hpp


#include "LeafRenderer.hpp"
#include "Effects.hpp"
#include "Vector2D.hpp"
#include "Color.hpp"
#include "DirectMesh.hpp"

#include "GPUProgramState.hpp"

#include "MutableMatrix44D.hpp"


//***************************************************************


class BusyQuadRenderer : public LeafRenderer{
private:
  double      _degrees;
  //  const std::string _textureFilename;
  IImage*     _image;
  Mesh*       _quadMesh;
  
  const bool      _animated;
  const Vector2D  _size;
  Color*          _backgroundColor;
  
  bool initMesh(const G3MRenderContext* rc);
  
  mutable MutableMatrix44D _modelviewMatrix;
  MutableMatrix44D _projectionMatrix;
  
  GLState _glState;
  void createGLState() const;
  
  
public:
  BusyQuadRenderer(IImage* image,
                   Color* backgroundColor,
                   const Vector2D& size,
                   const bool animated):
  _degrees(0),
  _quadMesh(NULL),
  _image(image),
  _backgroundColor(backgroundColor),
  _animated(animated),
  _size(size),
  _projectionMatrix(MutableMatrix44D::invalid())
  {
    createGLState();
  }
  
  void initialize(const G3MContext* context) {}
  
  bool isReadyToRender(const G3MRenderContext* rc) {
    return true;
  }
  
  void render(const G3MRenderContext* rc,
              const GLGlobalState& parentState);
  
  bool onTouchEvent(const G3MEventContext* ec,
                    const TouchEvent* touchEvent) {
    return false;
  }
  
  void onResizeViewportEvent(const G3MEventContext* ec,
                             int width, int height) {
    
  }
  
  virtual ~BusyQuadRenderer() {
    delete _quadMesh;
    delete _backgroundColor;
  }
  
  void incDegrees(double value) {
    _degrees += value;
    if (_degrees>360) _degrees -= 360;
    _modelviewMatrix = MutableMatrix44D::createRotationMatrix(Angle::fromDegrees(_degrees), Vector3D(0, 0, 1));
    _glState.getGPUProgramState()->setUniformMatrixValue(MODELVIEW, _projectionMatrix.multiply(_modelviewMatrix), false);
  }
  
  void start(const G3MRenderContext* rc);
  
  void stop(const G3MRenderContext* rc);
  
  void onResume(const G3MContext* context) {
    
  }
  
  void onPause(const G3MContext* context) {
    
  }
  
  void onDestroy(const G3MContext* context) {
    
  }
  
};

//***************************************************************

class BusyEffect : public EffectWithForce {
private:
  BusyQuadRenderer* _renderer;
  
public:
  
  BusyEffect(BusyQuadRenderer *renderer):
  EffectWithForce(1, 1),
  _renderer(renderer)
  { }
  
  virtual void start(const G3MRenderContext *rc,
                     const TimeInterval& when) {}
  
  virtual void doStep(const G3MRenderContext *rc,
                      const TimeInterval& when) {
    EffectWithForce::doStep(rc, when);
    _renderer->incDegrees(3);
  }
  
  virtual void stop(const G3MRenderContext *rc,
                    const TimeInterval& when) { }
  
  virtual void cancel(const TimeInterval& when) {
    // do nothing, just leave the effect in the intermediate state
  }
  
};



#endif

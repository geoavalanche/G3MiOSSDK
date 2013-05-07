//
//  GL.hpp
//  Glob3 Mobile
//
//  Created by Agustín Trujillo Pino on 14/06/11.
//  Copyright 2011 Universidad de Las Palmas. All rights reserved.
//

#ifndef G3MiOSSDK_GL_hpp
#define G3MiOSSDK_GL_hpp

#include "INativeGL.hpp"

#include "IImage.hpp"
#include "MutableMatrix44D.hpp"
#include "Color.hpp"
#include "MutableVector2D.hpp"
#include "IFloatBuffer.hpp"
#include "GLConstants.hpp"
#include "GLState.hpp"

#include <list>

class IGLProgramId;
class IGLUniformID;

#include "IGLTextureId.hpp"

class GPUProgramManager;
class GPUProgramState;
class GPUProgram;

class GL {
private:
  INativeGL* const _nativeGL;
  
  std::list<const IGLTextureId*> _texturesIdBag;
  long                           _texturesIdAllocationCounter;
  
  GLState *_currentState;
  
  ShaderProgram* _program;
  
  GPUProgram* _currentGPUProgram;
  
  inline void loadModelView();
  
  const IGLTextureId* getGLTextureId();
  
  //Get Locations warning of errors
  bool _errorGettingLocationOcurred;
  int checkedGetAttribLocation(ShaderProgram* program,
                               const std::string& name);
  IGLUniformID* checkedGetUniformLocation(ShaderProgram* program,
                                          const std::string& name);
  
//  IFloatBuffer* _billboardTexCoord;
  
  const bool _verbose;
  
  void setGLState(const GLState& state);
  void setProgramState(GPUProgramManager& progManager, const GPUProgramState& progState);
  
public:
  
  
  GL(INativeGL* const nativeGL,
     bool verbose) :
  _nativeGL(nativeGL),
  _verbose(verbose),
  _texturesIdAllocationCounter(0),
//  _billboardTexCoord(NULL),
  _program(NULL),
  _currentState(NULL),
  _currentGPUProgram(NULL)
  {
    //Init Constants
    GLCullFace::init(_nativeGL);
    GLBufferType::init(_nativeGL);
    GLFeature::init(_nativeGL);
    GLType::init(_nativeGL);
    GLPrimitive::init(_nativeGL);
    GLBlendFactor::init(_nativeGL);
    GLTextureType::init(_nativeGL);
    GLTextureParameter::init(_nativeGL);
    GLTextureParameterValue::init(_nativeGL);
    GLAlignment::init(_nativeGL);
    GLFormat::init(_nativeGL);
    GLVariable::init(_nativeGL);
    GLError::init(_nativeGL);
    
    _currentState = GLState::newDefault(); //Init after constants
  }
  
//  IFloatBuffer* getBillboardTexCoord();
  
  void clearScreen(const GLState& state);
  
  void drawElements(int mode,
                    IShortBuffer* indices, const GLState& state,
                    GPUProgramManager& progManager,const GPUProgramState* gpuState);
  
  void drawArrays(int mode,
                  int first,
                  int count, const GLState& state,
                  GPUProgramManager& progManager,const GPUProgramState* gpuState);
  
  bool useProgram(ShaderProgram* program);
  
  int getError();
  
  const IGLTextureId* uploadTexture(const IImage* image,
                                    int format,
                                    bool generateMipmap);
  
  void deleteTexture(const IGLTextureId* textureId);
  
  void getViewport(int v[]) {
    if (_verbose) ILogger::instance()->logInfo("GL::getViewport()");
    
    _nativeGL->getIntegerv(GLVariable::viewport(), v);
  }
  
  ~GL() {
#ifdef C_CODE
    delete _nativeGL;
#endif
#ifdef JAVA_CODE
    _nativeGL.dispose();
#endif
  }
  
  int createProgram() const {
    return _nativeGL->createProgram();
  }
  
  void attachShader(int program, int shader) const {
    _nativeGL->attachShader(program, shader);
  }
  
  int createShader(ShaderType type) const {
    return _nativeGL->createShader(type);
  }
  
  bool compileShader(int shader, const std::string& source) const {
    return _nativeGL->compileShader(shader, source);
  }
  
  bool deleteShader(int shader) const {
    return _nativeGL->deleteShader(shader);
  }
  
  void printShaderInfoLog(int shader) const {
    _nativeGL->printShaderInfoLog(shader);
  }
  
  bool linkProgram(int program) const {
    return _nativeGL->linkProgram(program);
  }
  
  void printProgramInfoLog(int program) const {
    _nativeGL->linkProgram(program);
  }
  
  bool deleteProgram(int program) const  {
    return _nativeGL->deleteProgram(program);
  }
  
  INativeGL* getNative() const{
    return _nativeGL;
  }
  
  void uniform2f(const IGLUniformID* loc,
                 float x,
                 float y) const{ _nativeGL->uniform2f(loc, x, y);}
  
  void uniform1f(const IGLUniformID* loc,
                 float x) const{ _nativeGL->uniform1f(loc, x);}
  void uniform1i(const IGLUniformID* loc,
                 int v) const{ _nativeGL->uniform1i(loc, v);}
  
  void uniformMatrix4fv(const IGLUniformID* location,
                        bool transpose,
                        const MutableMatrix44D* matrix) const{ _nativeGL->uniformMatrix4fv(location, transpose, matrix);}
  
  void uniform4f(const IGLUniformID* location,
                 float v0,
                 float v1,
                 float v2,
                 float v3) const{ _nativeGL->uniform4f(location, v0, v1, v2, v3);}
  
  void vertexAttribPointer(int index,
                           int size,
                           bool normalized,
                           int stride,
                           IFloatBuffer* buffer) const{
    _nativeGL->vertexAttribPointer(index, size, normalized, stride, buffer);
  }
  
  void bindAttribLocation(const GPUProgram* program, int loc, const std::string& name) const{
    _nativeGL->bindAttribLocation(program, loc, name);
  }
  
  int getProgramiv(const GPUProgram* program, int pname) const{
    return _nativeGL->getProgramiv(program, pname);
  }
  
  GPUUniform* getActiveUniform(const GPUProgram* program, int i) const{
    return _nativeGL->getActiveUniform(program, i);
  }
  
  GPUAttribute* getActiveAttribute(const GPUProgram* program, int i) const{
    return _nativeGL->getActiveAttribute(program, i);
  }
  
  GLState* getCurrentState() const{ return _currentState;}
  
  void useProgram(GPUProgram* program) ;
  
  void setUniformsDefaultValues() const;
  
  void enableVertexAttribArray(int location) const{
    _nativeGL->enableVertexAttribArray(location);
  }
  
  void disableVertexAttribArray(int location) const{
    _nativeGL->disableVertexAttribArray(location);
  }
};

#endif

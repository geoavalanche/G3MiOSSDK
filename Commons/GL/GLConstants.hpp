//
//  GLConstants.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 17/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_GLConstants_hpp
#define G3MiOSSDK_GLConstants_hpp

#include "INativeGL.hpp"

class GLCullFace {
  static int _front;
  static int _back;
  static int _frontAndBack;
  
public:
  static int front(){ return _front;}
  static int back(){ return _back;}
  static int frontAndBack(){ return _frontAndBack;}
  
  static void init(const INativeGL* ngl){
    _front = ngl->CullFace_Front();
    _back = ngl->CullFace_Back();
    _frontAndBack = ngl->CullFace_FrontAndBack();
  }
};

class GLBufferType {
  static int _colorBuffer;
  static int _depthBuffer;
  
public:
  static int colorBuffer(){ return _colorBuffer;}
  static int depthBuffer(){ return _depthBuffer;}
  
  static void init(const INativeGL* ngl){
    _colorBuffer = ngl->BufferType_ColorBuffer();
    _depthBuffer = ngl->BufferType_DepthBuffer();
  }
};

class GLFeature {
  static int _polygonOffsetFill;
  static int _depthTest;
  static int _blend;
  static int _cullFace;
  
public:
  static int polygonOffsetFill(){ return _polygonOffsetFill;}
  static int depthTest(){ return _depthTest;}
  static int blend(){ return _blend;}
  static int cullFace(){ return _cullFace;}

  static void init(const INativeGL* ngl){
    _polygonOffsetFill = ngl->Feature_PolygonOffsetFill();
    _depthTest = ngl->Feature_DepthTest();
    _blend = ngl->Feature_Blend();
    _cullFace = ngl->Feature_CullFace();
  }
};

class GLType {
  static int _float;
  static int _unsignedByte;
  static int _unsignedInt;
  static int _int;
  
public:
  static int glFloat(){ return _float;}
  static int glUnsignedByte(){ return _unsignedByte;}
  static int glUnsignedInt(){ return _unsignedInt;}
  static int glInt(){ return _int;}
  
  static void init(const INativeGL* ngl){
    _float = ngl->Type_Float();
    _unsignedByte = ngl->Type_UnsignedByte();
    _unsignedInt = ngl->Type_UnsignedInt();
    _int = ngl->Type_Int();
  }
};

class GLPrimitive {
  static int _triangles;
  static int _triangleStrip;
  static int _triangleFan;

  static int _lines;
  static int _lineStrip;
  static int _lineLoop;

  static int _points;
  
public:
  static int triangles() {
    return _triangles;
  }

  static int triangleStrip() {
    return _triangleStrip;
  }

  static int triangleFan() {
    return _triangleFan;
  }

  static int lines() {
    return _lines;
  }
  
  static int lineStrip() {
    return _lineStrip;
  }
  
  static int lineLoop() {
    return _lineLoop;
  }
  
  static int points() {
    return _points;
  }
  
  static void init(const INativeGL* ngl){
    _triangles     = ngl->Primitive_Triangles();
    _triangleStrip = ngl->Primitive_TriangleStrip();
    _triangleFan   = ngl->Primitive_TriangleFan();

    _lines     = ngl->Primitive_Lines();
    _lineStrip = ngl->Primitive_LineStrip();
    _lineLoop  = ngl->Primitive_LineLoop();

    _points = ngl->Primitive_Points();
  }
};

class GLBlendFactor {
  static int _srcAlpha;
  static int _oneMinusSrcAlpha;
  
  
public:
  static int srcAlpha(){ return _srcAlpha;}
  static int oneMinusSrcAlpha(){ return _oneMinusSrcAlpha;}
  
  static void init(const INativeGL* ngl){
    _srcAlpha = ngl->BlendFactor_SrcAlpha();
    _oneMinusSrcAlpha = ngl->BlendFactor_OneMinusSrcAlpha();
  }
};

class GLTextureType {
  static int _texture2D;
public:
  static int texture2D(){ return _texture2D;}
  
  static void init(const INativeGL* ngl){
    _texture2D = ngl->TextureType_Texture2D();
  }
};

class GLTextureParameter {
  static int _minFilter;
  static int _magFilter;
  static int _wrapS;
  static int _wrapT;
  
public:
  static int minFilter(){ return _minFilter;}
  static int magFilter(){ return _magFilter;}
  static int wrapS(){ return _wrapS;}
  static int wrapT(){ return _wrapT;}
  
  static void init(const INativeGL* ngl){
    _minFilter = ngl->TextureParameter_MinFilter();
    _magFilter = ngl->TextureParameter_MagFilter();
    _wrapS = ngl->TextureParameter_WrapS();
    _wrapT = ngl->TextureParameter_WrapT();
  }
};

class GLTextureParameterValue {
  static int _linear;
  static int _clampToEdge;
  
public:
  static int linear(){ return _linear;}
  static int clampToEdge(){ return _clampToEdge;}
  
  static void init(const INativeGL* ngl){
    _linear = ngl->TextureParameterValue_Linear();
    _clampToEdge = ngl->TextureParameterValue_ClampToEdge();
  }
};

class GLAlignment {
  static int _pack;
  static int _unpack;
  
public:
  static int pack(){ return _pack;}
  static int unpack(){ return _unpack;}
  
  static void init(const INativeGL* ngl){
    _pack = ngl->Alignment_Pack();
    _unpack = ngl->Alignment_Unpack();
  }
};

class GLFormat{
  static int _rgba;
  
public:
  static int rgba(){ return _rgba;}
  
  static void init(const INativeGL* ngl){
    _rgba = ngl->Format_RGBA();
  }
};

class GLVariable{
  static int _viewport;
  
public:
  static int viewport(){ return _viewport;}
  
  static void init(const INativeGL* ngl){
    _viewport = ngl->Variable_Viewport();
  }
};

class GLError{
  static int _noError;
  
public:
  static int noError(){ return _noError;}
  
  static void init(const INativeGL* ngl){
    _noError = ngl->Error_NoError();
  }
};

#endif

//
//  TextureMapping.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 12/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_TextureMapping_hpp
#define G3MiOSSDK_TextureMapping_hpp

//#include <vector>
#include "MutableVector2D.hpp"
#include "TexturesHandler.hpp"

class IGLTextureId;

class G3MRenderContext;
class IFloatBuffer;
class GLGlobalState;
class GPUProgramState;
class GLState;
class TexturesHandler;

class TextureMapping {
public:
  
  virtual ~TextureMapping() {
  }

  virtual void modifyGLState(GLState& state) const = 0;
};



class SimpleTextureMapping : public TextureMapping {
private:
#ifdef C_CODE
  const TextureIDReference* _glTextureId;
#endif
#ifdef JAVA_CODE
  private TextureIDReference _glTextureId;
#endif

  IFloatBuffer* _texCoords;
  const bool    _ownedTexCoords;

#warning replace MutableVector2D with pair of floats
  MutableVector2D _translation;
  MutableVector2D _scale;

  float _rotationInRadians;
  float _rotationCenterX;
  float _rotationCenterY;

  const bool _transparent;

  void releaseGLTextureId();

public:
  
  SimpleTextureMapping(const TextureIDReference* glTextureId,
                       IFloatBuffer* texCoords,
                       bool ownedTexCoords,
                       bool transparent) :
  _glTextureId(glTextureId),
  _texCoords(texCoords),
  _translation(0, 0),
  _scale(1, 1),
  _ownedTexCoords(ownedTexCoords),
  _transparent(transparent),
  _rotationInRadians(0),
  _rotationCenterX(0),
  _rotationCenterY(0)
  {
  }
  
  void setTranslationAndScale(const Vector2D& translation,
                              const Vector2D& scale) {
    _translation = translation.asMutableVector2D();
    _scale       = scale.asMutableVector2D();
  }

  void setRotation(float angleInRadians,
                   float centerX,
                   float centerY) {
    _rotationInRadians = angleInRadians;
    _rotationCenterX = centerX;
    _rotationCenterY = centerY;
  }
  
  virtual ~SimpleTextureMapping();
  
  const IGLTextureId* getGLTextureId() const {
    return _glTextureId->getID();
  }

  IFloatBuffer* getTexCoords() const {
    return _texCoords;
  }
    
  void modifyGLState(GLState& state) const;

};
#endif

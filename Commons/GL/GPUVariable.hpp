//
//  GPUVariable.h
//  G3MiOSSDK
//
//  Created by Jose Miguel SN on 20/06/13.
//
//

#ifndef __G3MiOSSDK__GPUVariable__
#define __G3MiOSSDK__GPUVariable__

#include <string>

enum GPUVariableType{
  ATTRIBUTE = 1,
  UNIFORM = 2
};

enum GPUUniformKey{
  UNRECOGNIZED_UNIFORM = -1,
  FLAT_COLOR = 0,
  MODELVIEW = 1,
  TEXTURE_EXTENT = 2,
  VIEWPORT_EXTENT = 3,
  TRANSLATION_TEXTURE_COORDS = 4,
  SCALE_TEXTURE_COORDS = 5,
  POINT_SIZE= 6
};

enum GPUAttributeKey{
  UNRECOGNIZED_ATTRIBUTE = -1,
  POSITION = 0,
  TEXTURE_COORDS = 1,
  COLOR = 2
};

class GPUVariable{
  
protected:
  
  const GPUVariableType _variableType;
  const std::string _name;
  
  //Uniform metadata based in our shaders
//  long _key;
//  long _group;
//  long _priority;

//  void createMetadata();

public:
  
//  static const int UNRECOGNIZED;
//  
//  static const int FLAT_COLOR;
//  static const int MODELVIEW;
//  static const int TEXTURE_EXTENT;
//  static const int VIEWPORT_EXTENT;
//  static const int TRANSLATION_TEXTURE_COORDS;
//  static const int SCALE_TEXTURE_COORDS;
//  static const int POINT_SIZE;
//  
//  static const int POSITION;
//  static const int TEXTURE_COORDS;
//  static const int COLOR;
//  
//  //To be deleted
//  static const int EnableColorPerVertex;
//  static const int EnableTexture;
//  static const int EnableFlatColor;
//  static const int FlatColorIntensity;
//  static const int ColorPerVertexIntensity;

//  static int getKeyForName(const std::string& name, GPUVariableType variableType);

  static GPUUniformKey getUniformKey(const std::string& name);
  static GPUAttributeKey getAttributeKey(const std::string& name);

  static int getUniformCode(GPUUniformKey u);
  static int getAttributeCode(GPUAttributeKey a);

//  static const int GROUP_COLOR;
//  static const int GROUP_NOGROUP;

  virtual ~GPUVariable(){}
  
  GPUVariable(const std::string& name, GPUVariableType type): _name(name), _variableType(type){
//    createMetadata();
  }
  
  //Uniform metadata based in our shaders
//  long getKey() const { return _key;}
//  long getGroup() const { return _group;}
//  long getPriority() const { return _priority;}

};

#endif /* defined(__G3MiOSSDK__GPUVariable__) */

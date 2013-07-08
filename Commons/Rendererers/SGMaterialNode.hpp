//
//  SGMaterialNode.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/8/12.
//
//

#ifndef __G3MiOSSDK__SGMaterialNode__
#define __G3MiOSSDK__SGMaterialNode__

#include "SGNode.hpp"
#include "Color.hpp"

class SGMaterialNode : public SGNode {
private:
#ifdef C_CODE
  const Color* _baseColor;
#endif
#ifdef JAVA_CODE
  private Color _baseColor;
#endif
  const Color* _specularColor;

//  const double _specular;
//  const double _shine;
//  const double _alpha;
//  const double _emit;

public:

  SGMaterialNode(const std::string& id,
                 const std::string& sId,
                 Color* baseColor,
                 Color* specularColor,
                 double specular,
                 double shine,
                 double alpha,
                 double emit) :
  SGNode(id, sId),
  _baseColor(baseColor),
  _specularColor(specularColor)
//  _specular(specular),
//  _shine(shine),
//  _alpha(alpha),
//  _emit(emit)
  {

  }

  void setBaseColor(Color* baseColor) {
    if (baseColor != _baseColor) {
      delete _baseColor;
      _baseColor = baseColor;
    }
  }

  ~SGMaterialNode() {
    delete _baseColor;
    delete _specularColor;
  }

  void modifyGLGlobalState(GLGlobalState& GLGlobalState) const;
  void modifyGPUProgramState(GPUProgramState& progState) const;

  std::string description(){
    return "SGMaterialNode";
  }

};

#endif

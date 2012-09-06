//
//  FloatBufferBuilderFromCartesian3D.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 06/09/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_FloatBufferBuilderFromCartesian3D_hpp
#define G3MiOSSDK_FloatBufferBuilderFromCartesian3D_hpp

#include "Vector3D.hpp"
#include "FloatBufferBuilder.hpp"

enum CenterStrategy {
  NoCenter,
  FirstVertex,
  GivenCenter
};

class FloatBufferBuilderFromCartesian3D: public FloatBufferBuilder {
private:
  
  const CenterStrategy _centerStrategy;
  float _cx;
  float _cy;
  float _cz;
  
  void setCenter(const Vector3D& center){
    _cx = (float)center.x();
    _cy = (float)center.y();
    _cz = (float)center.z();
  }
  
public:
  
  
  
  FloatBufferBuilderFromCartesian3D(CenterStrategy cs, const Vector3D& center):
  _centerStrategy(cs){
    setCenter(center);
  }
  
  void add(const Vector3D& vector) {
    float x = (float) vector.x();
    float y = (float) vector.y();
    float z = (float) vector.z();
    
    if (_centerStrategy == FirstVertex && _values.size() == 0){
      setCenter(vector);
    }
    
    if (_centerStrategy != NoCenter){
      x -= _cx;
      y -= _cy;
      z -= _cz;
    } else{
    }
    
    _values.push_back(x);
    _values.push_back(y);
    _values.push_back(z);
  }
  
  Vector3D getCenter(){
    return Vector3D((double)_cx,(double)_cy,(double)_cz);
  }
};

#endif

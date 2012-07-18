//
//  Box.h
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 16/07/12.
//  Copyright (c) 2012 Universidad de Las Palmas. All rights reserved.
//

#ifndef G3MiOSSDK_Box_h
#define G3MiOSSDK_Box_h

#include "Extent.h"
#include "Vector3D.hpp"
#include "Frustum.h"

#include <vector>

class Vector2D;


class Box: public Extent {
  
public:
  Box(const Vector3D& lower,
      const Vector3D& upper):
  _lower(lower),
  _upper(upper)
  {}
  
  bool touches(const Frustum* frustum) const {
    return frustum->touchesWithBox(this);
  };
  
  Vector3D getLower() const { return _lower; }
  Vector3D getUpper() const { return _upper; }
  
  inline std::vector<Vector3D> getCorners() const;
  
  double squaredProjectedArea(const RenderContext* rc) const;
  Vector2D projectedExtent(const RenderContext* rc) const;

  
private:
  const Vector3D _lower;
  const Vector3D _upper;
};

#endif

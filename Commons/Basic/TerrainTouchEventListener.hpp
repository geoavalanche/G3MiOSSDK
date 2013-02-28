//
//  TerrainTouchEventListener.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 14/08/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_TerrainTouchEventListener_hpp
#define G3MiOSSDK_TerrainTouchEventListener_hpp

#include "Geodetic3D.hpp"
#include "Sector.hpp"
class Layer;

class TerrainTouchEvent{
private:
  const Geodetic3D _position;
  const Sector     _sector;
  const Layer*     _layer;

public:
  TerrainTouchEvent(const Geodetic3D& position,
                    const Sector& sector,
                    const Layer* layer):
  _position(position),
  _sector(sector),
  _layer(layer)
  {
    
  }
  
  const Geodetic3D getPosition() const {
    return _position;
  }
  
  const Sector getSector() const {
    return _sector;
  }
  
  const Layer* getLayer() const {
    return _layer;
  }

};

class TerrainTouchEventListener {
public:
  
  virtual void onTerrainTouchEvent(const G3MEventContext* context,
                                   const TerrainTouchEvent& event) = 0;
  
  virtual ~TerrainTouchEventListener(){}
  
};

#endif

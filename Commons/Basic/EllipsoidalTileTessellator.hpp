//
//  EllipsoidalTileTessellator.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 27/06/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#ifndef G3MiOSSDK_EllipsoidalTileTessellator_hpp
#define G3MiOSSDK_EllipsoidalTileTessellator_hpp

#include "TileTessellator.hpp"
#include <string>
#include <vector>
#include "MutableVector3D.hpp"
#include "MutableVector2D.hpp"
#include "Geodetic2D.hpp"
#include "Planet.hpp"

class EllipsoidalTileTessellator : public TileTessellator {
private:
  
  const std::string _textureFilename;
  
  static void addVertex(const Planet* planet,
                        std::vector<MutableVector3D>* vertices,
                        std::vector<MutableVector2D>* texCoords,
                        const Geodetic2D& g) {
    vertices->push_back( planet->toVector3D(g).asMutableVector3D() );
    
    
    Vector3D n = planet->geodeticSurfaceNormal(g);
    
    double s = atan2(n.y(), n.x()) / (M_PI * 2) + 0.5;
    double t = asin(n.z()) / M_PI + 0.5;
    
//    const double s = (g.longitude().degrees() + 180) / 360;
//    const double t = (g.latitude().degrees() + 90) / 180;
    
    MutableVector2D texCoord(s, 1-t);
    texCoords->push_back(texCoord);
    
  }
  
public:
  EllipsoidalTileTessellator(const std::string textureFilename) :
  _textureFilename(textureFilename)
  {
    
  }
  
  virtual ~EllipsoidalTileTessellator() { }
  
  virtual Mesh* createMesh(const RenderContext* rc,
                           const Tile* tile) const;
};

#endif
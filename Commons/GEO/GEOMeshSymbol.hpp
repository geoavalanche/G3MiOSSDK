//
//  GEOMeshSymbol.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 3/27/13.
//
//

#ifndef __G3MiOSSDK__GEOMeshSymbol__
#define __G3MiOSSDK__GEOMeshSymbol__

#include "GEOSymbol.hpp"

class Mesh;
class Geodetic2D;
class Color;
class Ellipsoid;

class GEOMeshSymbol : public GEOSymbol {
protected:

  Mesh* createLine2DMesh(const std::vector<Geodetic2D*>* coordinates,
                         const Color& lineColor,
                         float lineWidth,
                         double deltaHeight,
                         const Ellipsoid* ellipsoid) const;

  Mesh* createLines2DMesh(const std::vector<std::vector<Geodetic2D*>*>* coordinatesArray,
                          const Color& lineColor,
                          float lineWidth,
                          double deltaHeight,
                          const Ellipsoid* ellipsoid) const;


  virtual Mesh* createMesh(const G3MRenderContext* rc) const = 0;

public:

  void symbolize(const G3MRenderContext* rc,
                 const GEOSymbolizationContext& sc) const;


};

#endif

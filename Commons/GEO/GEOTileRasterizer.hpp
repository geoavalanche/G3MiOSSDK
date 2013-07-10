//
//  GEOTileRasterizer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 7/10/13.
//
//

#ifndef __G3MiOSSDK__GEOTileRasterizer__
#define __G3MiOSSDK__GEOTileRasterizer__

#include "TileRasterizer.hpp"
#include "QuadTree.hpp"

class GEORasterSymbol;


class GEOTileRasterizer : public TileRasterizer {
private:
  QuadTree _quadTree;

public:
  std::string getId() const {
    return "GEOTileRasterizer";
  }

  void rasterize(IImage* image,
                 const Tile* tile,
                 IImageListener* listener,
                 bool autodelete) const;

  void addSymbol(const GEORasterSymbol* symbol);

};

#endif

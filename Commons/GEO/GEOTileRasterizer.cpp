//
//  GEOTileRasterizer.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 7/10/13.
//
//

#include "GEOTileRasterizer.hpp"
#include "GEORasterSymbol.hpp"

#include "Tile.hpp"
#include "IImageListener.hpp"

void GEOTileRasterizer::addSymbol(const GEORasterSymbol* symbol) {
  const bool added = _quadTree.add(*symbol->getSector(),
                                   symbol);
  if (!added) {
    delete symbol;
  }
}

class GEOTileRasterizer_QuadTreeVisitor : public QuadTreeVisitor {
public:
  bool visitElement(const Sector& sector,
                    const void*   element) const {

    return true;
  }
};

void GEOTileRasterizer::rasterize(IImage* image,
                                  const Tile* tile,
                                  IImageListener* listener,
                                  bool autodelete) const {
  _quadTree.visitElements(tile->getSector(),
                          GEOTileRasterizer_QuadTreeVisitor());

  listener->imageCreated(image);
  if (autodelete) {
    delete listener;
  }
}

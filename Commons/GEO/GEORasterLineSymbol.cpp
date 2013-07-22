//
//  GEORasterLineSymbol.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 7/10/13.
//
//

#include "GEORasterLineSymbol.hpp"

#include "Tile.hpp"
#include "ICanvas.hpp"

GEORasterLineSymbol::GEORasterLineSymbol(const std::vector<Geodetic2D*>* coordinates,
                                         const GEOLine2DRasterStyle& style):
GEORasterSymbol( calculateSectorFromCoordinates(coordinates) ),
_coordinates( copyCoordinates(coordinates) ),
_style(style)
{
}


GEORasterLineSymbol::~GEORasterLineSymbol() {
  if (_coordinates != NULL) {
    const int size = _coordinates->size();

    for (int i = 0; i < size; i++) {
      const Geodetic2D* coordinate = _coordinates->at(i);
      delete coordinate;
    }

    delete _coordinates;
  }
}

void GEORasterLineSymbol::rasterize(ICanvas*                   canvas,
                                    const GEORasterProjection* projection) const {
  _style.apply(canvas);

  rasterLine(_coordinates,
             canvas,
             projection);
}

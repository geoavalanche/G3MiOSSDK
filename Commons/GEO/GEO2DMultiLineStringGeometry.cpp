//
//  GEO2DMultiLineStringGeometry.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/30/12.
//
//

#include "GEO2DMultiLineStringGeometry.hpp"

#include "Geodetic2D.hpp"
#include "GEOSymbolizer.hpp"
#include "GEORasterSymbolizer.hpp"
#include "GEO2DCoordinatesArrayData.hpp"

GEO2DMultiLineStringGeometry::GEO2DMultiLineStringGeometry(std::vector<std::vector<Geodetic2D*>*>* coordinatesArray)
{
  _coordinatesArrayData = (coordinatesArray == NULL) ? NULL : new GEO2DCoordinatesArrayData(coordinatesArray);
}


GEO2DMultiLineStringGeometry::~GEO2DMultiLineStringGeometry() {
//  const int coordinatesArrayCount = _coordinatesArray->size();
//  for (int i = 0; i < coordinatesArrayCount; i++) {
//    std::vector<Geodetic2D*>* coordinates = _coordinatesArray->at(i);
//    const int coordinatesCount = coordinates->size();
//    for (int j = 0; j < coordinatesCount; j++) {
//      Geodetic2D* coordinate = coordinates->at(j);
//      delete coordinate;
//    }
//    delete coordinates;
//  }
//
//  delete _coordinatesArray;

  if (_coordinatesArrayData != NULL) {
    _coordinatesArrayData->_release();
  }

#ifdef JAVA_CODE
  super.dispose();
#endif
}

std::vector<GEOSymbol*>* GEO2DMultiLineStringGeometry::createSymbols(const GEOSymbolizer* symbolizer) const {
  return symbolizer->createSymbols(this);
}

std::vector<GEORasterSymbol*>* GEO2DMultiLineStringGeometry::createRasterSymbols(const GEORasterSymbolizer* symbolizer) const {
  return symbolizer->createSymbols(this);
}

//
//  GEORasterPolygonSymbol.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 7/23/13.
//
//

#ifndef __G3MiOSSDK__GEORasterPolygonSymbol__
#define __G3MiOSSDK__GEORasterPolygonSymbol__

#include "GEORasterSymbol.hpp"

#include "GEO2DLineRasterStyle.hpp"
#include "GEO2DSurfaceRasterStyle.hpp"


class GEORasterPolygonSymbol : public GEORasterSymbol {
private:
#ifdef C_CODE
  const std::vector<Geodetic2D*>* _coordinates;
  const GEO2DLineRasterStyle      _lineStyle;
#endif
#ifdef JAVA_CODE
  private java.util.ArrayList<Geodetic2D> _coordinates;
  private final GEO2DLineRasterStyle      _lineStyle;
#endif

  const std::vector<std::vector<Geodetic2D*>*>* _holesCoordinatesArray;
  const GEO2DSurfaceRasterStyle                 _surfaceStyle;
  
public:
  void rasterize(ICanvas*                   canvas,
                 const GEORasterProjection* projection) const;


  GEORasterPolygonSymbol(const std::vector<Geodetic2D*>*               coordinates,
                         const std::vector<std::vector<Geodetic2D*>*>* holesCoordinatesArray,
                         const GEO2DLineRasterStyle&                   lineStyle,
                         const GEO2DSurfaceRasterStyle&                surfaceStyle);

};

#endif

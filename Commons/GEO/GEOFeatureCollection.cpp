//
//  GEOFeatureCollection.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/29/12.
//
//

#include "GEOFeatureCollection.hpp"

#include "GEOFeature.hpp"


GEOFeatureCollection::~GEOFeatureCollection() {
  const int featuresCount = _features.size();
  for (int i = 0; i < featuresCount; i++) {
    GEOFeature* feature = _features[i];
    delete feature;
  }

#ifdef JAVA_CODE
  super.dispose();
#endif

}

void GEOFeatureCollection::symbolize(const G3MRenderContext* rc,
                                     const GEOSymbolizer*    symbolizer,
                                     MeshRenderer*           meshRenderer,
                                     ShapesRenderer*         shapesRenderer,
                                     MarksRenderer*          marksRenderer,
                                     GEOTileRasterizer*      geoTileRasterizer) const {
  const int featuresCount = _features.size();
  for (int i = 0; i < featuresCount; i++) {
    GEOFeature* feature = _features[i];
    feature->symbolize(rc,
                       symbolizer,
                       meshRenderer,
                       shapesRenderer,
                       marksRenderer,
                       geoTileRasterizer);
  }
}

void GEOFeatureCollection::rasterize(const GEORasterSymbolizer* symbolizer,
                                     ICanvas* canvas,
                                     const GEORasterProjection* projection,
                                     int tileLevel) const {
  const int featuresCount = _features.size();
  for (int i = 0; i < featuresCount; i++) {
    GEOFeature* feature = _features[i];
    if (feature != NULL) {
      feature->rasterize(symbolizer,
                         canvas,
                         projection,
                         tileLevel);
    }
  }
}


long long GEOFeatureCollection::getCoordinatesCount() const {
  long long result = 0;
  const int featuresCount = _features.size();
  for (int i = 0; i < featuresCount; i++) {
    GEOFeature* feature = _features[i];
    result += feature->getCoordinatesCount();
  }
  return result;
}

//
//  GoogleMapsLayer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 3/8/13.
//
//

#ifndef __G3MiOSSDK__GoogleMapsLayer__
#define __G3MiOSSDK__GoogleMapsLayer__

#include "Layer.hpp"

class GoogleMapsLayer : public Layer {
private:
  const std::string _key;
  const Sector      _sector;

protected:

public:

  GoogleMapsLayer(const std::string& key,
                  const TimeInterval& timeToCache,
                  int initialLevel = 2,
                  LayerCondition* condition = NULL);

  URL getFeatureInfoURL(const Geodetic2D& position,
                        const Sector& sector) const;


  std::vector<Petition*> createTileMapPetitions(const G3MRenderContext* rc,
                                                const Tile* tile) const;
  
};

#endif

//
//  HereLayer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 3/7/13.
//
//

#ifndef __G3MiOSSDK__HereLayer__
#define __G3MiOSSDK__HereLayer__

#include "Layer.hpp"

#include <string>


class HereLayer : public Layer {
private:
  const Sector      _sector;
  const std::string _appId;
  const std::string _appCode;

public:

  HereLayer(const std::string& appId,
            const std::string& appCode,
            const TimeInterval& timeToCache,
            int initialLevel = 2,
            LayerCondition* condition = NULL);

  std::vector<Petition*> createTileMapPetitions(const G3MRenderContext* rc,
                                                const Tile* tile) const;

  URL getFeatureInfoURL(const Geodetic2D& position,
                        const Sector& sector) const;


};

#endif

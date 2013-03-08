//
//  MapBoxLayer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 3/8/13.
//
//

#ifndef __G3MiOSSDK__MapBoxLayer__
#define __G3MiOSSDK__MapBoxLayer__

#include "MercatorTiledLayer.hpp"

class MapBoxLayer : public MercatorTiledLayer {
private:

  static const std::vector<std::string> getSubdomains() {
    std::vector<std::string> result;
    result.push_back("a.");
    result.push_back("b.");
    result.push_back("c.");
    result.push_back("d.");
    return result;
  }

public:
  // https://tiles.mapbox.com/v3/dgd.map-v93trj8v/3/3/3.png
  // https://tiles.mapbox.com/v3/dgd.map-v93trj8v/7/62/48.png?updated=f0e992c
  
  MapBoxLayer(const std::string& mapKey,
              const TimeInterval& timeToCache,
              //int initialMapBoxLevel = 1,
              LayerCondition* condition = NULL) :
  MercatorTiledLayer("MapBoxLayer",
                     "http://",
                     "tiles.mapbox.com/v3/" + mapKey,
                     getSubdomains(),
                     "png",
                     timeToCache,
                     Sector::fullSphere(),
                     1, //initialMapBoxLevel,
                     17,
                     condition)
  {

  }

};

#endif

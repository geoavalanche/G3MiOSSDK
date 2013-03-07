//
//  TMSLayer.cpp
//  G3MiOSSDK
//
//  Created by Eduardo de la Montaña on 05/03/13.
//
//

#include "TMSLayer.hpp"


#include "LayerTilesRenderParameters.hpp"
#include "IStringBuilder.hpp"
#include "Petition.hpp"
#include "Tile.hpp"
#include "ILogger.hpp"
#include "IStringUtils.hpp"


TMSLayer::TMSLayer(const std::string& mapLayer,
         const URL& mapServerURL,
         const Sector& sector,
         const std::string& format,
         const std::string srs,
         const bool isTransparent,
         LayerCondition* condition,
         const TimeInterval& timeToCache,
                   const LayerTilesRenderParameters* parameters):

Layer(condition,
      mapLayer,
      timeToCache,
      (parameters == NULL)
      ? LayerTilesRenderParameters::createDefaultNonMercator(sector)
      : parameters),
_mapServerURL(mapServerURL),
_mapLayer(mapLayer),
_sector(sector),
_format(format),
_srs(srs),
_isTransparent(isTransparent)
{
}

std::vector<Petition*> TMSLayer::getMapPetitions(const G3MRenderContext* rc,
                                       const Tile* tile,
                                                 const Vector2I& tileTextureResolution) const {
  std::vector<Petition*> petitions;
  
  const Sector tileSector = tile->getSector();
  if (!_sector.touchesWith(tileSector)) {
    return petitions;
  }
  
  IStringBuilder* isb = IStringBuilder::newStringBuilder();  
  isb->addString(_mapServerURL.getPath());
  isb->addString(_mapLayer);
  isb->addString("/");
  isb->addInt(tile->getLevel());
  isb->addString("/");
  isb->addInt(tile->getColumn());
  isb->addString("/");
  isb->addInt(tile->getRow());
  isb->addString(".");
  isb->addString(IStringUtils::instance()->replaceSubstring(_format, "image/", ""));
  
  ILogger::instance()->logInfo(isb->getString());
  
  Petition *petition = new Petition(tileSector, URL(isb->getString(), false), _timeToCache, _isTransparent);
  petitions.push_back(petition);
  
	return petitions;
  
}

URL TMSLayer::getFeatureInfoURL(const Geodetic2D& g,
                                const Sector& sector) const {
  return URL::nullURL();
  
}

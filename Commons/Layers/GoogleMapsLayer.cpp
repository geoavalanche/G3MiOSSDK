//
//  GoogleMapsLayer.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 3/8/13.
//
//

#include "GoogleMapsLayer.hpp"

#include "Vector2I.hpp"
#include "LayerTilesRenderParameters.hpp"
#include "Tile.hpp"
#include "IStringBuilder.hpp"
#include "Petition.hpp"

GoogleMapsLayer::GoogleMapsLayer(const std::string& key,
                                 const TimeInterval& timeToCache,
                                 int initialLevel,
                                 LayerCondition* condition) :
Layer(condition,
      "GoogleMaps",
      timeToCache,
      new LayerTilesRenderParameters(Sector::fullSphere(),
                                     1,
                                     1,
                                     initialLevel,
                                     20,
                                     Vector2I(256, 256),
                                     LayerTilesRenderParameters::defaultTileMeshResolution(),
                                     true) ),
_key(key),
_sector(Sector::fullSphere())
{

}



URL GoogleMapsLayer::getFeatureInfoURL(const Geodetic2D& position,
                                       const Sector& sector) const {
  return URL();
}


std::vector<Petition*> GoogleMapsLayer::createTileMapPetitions(const G3MRenderContext* rc,
                                                               const Tile* tile) const {
  std::vector<Petition*> petitions;

  const Sector tileSector = tile->getSector();
  if (!_sector.touchesWith(tileSector)) {
    return petitions;
  }

  const Sector sector = tileSector.intersection(_sector);
  if (sector.getDeltaLatitude().isZero() ||
      sector.getDeltaLongitude().isZero() ) {
    return petitions;
  }


  IStringBuilder* isb = IStringBuilder::newStringBuilder();

  // http://maps.googleapis.com/maps/api/staticmap?center=New+York,NY&zoom=13&size=600x300&key=AIzaSyC9pospBjqsfpb0Y9N3E3uNMD8ELoQVOrc&sensor=false

  /*
   http://maps.googleapis.com/maps/api/staticmap
   ?center=New+York,NY
   &zoom=13
   &size=600x300
   &key=AIzaSyC9pospBjqsfpb0Y9N3E3uNMD8ELoQVOrc
   &sensor=false
   */

  isb->addString("http://maps.googleapis.com/maps/api/staticmap?sensor=false");

  isb->addString("&center=");
  isb->addDouble(tileSector.getCenter().latitude().degrees());
  isb->addString(",");
  isb->addDouble(tileSector.getCenter().longitude().degrees());

  const int level = tile->getLevel();
  isb->addString("&zoom=");
  isb->addInt(level);

  isb->addString("&size=");
  isb->addInt(_parameters->_tileTextureResolution._x);
  isb->addString("x");
  isb->addInt(_parameters->_tileTextureResolution._y);

  isb->addString("&format=jpg");


//  isb->addString("&maptype=roadmap);
//  isb->addString("&maptype=satellite");
  isb->addString("&maptype=hybrid");
//  isb->addString("&maptype=terrain");


  isb->addString("&key=");
  isb->addString(_key);


  const std::string path = isb->getString();

  delete isb;

  petitions.push_back( new Petition(tileSector,
                                    URL(path, false),
                                    _timeToCache,
                                    true) );
  
  return petitions;
}

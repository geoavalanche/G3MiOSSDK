//
//  WMSLayer.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 18/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "WMSLayer.hpp"
#include "Tile.hpp"
#include "Petition.hpp"

#include "IStringBuilder.hpp"
#include "LayerTilesRenderParameters.hpp"
#include "MercatorUtils.hpp"


WMSLayer::WMSLayer(const std::string& mapLayer,
                   const URL& mapServerURL,
                   const WMSServerVersion mapServerVersion,
                   const std::string& queryLayer,
                   const URL& queryServerURL,
                   const WMSServerVersion queryServerVersion,
                   const Sector& sector,
                   const std::string& format,
                   const std::string srs,
                   const std::string& style,
                   const bool isTransparent,
                   LayerCondition* condition,
                   const TimeInterval& timeToCache,
                   bool readExpired,
                   const LayerTilesRenderParameters* parameters):
Layer(condition,
      mapLayer,
      timeToCache,
      readExpired,
      (parameters == NULL)
      ? LayerTilesRenderParameters::createDefaultNonMercator(Sector::fullSphere())
      : parameters),
_mapLayer(mapLayer),
_mapServerURL(mapServerURL),
_mapServerVersion(mapServerVersion),
_queryLayer(queryLayer),
_queryServerURL(queryServerURL),
_queryServerVersion(queryServerVersion),
_sector(sector),
_format(format),
_srs(srs),
_style(style),
_isTransparent(isTransparent),
_extraParameter("")
{

}

WMSLayer::WMSLayer(const std::string& mapLayer,
                   const URL& mapServerURL,
                   const WMSServerVersion mapServerVersion,
                   const Sector& sector,
                   const std::string& format,
                   const std::string srs,
                   const std::string& style,
                   const bool isTransparent,
                   LayerCondition* condition,
                   const TimeInterval& timeToCache,
                   bool readExpired,
                   const LayerTilesRenderParameters* parameters):
Layer(condition,
      mapLayer,
      timeToCache,
      readExpired,
      (parameters == NULL)
      ? LayerTilesRenderParameters::createDefaultNonMercator(Sector::fullSphere())
      : parameters),
_mapLayer(mapLayer),
_mapServerURL(mapServerURL),
_mapServerVersion(mapServerVersion),
_queryLayer(mapLayer),
_queryServerURL(mapServerURL),
_queryServerVersion(mapServerVersion),
_sector(sector),
_format(format),
_srs(srs),
_style(style),
_isTransparent(isTransparent),
_extraParameter("")
{

}

double WMSLayer::toBBOXLongitude(const Angle& longitude) const {
  return (_parameters->_mercator) ? MercatorUtils::longitudeToMeters(longitude) : longitude._degrees;
}

double WMSLayer::toBBOXLatitude(const Angle& latitude) const {
  return (_parameters->_mercator) ? MercatorUtils::latitudeToMeters(latitude) : latitude._degrees;
}

std::vector<Petition*> WMSLayer::createTileMapPetitions(const G3MRenderContext* rc,
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

  const Vector2I tileTextureResolution = _parameters->_tileTextureResolution;

	//Server name
  std::string req = _mapServerURL.getPath();
	if (req[req.size() - 1] != '?') {
		req += '?';
	}

//  //If the server refer to itself as localhost...
//  const int localhostPos = req.find("localhost");
//  if (localhostPos != -1) {
//    req = req.substr(localhostPos+9);
//
//    const int slashPos = req.find("/", 8);
//    std::string newHost = req.substr(0, slashPos);
//
//    req = newHost + req;
//  }

  req += "REQUEST=GetMap&SERVICE=WMS";


  switch (_mapServerVersion) {
    case WMS_1_3_0:
    {
      req += "&VERSION=1.3.0";

      IStringBuilder* isb = IStringBuilder::newStringBuilder();

      isb->addString("&WIDTH=");
      isb->addInt(tileTextureResolution._x);
      isb->addString("&HEIGHT=");
      isb->addInt(tileTextureResolution._y);

      isb->addString("&BBOX=");
      isb->addDouble( toBBOXLatitude( sector.lower().latitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLongitude( sector.lower().longitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLatitude( sector.upper().latitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLongitude( sector.upper().longitude() ) );

      req += isb->getString();
      delete isb;

      req += "&CRS=EPSG:4326";

      break;
    }
    case WMS_1_1_0:
    default:
    {
      // default is 1.1.1
      req += "&VERSION=1.1.1";

      IStringBuilder* isb = IStringBuilder::newStringBuilder();

      isb->addString("&WIDTH=");
      isb->addInt(tileTextureResolution._x);
      isb->addString("&HEIGHT=");
      isb->addInt(tileTextureResolution._y);

      isb->addString("&BBOX=");
      isb->addDouble( toBBOXLongitude( sector.lower().longitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLatitude( sector.lower().latitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLongitude( sector.upper().longitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLatitude( sector.upper().latitude() ) );

      req += isb->getString();
      delete isb;
      break;
    }
  }

  req += "&LAYERS=" + _mapLayer;

	req += "&FORMAT=" + _format;

  if (_srs != "") {
    req += "&SRS=" + _srs;
  }
	else {
    req += "&SRS=EPSG:4326";
  }

  //Style
  if (_style != "") {
    req += "&STYLES=" + _style;
  }
	else {
    req += "&STYLES=";
  }

  //ASKING TRANSPARENCY
  if (_isTransparent) {
    req += "&TRANSPARENT=TRUE";
  }
  else {
    req += "&TRANSPARENT=FALSE";
  }

  if (_extraParameter.compare("") != 0) {
    req += "&";
    req += _extraParameter;
  }

//  printf("Request: %s\n", req.c_str());

  Petition *petition = new Petition(sector,
                                    URL(req, false),
                                    getTimeToCache(),
                                    getReadExpired(),
                                    _isTransparent);
  petitions.push_back(petition);

	return petitions;
}

URL WMSLayer::getFeatureInfoURL(const Geodetic2D& position,
                                const Sector& tileSector) const {
  if (!_sector.touchesWith(tileSector)) {
    return URL::nullURL();
  }

  const Sector sector = tileSector.intersection(_sector);

	//Server name
  std::string req = _queryServerURL.getPath();
	if (req[req.size()-1] != '?') {
		req += '?';
	}

  //If the server refer to itself as localhost...
  int pos = req.find("localhost");
  if (pos != -1) {
    req = req.substr(pos+9);

    int pos2 = req.find("/", 8);
    std::string newHost = req.substr(0, pos2);

    req = newHost + req;
  }

  req += "REQUEST=GetFeatureInfo&SERVICE=WMS";

  //SRS
  if (_srs != "") {
    req += "&SRS=" + _srs;
  }
	else {
    req += "&SRS=EPSG:4326";
  }

  switch (_queryServerVersion) {
    case WMS_1_3_0:
    {
      req += "&VERSION=1.3.0";

      IStringBuilder* isb = IStringBuilder::newStringBuilder();

      isb->addString("&WIDTH=");
      isb->addInt(_parameters->_tileTextureResolution._x);
      isb->addString("&HEIGHT=");
      isb->addInt(_parameters->_tileTextureResolution._y);

      isb->addString("&BBOX=");
      isb->addDouble( toBBOXLatitude( sector.lower().latitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLongitude( sector.lower().longitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLatitude( sector.upper().latitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLongitude( sector.upper().longitude() ) );

      req += isb->getString();

      delete isb;

      req += "&CRS=EPSG:4326";

      break;
    }
    case WMS_1_1_0:
    default:
    {
      // default is 1.1.1
      req += "&VERSION=1.1.1";

      IStringBuilder* isb = IStringBuilder::newStringBuilder();

      isb->addString("&WIDTH=");
      isb->addInt(_parameters->_tileTextureResolution._x);
      isb->addString("&HEIGHT=");
      isb->addInt(_parameters->_tileTextureResolution._y);

      isb->addString("&BBOX=");
      isb->addDouble( toBBOXLongitude( sector.lower().longitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLatitude( sector.lower().latitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLongitude( sector.upper().longitude() ) );
      isb->addString(",");
      isb->addDouble( toBBOXLatitude( sector.upper().latitude() ) );

      req += isb->getString();

      delete isb;
      break;
    }
  }
  req += "&LAYERS=" + _queryLayer;
  req += "&QUERY_LAYERS=" + _queryLayer;

  req += "&INFO_FORMAT=text/plain";

  const IMathUtils* mu = IMathUtils::instance();

  double u;
  double v;
  if (_parameters->_mercator) {
    u = sector.getUCoordinates(position.longitude());
    v = MercatorUtils::getMercatorV(position.latitude());
  }
  else {
    const Vector2D uv = sector.getUVCoordinates(position);
    u = uv._x;
    v = uv._y;
  }

  //X and Y
  //const Vector2D uv = sector.getUVCoordinates(position);
//  const int x = (int) mu->round( (uv._x * _parameters->_tileTextureResolution._x) );
//  const int y = (int) mu->round( (uv._y * _parameters->_tileTextureResolution._y) );
  const int x = (int) mu->round( (u * _parameters->_tileTextureResolution._x) );
  const int y = (int) mu->round( (v * _parameters->_tileTextureResolution._y) );
  // const int y = (int) mu->round( ((1.0 - uv._y) * _parameters->_tileTextureResolution._y) );

  IStringBuilder* isb = IStringBuilder::newStringBuilder();
  isb->addString("&X=");
  isb->addInt(x);
  isb->addString("&Y=");
  isb->addInt(y);
  req += isb->getString();
  delete isb;
  
	return URL(req, false);
}

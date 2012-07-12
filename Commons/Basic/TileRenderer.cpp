//
//  TileRenderer.cpp
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 12/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "TileRenderer.hpp"
#include "Tile.hpp"

#include "TileTessellator.hpp"
#include "TileTexturizer.hpp"

TileRenderer::~TileRenderer() {
  clearTopLevelTiles();
  
  delete _tessellator;
  delete _texturizer;
}

void TileRenderer::clearTopLevelTiles() {
  for (int i = 0; i < _topLevelTiles.size(); i++) {
    Tile* tile = _topLevelTiles[i];
    delete tile;
  }
  
  _topLevelTiles.clear();
}

void TileRenderer::createTopLevelTiles(const InitializationContext* ic) {
  int __diego_at_work;
  
  const Sector topSector(Geodetic2D(Angle::fromDegrees(-90), Angle::fromDegrees(-180)),
                         Geodetic2D(Angle::fromDegrees(90), Angle::fromDegrees(180)));
  const int K = 2;
  const int splitsByLatitude = 2 * K;
  const int splitsByLongitude = 4 * K;
  const int topLevel = 0;
  
  
  const Angle fromLatitude = topSector.lower().latitude();
  const Angle fromLongitude = topSector.lower().longitude();
  
  const Angle deltaLan = topSector.getDeltaLatitude();
  const Angle deltaLon = topSector.getDeltaLongitude();
  
  const Angle tileHeight = deltaLan.div(splitsByLatitude);
  const Angle tileWidth = deltaLon.div(splitsByLongitude);
  
//  for (int row = 0; row < splitsByLatitude; row++) {
  for (int row = K; row < K+1; row++) {
    const Angle tileLatFrom = tileHeight.times(row).add(fromLatitude);
    const Angle tileLatTo = tileLatFrom.add(tileHeight);
    
    //for (int col = 0; col < splitsByLongitude; col++) {
    for (int col = 2*K-1; col < 2*K; col++) {
      const Angle tileLonFrom = tileWidth.times(col).add(fromLongitude);
      const Angle tileLonTo = tileLonFrom.add(tileWidth);
      
      const Geodetic2D tileLower(tileLatFrom, tileLonFrom);
      const Geodetic2D tileUpper(tileLatTo, tileLonTo);
      const Sector sector(tileLower, tileUpper);
      
      Tile* tile = new Tile(sector, topLevel, row, col);
      _topLevelTiles.push_back(tile);
    }
  }
  
  ic->getLogger()->logInfo("Created %i top level tiles", _topLevelTiles.size());
}

void TileRenderer::initialize(const InitializationContext* ic) {
  clearTopLevelTiles();
  createTopLevelTiles(ic);
}

int TileRenderer::render(const RenderContext* rc) {
  
 // std::vector<Tile*> visibleTiles = getVisibleTiles(rc);
  
  for (int i = 0; i < _topLevelTiles.size(); i++) {
    Tile* tile = _topLevelTiles[i];
    tile->render(rc, _tessellator, _texturizer);
  }
  
  return MAX_TIME_TO_RENDER;
}

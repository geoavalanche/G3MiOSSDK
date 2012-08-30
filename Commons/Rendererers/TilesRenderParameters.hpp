//
//  TilesRenderParameters.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 08/08/12.
//
//

#ifndef G3MiOSSDK_TilesRenderParameters_hpp
#define G3MiOSSDK_TilesRenderParameters_hpp

#include "Sector.hpp"

class TilesRenderParameters {
public:
  const Sector _topSector;
  const int    _splitsByLatitude;
  const int    _splitsByLongitude;
  const int    _topLevel;
  const int    _maxLevel;
  const int    _tileTextureHeight;
  const int    _tileTextureWidth;
  const int    _tileResolution;
  const bool   _renderDebug;
  
  const bool   _useTilesSplitBudget;
  
  const bool   _forceTopLevelTilesRenderOnStart;
  
  TilesRenderParameters(const Sector topSector,
                        const int    splitsByLatitude,
                        const int    splitsByLongitude,
                        const int    topLevel,
                        const int    maxLevel,
                        const int    tileTextureHeight,
                        const int    tileTextureWidth,
                        const int    tileResolution,
                        const bool   renderDebug,
                        const bool   useTilesSplitBudget,
                        const bool   forceTopLevelTilesRenderOnStart) :
  _topSector(topSector),
  _splitsByLatitude(splitsByLatitude),
  _splitsByLongitude(splitsByLongitude),
  _topLevel(topLevel),
  _maxLevel(maxLevel),
  _tileTextureHeight(tileTextureHeight),
  _tileTextureWidth(tileTextureWidth),
  _tileResolution(tileResolution),
  _renderDebug(renderDebug),
  _useTilesSplitBudget(useTilesSplitBudget),
  _forceTopLevelTilesRenderOnStart(forceTopLevelTilesRenderOnStart)
  {
    
  }
  
  static TilesRenderParameters* createDefault(const bool renderDebug,
                                              const bool useTilesSplitBudget,
                                              const bool forceTopLevelTilesRenderOnStart) {
    const int K = 1;
    const int splitsByLatitude = 2 * K;
    const int splitsByLongitude = 4 * K;
    const int topLevel = 0;
    const int maxLevel = 17;
    const int tileTextureHeight = 256;
    const int tileTextureWidth = 256;
    const int tileResolution = 10;
    
    return new TilesRenderParameters(Sector::fullSphere(),
                                     splitsByLatitude,
                                     splitsByLongitude,
                                     topLevel,
                                     maxLevel,
                                     tileTextureHeight,
                                     tileTextureWidth,
                                     tileResolution,
                                     renderDebug,
                                     useTilesSplitBudget,
                                     forceTopLevelTilesRenderOnStart);
  }
  
  
  static TilesRenderParameters* createSingleSector(const bool renderDebug,
                                                   const bool useTilesSplitBudget,
                                                   const bool forceTopLevelTilesRenderOnStart) {
    const int splitsByLatitude = 1;
    const int splitsByLongitude = 1;
    const int topLevel = 0;
    const int maxLevel = 2;
    const int tileTextureHeight = 256;
    const int tileTextureWidth = 256;
    const int tRes = 10;

    //    Sector sector = Sector::fullSphere();
    //    Sector sector = Sector(Geodetic2D(Angle::fromDegrees(-90), Angle::fromDegrees(-180)),
    //                           Geodetic2D(Angle::fromDegrees(90), Angle::fromDegrees(180)));
    Sector sector = Sector(Geodetic2D(Angle::fromDegrees(0), Angle::fromDegrees(0)),
                           Geodetic2D(Angle::fromDegrees(90), Angle::fromDegrees(90)));
    
    return new TilesRenderParameters(sector,
                                     splitsByLatitude,
                                     splitsByLongitude,
                                     topLevel,
                                     maxLevel,
                                     tileTextureHeight,
                                     tileTextureWidth,
                                     tRes,
                                     renderDebug,
                                     useTilesSplitBudget,
                                     forceTopLevelTilesRenderOnStart);
  }
};

#endif

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
#include "Camera.hpp"
#include "ITimer.hpp"
#include "TilesRenderParameters.hpp"
#include "TouchEvent.hpp"
#include "LayerSet.hpp"
#include "VisibleSectorListener.hpp"
#include "IThreadUtils.hpp"
#include "DownloadPriority.hpp"
#include "ElevationDataProvider.hpp"
#include "LayerTilesRenderParameters.hpp"
#include "MercatorUtils.hpp"

#include "GPUProgramState.hpp"

#include <algorithm>

class VisibleSectorListenerEntry {
private:
  VisibleSectorListener* _listener;
  const long long        _stabilizationIntervalInMS;

  Sector* _lastSector;
  long long _whenNotifyInMS;

  ITimer*  _timer;

  ITimer* getTimer() {
    if (_timer == NULL) {
      _timer = IFactory::instance()->createTimer();
    }
    return _timer;
  }


public:
  VisibleSectorListenerEntry(VisibleSectorListener* listener,
                             const TimeInterval& stabilizationInterval) :
  _listener(listener),
  _stabilizationIntervalInMS(stabilizationInterval.milliseconds()),
  _lastSector(NULL),
  _timer(NULL),
  _whenNotifyInMS(0)
  {

  }

  void notifyListener(const Sector* visibleSector,
                      const G3MRenderContext* rc) const {
    const Geodetic3D cameraPosition = rc->getPlanet()->toGeodetic3D( rc->getCurrentCamera()->getCartesianPosition() );

    _listener->onVisibleSectorChange(*_lastSector, cameraPosition);
  }

  void tryToNotifyListener(const Sector* visibleSector,
                           const G3MRenderContext* rc) {
    if ( _stabilizationIntervalInMS == 0 ) {
      if ( (_lastSector == NULL) || (!_lastSector->isEqualsTo(*visibleSector)) ) {
        delete _lastSector;
        _lastSector = new Sector(*visibleSector);

        notifyListener(visibleSector, rc);
      }
    }
    else {
      const long long now = getTimer()->now().milliseconds();

      if ( (_lastSector == NULL) || (!_lastSector->isEqualsTo(*visibleSector)) ) {
        delete _lastSector;
        _lastSector = new Sector(*visibleSector);
        _whenNotifyInMS = now + _stabilizationIntervalInMS;
      }

      if (_whenNotifyInMS != 0) {
        if (now >= _whenNotifyInMS) {
          notifyListener(visibleSector, rc);

          _whenNotifyInMS = 0;
        }
      }
    }

  }

  ~VisibleSectorListenerEntry() {
    delete _listener;

    if (_timer != NULL) {
      IFactory::instance()->deleteTimer(_timer);
    }

    delete _lastSector;
  }
};


TileRenderer::TileRenderer(const TileTessellator* tessellator,
                           ElevationDataProvider* elevationDataProvider,
                           float verticalExaggeration,
                           TileTexturizer*  texturizer,
                           LayerSet* layerSet,
                           const TilesRenderParameters* parameters,
                           bool showStatistics,
                           long long texturePriority) :
_tessellator(tessellator),
_elevationDataProvider(elevationDataProvider),
_verticalExaggeration(verticalExaggeration),
_texturizer(texturizer),
_layerSet(layerSet),
_parameters(parameters),
_showStatistics(showStatistics),
_firstLevelTilesJustCreated(false),
_lastSplitTimer(NULL),
_lastCamera(NULL),
_firstRender(false),
_context(NULL),
_lastVisibleSector(NULL),
_texturePriority(texturePriority),
_allFirstLevelTilesAreTextureSolved(false),
_programState(NULL)
{
  _layerSet->setChangeListener(this);
}

void TileRenderer::recreateTiles() {
  pruneFirstLevelTiles();
  clearFirstLevelTiles();
  _firstRender = true;
  _allFirstLevelTilesAreTextureSolved = false;
  createFirstLevelTiles(_context);
}

class RecreateTilesTask : public GTask {
private:
  TileRenderer* _tileRenderer;
public:
  RecreateTilesTask(TileRenderer* tileRenderer) :
  _tileRenderer(tileRenderer)
  {
  }

  void run(const G3MContext* context) {
    _tileRenderer->recreateTiles();
  }
};

void TileRenderer::changed(const LayerSet* layerSet) {
  // recreateTiles();

  // recreateTiles() delete tiles, then meshes, and delete textures from the GPU so it has to be executed in the OpenGL thread
  _context->getThreadUtils()->invokeInRendererThread(new RecreateTilesTask(this), true);
}

TileRenderer::~TileRenderer() {
  clearFirstLevelTiles();

  delete _tessellator;
  delete _elevationDataProvider;
  delete _texturizer;
  delete _parameters;

  delete _lastSplitTimer;

  delete _lastVisibleSector;

  const int visibleSectorListenersCount = _visibleSectorListeners.size();
  for (int i = 0; i < visibleSectorListenersCount; i++) {
    VisibleSectorListenerEntry* entry = _visibleSectorListeners[i];
    delete entry;
  }
}

void TileRenderer::clearFirstLevelTiles() {
  const int firstLevelTilesCount = _firstLevelTiles.size();
  for (int i = 0; i < firstLevelTilesCount; i++) {
    Tile* tile = _firstLevelTiles[i];

    tile->toBeDeleted(_texturizer, _elevationDataProvider);

    delete tile;
  }

  _firstLevelTiles.clear();
}

#ifdef C_CODE
class SortTilesClass {
public:
  bool operator() (Tile* i, Tile* j) {
    const int rowI = i->getRow();
    const int rowJ = j->getRow();

    if (rowI < rowJ) {
      return true;
    }
    if (rowI > rowJ) {
      return false;
    }

    return ( i->getColumn() < j->getColumn() );
  }
} sortTilesObject;
#endif

void TileRenderer::sortTiles(std::vector<Tile*>& tiles) const {
#ifdef C_CODE
  std::sort(tiles.begin(),
            tiles.end(),
            sortTilesObject);
#endif
#ifdef JAVA_CODE
  java.util.Collections.sort(tiles, //
                             new java.util.Comparator<Tile>() {
                               @Override
                               public int compare(final Tile i,
                                                  final Tile j) {
                                 final int rowI = i.getRow();
                                 final int rowJ = j.getRow();
                                 if (rowI < rowJ) {
                                   return -1;
                                 }
                                 if (rowI > rowJ) {
                                   return 1;
                                 }

                                 final int columnI = i.getColumn();
                                 final int columnJ = j.getColumn();
                                 if (columnI < columnJ) {
                                   return -1;
                                 }
                                 if (columnI > columnJ) {
                                   return 1;
                                 }
                                 return 0;
                               }
                             });
#endif
}

void TileRenderer::createFirstLevelTiles(std::vector<Tile*>& firstLevelTiles,
                                         Tile* tile,
                                         int firstLevel,
                                         bool mercator) const {
  if (tile->getLevel() == firstLevel) {
    firstLevelTiles.push_back(tile);
  }
  else {
    const Sector sector = tile->getSector();
    const Geodetic2D lower = sector.lower();
    const Geodetic2D upper = sector.upper();

    const Angle splitLongitude = Angle::midAngle(lower.longitude(),
                                                 upper.longitude());

    const Angle splitLatitude = mercator
    /*                               */ ? MercatorUtils::calculateSplitLatitude(lower.latitude(),
                                                                                upper.latitude())
    /*                               */ : Angle::midAngle(lower.latitude(),
                                                          upper.latitude());


    std::vector<Tile*>* children = tile->createSubTiles(splitLatitude,
                                                        splitLongitude,
                                                        false);

    const int childrenSize = children->size();
    for (int i = 0; i < childrenSize; i++) {
      Tile* child = children->at(i);
      createFirstLevelTiles(firstLevelTiles, child, firstLevel, mercator);
    }

    delete children;
    delete tile;
  }
}

void TileRenderer::createFirstLevelTiles(const G3MContext* context) {

  const LayerTilesRenderParameters* parameters = _layerSet->getLayerTilesRenderParameters();
  if (parameters == NULL) {
    ILogger::instance()->logError("LayerSet returned a NULL for LayerTilesRenderParameters, can't create first-level tiles");
    return;
  }

  std::vector<Tile*> topLevelTiles;

  const Angle fromLatitude  = parameters->_topSector.lower().latitude();
  const Angle fromLongitude = parameters->_topSector.lower().longitude();

  const Angle deltaLan = parameters->_topSector.getDeltaLatitude();
  const Angle deltaLon = parameters->_topSector.getDeltaLongitude();

  const int topSectorSplitsByLatitude  = parameters->_topSectorSplitsByLatitude;
  const int topSectorSplitsByLongitude = parameters->_topSectorSplitsByLongitude;

  const Angle tileHeight = deltaLan.div(topSectorSplitsByLatitude);
  const Angle tileWidth  = deltaLon.div(topSectorSplitsByLongitude);

  for (int row = 0; row < topSectorSplitsByLatitude; row++) {
    const Angle tileLatFrom = tileHeight.times(row).add(fromLatitude);
    const Angle tileLatTo   = tileLatFrom.add(tileHeight);

    for (int col = 0; col < topSectorSplitsByLongitude; col++) {
      const Angle tileLonFrom = tileWidth.times(col).add(fromLongitude);
      const Angle tileLonTo   = tileLonFrom.add(tileWidth);

      const Geodetic2D tileLower(tileLatFrom, tileLonFrom);
      const Geodetic2D tileUpper(tileLatTo, tileLonTo);
      const Sector sector(tileLower, tileUpper);

      Tile* tile = new Tile(_texturizer, NULL, sector, 0, row, col);
      if (parameters->_firstLevel == 0) {
        _firstLevelTiles.push_back(tile);
      }
      else {
        topLevelTiles.push_back(tile);
      }
    }
  }

  if (parameters->_firstLevel > 0) {
    const int topLevelTilesSize = topLevelTiles.size();
    for (int i = 0; i < topLevelTilesSize; i++) {
      Tile* tile = topLevelTiles[i];
      createFirstLevelTiles(_firstLevelTiles, tile, parameters->_firstLevel, parameters->_mercator);
    }
  }

  sortTiles(_firstLevelTiles);

  context->getLogger()->logInfo("Created %d first level tiles", _firstLevelTiles.size());

  _firstLevelTilesJustCreated = true;
}

void TileRenderer::initialize(const G3MContext* context) {
  _context = context;

  clearFirstLevelTiles();
  createFirstLevelTiles(context);

  delete _lastSplitTimer;
  _lastSplitTimer = context->getFactory()->createTimer();

  _layerSet->initialize(context);
  _texturizer->initialize(context, _parameters);
  if (_elevationDataProvider != NULL) {
    _elevationDataProvider->initialize(context);
  }
}

bool TileRenderer::isReadyToRender(const G3MRenderContext *rc) {
  if (!_layerSet->isReady()) {
    return false;
  }

  if (_elevationDataProvider != NULL) {
    if (!_elevationDataProvider->isReadyToRender(rc)) {
      return false;
    }
  }

  if (_firstLevelTilesJustCreated) {
    _firstLevelTilesJustCreated = false;

    const int firstLevelTilesCount = _firstLevelTiles.size();

    if (_parameters->_forceFirstLevelTilesRenderOnStart) {
      TilesStatistics statistics;

      TileRenderContext trc(_tessellator,
                            _elevationDataProvider,
                            _texturizer,
                            _layerSet,
                            _parameters,
                            &statistics,
                            _lastSplitTimer,
                            true,
                            _texturePriority,
                            _verticalExaggeration);

      for (int i = 0; i < firstLevelTilesCount; i++) {
        Tile* tile = _firstLevelTiles[i];
        tile->prepareForFullRendering(rc, &trc);
      }
    }

    if (_texturizer != NULL) {
      for (int i = 0; i < firstLevelTilesCount; i++) {
        Tile* tile = _firstLevelTiles[i];
        _texturizer->justCreatedTopTile(rc, tile, _layerSet);
      }
    }
  }

  if (_parameters->_forceFirstLevelTilesRenderOnStart) {
    if (!_allFirstLevelTilesAreTextureSolved) {
      const int firstLevelTilesCount = _firstLevelTiles.size();
      for (int i = 0; i < firstLevelTilesCount; i++) {
        Tile* tile = _firstLevelTiles[i];
        if (!tile->isTextureSolved()) {
          return false;
        }
      }

      if (_tessellator != NULL) {
        if (!_tessellator->isReady(rc)) {
          return false;
        }
      }

      if (_texturizer != NULL) {
        if (!_texturizer->isReady(rc, _layerSet)) {
          return false;
        }
      }

      _allFirstLevelTilesAreTextureSolved = true;
    }
  }

  return true;
}

void TileRenderer::render(const G3MRenderContext* rc,
                          const GLState& parentState) {
  // Saving camera for use in onTouchEvent
  _lastCamera = rc->getCurrentCamera();
  
  _lastCamera->applyOnGPUProgramState(_programState); //Projection and Modelview
  
  TilesStatistics statistics;

  TileRenderContext trc(_tessellator,
                        _elevationDataProvider,
                        _texturizer,
                        _layerSet,
                        _parameters,
                        &statistics,
                        _lastSplitTimer,
                        _firstRender /* if first render, force full render */,
                        _texturePriority,
                        _verticalExaggeration);

  const int firstLevelTilesCount = _firstLevelTiles.size();

  if (_firstRender && _parameters->_forceFirstLevelTilesRenderOnStart) {
    // force one render pass of the firstLevelTiles tiles to make the (toplevel) textures
    // loaded as they will be used as last-chance fallback texture for any tile.
    _firstRender = false;

    for (int i = 0; i < firstLevelTilesCount; i++) {
      Tile* tile = _firstLevelTiles[i];
      tile->render(rc,
                   &trc,
                   parentState, &_programState,
                   NULL);
    }
  }
  else {
    std::list<Tile*> toVisit;
    for (int i = 0; i < firstLevelTilesCount; i++) {
      toVisit.push_back(_firstLevelTiles[i]);
    }

    while (toVisit.size() > 0) {
      std::list<Tile*> toVisitInNextIteration;

      for (std::list<Tile*>::iterator iter = toVisit.begin();
           iter != toVisit.end();
           iter++) {
        Tile* tile = *iter;

        tile->render(rc,
                     &trc,
                     parentState, &_programState,
                     &toVisitInNextIteration);
      }

      toVisit = toVisitInNextIteration;
    }
  }

  if (_showStatistics) {
    statistics.log( rc->getLogger() );
  }


  const Sector* renderedSector = statistics.getRenderedSector();
  if (renderedSector != NULL) {
    if ( (_lastVisibleSector == NULL) || !renderedSector->isEqualsTo(*_lastVisibleSector) ) {
      delete _lastVisibleSector;
      _lastVisibleSector = new Sector(*renderedSector);
    }
  }

  if (_lastVisibleSector != NULL) {
    const int visibleSectorListenersCount = _visibleSectorListeners.size();
    for (int i = 0; i < visibleSectorListenersCount; i++) {
      VisibleSectorListenerEntry* entry = _visibleSectorListeners[i];

      entry->tryToNotifyListener(_lastVisibleSector, rc);
    }
  }

}


bool TileRenderer::onTouchEvent(const G3MEventContext* ec,
                                const TouchEvent* touchEvent) {
  if (_lastCamera == NULL) {
    return false;
  }

  if (touchEvent->getType() == LongPress) {
    const Vector2I pixel = touchEvent->getTouch(0)->getPos();
    const Vector3D ray = _lastCamera->pixel2Ray(pixel);
    const Vector3D origin = _lastCamera->getCartesianPosition();

    const Planet* planet = ec->getPlanet();

    const Vector3D positionCartesian = planet->closestIntersection(origin, ray);
    if (positionCartesian.isNan()) {
      return false;
    }

    const Geodetic3D position = planet->toGeodetic3D(positionCartesian);

    const int firstLevelTilesCount = _firstLevelTiles.size();
    for (int i = 0; i < firstLevelTilesCount; i++) {
      const Tile* tile = _firstLevelTiles[i]->getDeepestTileContaining(position);
      if (tile != NULL) {
        ILogger::instance()->logInfo("Touched on %s", tile->description().c_str());
        if (_texturizer->onTerrainTouchEvent(ec, position, tile, _layerSet)) {
          return true;
        }
      }
    }

  }

  return false;
}


void TileRenderer::pruneFirstLevelTiles() {
  const int firstLevelTilesCount = _firstLevelTiles.size();
  for (int i = 0; i < firstLevelTilesCount; i++) {
    Tile* tile = _firstLevelTiles[i];
    tile->prune(_texturizer, _elevationDataProvider);
  }
}

void TileRenderer::addVisibleSectorListener(VisibleSectorListener* listener,
                                            const TimeInterval& stabilizationInterval) {
  _visibleSectorListeners.push_back( new VisibleSectorListenerEntry(listener,
                                                                    stabilizationInterval) );
}

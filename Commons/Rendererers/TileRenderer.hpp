//
//  TileRenderer.h
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 12/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_TileRenderer_h
#define G3MiOSSDK_TileRenderer_h

class Tile;
class TileTessellator;
class TileTexturizer;
class LayerSet;
class VisibleSectorListenerEntry;
class VisibleSectorListener;

#include "IStringBuilder.hpp"
#include "LeafRenderer.hpp"
#include "Sector.hpp"
#include "Tile.hpp"
#include "TileKey.hpp"
#include "Camera.hpp"
#include "LayerSet.hpp"


class TileRenderContext {
private:
  const TileTessellator*       _tessellator;
  TileTexturizer*              _texturizer;
  const TilesRenderParameters* _parameters;
  TilesStatistics*             _statistics;

  const LayerSet*              _layerSet;

  const bool _isForcedFullRender;

  ITimer* _lastSplitTimer; // timer to start every time a tile get splitted into subtiles
  
  long long _texturePriority;

public:
  TileRenderContext(const TileTessellator*       tessellator,
                    TileTexturizer*              texturizer,
                    const LayerSet*              layerSet,
                    const TilesRenderParameters* parameters,
                    TilesStatistics*             statistics,
                    ITimer*                      lastSplitTimer,
                    bool                         isForcedFullRender,
                    long long                    texturePriority) :
  _tessellator(tessellator),
  _texturizer(texturizer),
  _layerSet(layerSet),
  _parameters(parameters),
  _statistics(statistics),
  _lastSplitTimer(lastSplitTimer),
  _isForcedFullRender(isForcedFullRender),
  _texturePriority(texturePriority)
  {

  }

  const LayerSet* getLayerSet() const {
    return _layerSet;
  }

  const TileTessellator* getTessellator() const {
    return _tessellator;
  }

  TileTexturizer* getTexturizer() const {
    return _texturizer;
  }

  const TilesRenderParameters* getParameters() const {
    return _parameters;
  }

  TilesStatistics* getStatistics() const {
    return _statistics;
  }

  ITimer* getLastSplitTimer() const {
    return _lastSplitTimer;
  }

  bool isForcedFullRender() const {
    return _isForcedFullRender;
  }
  
  long long getTexturePriority() const {
    return _texturePriority;
  }

};


class TilesStatistics {
private:
  long               _tilesProcessed;
  long               _tilesVisible;
  long               _tilesRendered;

  static const int _maxLOD = 30;

  int _tilesProcessedByLevel[_maxLOD];
  int _tilesVisibleByLevel[_maxLOD];
  int _tilesRenderedByLevel[_maxLOD];

  int _splitsCountInFrame;
  int _buildersStartsInFrame;

  Sector* _renderedSector;

public:

  TilesStatistics() :
  _tilesProcessed(0),
  _tilesVisible(0),
  _tilesRendered(0),
  _splitsCountInFrame(0),
  _buildersStartsInFrame(0),
  _renderedSector(NULL)
  {
    for (int i = 0; i < _maxLOD; i++) {
      _tilesProcessedByLevel[i] = _tilesVisibleByLevel[i] = _tilesRenderedByLevel[i] = 0;
    }
  }

  ~TilesStatistics() {
    //    if (_buildersStartsInFrame > 0) {
    //      printf("buildersStartsInFrame=%d\n", _buildersStartsInFrame);
    //    }
    delete _renderedSector;
  }

  int getSplitsCountInFrame() const {
    return _splitsCountInFrame;
  }

  void computeSplitInFrame() {
    _splitsCountInFrame++;
  }

  int getBuildersStartsInFrame() const {
    return _buildersStartsInFrame;
  }

  void computeBuilderStartInFrame() {
    _buildersStartsInFrame++;
  }

  void computeTileProcessed(Tile* tile) {
    _tilesProcessed++;

    const int level = tile->getLevel();
    _tilesProcessedByLevel[level] = _tilesProcessedByLevel[level] + 1;
  }

  void computeVisibleTile(Tile* tile) {
    _tilesVisible++;

    const int level = tile->getLevel();
    _tilesVisibleByLevel[level] = _tilesVisibleByLevel[level] + 1;
  }

  void computeRenderedSector(Tile* tile) {
    const Sector sector = tile->getSector();
    if (_renderedSector == NULL) {
#ifdef C_CODE
      _renderedSector = new Sector( sector );
#endif
#ifdef JAVA_CODE
      _renderedSector = sector;
#endif
    }
    else {
      if (!_renderedSector->fullContains(sector)) {
        Sector* previous = _renderedSector;

#ifdef C_CODE
        _renderedSector = new Sector( _renderedSector->mergedWith(sector) );
#endif
#ifdef JAVA_CODE
        _renderedSector = _renderedSector.mergedWith(sector);
#endif

        delete previous;
      }
    }
  }

  void computeTileRendered(Tile* tile) {
    _tilesRendered++;

    const int level = tile->getLevel();
    _tilesRenderedByLevel[level] = _tilesRenderedByLevel[level] + 1;

    
    computeRenderedSector(tile);
  }

  const Sector* getRenderedSector() const {
    return _renderedSector;
  }

  //  bool equalsTo(const TilesStatistics& that) const {
  //    if (_tilesProcessed != that._tilesProcessed) {
  //      return false;
  //    }
  //    if (_tilesRendered != that._tilesRendered) {
  //      return false;
  //    }
  //    if (_tilesRenderedByLevel != that._tilesRenderedByLevel) {
  //      return false;
  //    }
  //    if (_tilesProcessedByLevel != that._tilesProcessedByLevel) {
  //      return false;
  //    }
  //    return true;
  //  }


  static std::string asLogString(const int m[], const int nMax) {

    bool first = true;
    IStringBuilder *isb = IStringBuilder::newStringBuilder();
    for(int i = 0; i < nMax; i++) {
      const int level   = i;
      const int counter = m[i];
      if (counter != 0){
        if (first) {
          first = false;
        }
        else {
          isb->addString(",");
        }
        isb->addString("L");
        isb->addInt(level);
        isb->addString(":");
        isb->addInt(counter);
      }
    }

    std::string s = isb->getString();
    delete isb;
    return s;
  }

  void log(const ILogger* logger) const {
    logger->logInfo("Tiles processed:%d (%s), visible:%d (%s), rendered:%d (%s).",
                    _tilesProcessed, asLogString(_tilesProcessedByLevel, _maxLOD).c_str(),
                    _tilesVisible,   asLogString(_tilesVisibleByLevel, _maxLOD).c_str(),
                    _tilesRendered,  asLogString(_tilesRenderedByLevel, _maxLOD).c_str());
  }

};


class TileRenderer: public LeafRenderer, LayerSetChangedListener {
private:
  const TileTessellator*       _tessellator;
  TileTexturizer*              _texturizer;
  LayerSet*                    _layerSet;
  const TilesRenderParameters* _parameters;
  const bool                   _showStatistics;
  bool                         _topTilesJustCreated;

#ifdef C_CODE
  const Camera*     _lastCamera;
  const G3MContext* _context;
#endif
#ifdef JAVA_CODE
  private Camera     _lastCamera;
  private G3MContext _context;
#endif

  std::vector<Tile*>     _topLevelTiles;

  ITimer* _lastSplitTimer;      // timer to start every time a tile get splitted into subtiles

  void clearTopLevelTiles();
  void createTopLevelTiles(const G3MContext* context);

  bool _firstRender;

  void pruneTopLevelTiles();

  Sector* _lastVisibleSector;

  std::vector<VisibleSectorListenerEntry*> _visibleSectorListeners;
  
  long long _texturePriority;

public:
  TileRenderer(const TileTessellator* tessellator,
               TileTexturizer*  texturizer,
               LayerSet* layerSet,
               const TilesRenderParameters* parameters,
               bool showStatistics,
               long long texturePriority);

  ~TileRenderer();

  void initialize(const G3MContext* context);

  void render(const G3MRenderContext* rc,
              const GLState& parentState);

  bool onTouchEvent(const G3MEventContext* ec,
                    const TouchEvent* touchEvent);

  void onResizeViewportEvent(const G3MEventContext* ec,
                             int width, int height) {

  }

  bool isReadyToRender(const G3MRenderContext* rc);


  void start() {
    _firstRender = true;
  }

  void stop() {
    _firstRender = false;
  }

  void onResume(const G3MContext* context) {

  }

  void onPause(const G3MContext* context) {
    recreateTiles();
  }

  void onDestroy(const G3MContext* context) {

  }

  void setEnable(bool enable) {
#ifdef C_CODE
    LeafRenderer::setEnable(enable);
#endif
#ifdef JAVA_CODE
    super.setEnable(enable);
#endif
    
    if (!enable) {
      pruneTopLevelTiles();
    }
  }

  void changed(const LayerSet* layerSet);

  void recreateTiles();

  /**
   Answer the visible-sector, it can be null if globe was not yet rendered.
   */
  const Sector* getVisibleSector() const {
    return _lastVisibleSector;
  }

  /**
   Add a listener for notification of visible-sector changes.

   @param stabilizationInterval How many time the visible-sector has to be settled (without changes) before triggering the event.  Useful for avoid process while the camera is being moved (as in animations).  If stabilizationInterval is zero, the event is triggered inmediatly. 
   */
  void addVisibleSectorListener(VisibleSectorListener* listener,
                                const TimeInterval& stabilizationInterval);

  /**
   Add a listener for notification of visible-sector changes.

   The event is triggered immediately without waiting for the visible-sector get settled.
   */
  void addVisibleSectorListener(VisibleSectorListener* listener) {
    addVisibleSectorListener(listener, TimeInterval::zero());
  }
  
  /**
   Change the download-priority used by Tiles (for downloading textures).
   */
  void setTexturePriority(long long texturePriority) {
    _texturePriority = texturePriority;
  }
  
  long long getTexturePriority() const {
    return _texturePriority;
  }

};


#endif

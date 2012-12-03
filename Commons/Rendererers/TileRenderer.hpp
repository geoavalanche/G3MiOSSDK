//
//  TileRenderer.h
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 12/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_TileRenderer_h
#define G3MiOSSDK_TileRenderer_h

#include "IStringBuilder.hpp"

#include "LeafRenderer.hpp"

class Tile;
class TileTessellator;
class TileTexturizer;
class LayerSet;

#include "Sector.hpp"
#include <map>

#include "Tile.hpp"
#include "TileKey.hpp"
#include "Camera.hpp"

#include "LayerSet.hpp"

//class TileCacheEntry {
//public:
//  Tile* _tile;
//  long  _timestamp;
//
//  TileCacheEntry(Tile* tile,
//                 long  timestamp) :
//  _tile(tile),
//  _timestamp(timestamp)
//  {
//
//  }
//
//  ~TileCacheEntry() {
//    if (_tile != NULL) {
//      delete _tile;
//    }
//  }
//};
//
//class TileRenderer;
//
//class TilesCache {
//private:
//  TileRenderer*                _tileRenderer;
//  const int                    _maxElements;
//  std::vector<TileCacheEntry*> _entries;
//
//  long _tsCounter;
//
//public:
//  TilesCache(TileRenderer* tileRenderer, int maxElements) :
//  _tileRenderer(tileRenderer),
//  _maxElements(maxElements),
//  _tsCounter(0)
//  {
//
//  }
//
//  Tile* getTile(const int level,
//                const int row, const int column);
//
//  void putTile(Tile* tile);
//
//};


class TileRenderContext {
private:
  const TileTessellator*       _tessellator;
  TileTexturizer*              _texturizer;
  const TilesRenderParameters* _parameters;
  TilesStatistics*             _statistics;
  
  const LayerSet*              _layerSet;
  
  const bool _isForcedFullRender;
  
  ITimer* _lastSplitTimer;      // timer to start every time a tile get splitted into subtiles
  
public:
  TileRenderContext(const TileTessellator*       tessellator,
                    TileTexturizer*              texturizer,
                    const LayerSet*              layerSet,
                    const TilesRenderParameters* parameters,
                    TilesStatistics*             statistics,
                    ITimer*                      lastSplitTimer,
                    bool                         isForcedFullRender) :
  _tessellator(tessellator),
  _texturizer(texturizer),
  _layerSet(layerSet),
  _parameters(parameters),
  _statistics(statistics),
  _lastSplitTimer(lastSplitTimer),
  _isForcedFullRender(isForcedFullRender)
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
  
};


class TilesStatistics {
private:
  long               _tilesProcessed;
  //std::map<int, int> _tilesProcessedByLevel;
  
  long               _tilesVisible;
  //std::map<int, int> _tilesVisibleByLevel;
  
  long               _tilesRendered;
  //std::map<int, int> _tilesRenderedByLevel;
  
  static const int _maxLOD = 30;
  
  int _tilesProcessedByLevel[_maxLOD];
  int _tilesVisibleByLevel[_maxLOD];
  int _tilesRenderedByLevel[_maxLOD];
  
  int _splitsCountInFrame;
  int _buildersStartsInFrame;
  
public:
  
  TilesStatistics() :
  _tilesProcessed(0),
  _tilesVisible(0),
  _tilesRendered(0),
  _splitsCountInFrame(0),
  _buildersStartsInFrame(0)
  {
    for(int i = 0; i < _maxLOD; i++){
      _tilesProcessedByLevel[i] = _tilesVisibleByLevel[i] = _tilesRenderedByLevel[i] = 0;
    }
  }
  
  ~TilesStatistics() {
//    if (_buildersStartsInFrame > 0) {
//      printf("buildersStartsInFrame=%d\n", _buildersStartsInFrame);
//    }
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
  
  void computeTileRendered(Tile* tile) {
    _tilesRendered++;
    
    const int level = tile->getLevel();
    _tilesRenderedByLevel[level] = _tilesRenderedByLevel[level] + 1;
  }
  
  bool equalsTo(const TilesStatistics& that) const {
    if (_tilesProcessed != that._tilesProcessed) {
      return false;
    }
    if (_tilesRendered != that._tilesRendered) {
      return false;
    }
    if (_tilesRenderedByLevel != that._tilesRenderedByLevel) {
      return false;
    }
    if (_tilesProcessedByLevel != that._tilesProcessedByLevel) {
      return false;
    }
    return true;
  }
  
//  static std::string asLogString(std::map<int, int> map) {
//    
//    bool first = true;
//#ifdef C_CODE
//    
//    IStringBuilder *isb = IStringBuilder::newStringBuilder();
//    for(std::map<int, int>::const_iterator i = map.begin();
//        i != map.end();
//        ++i ) {
//      const int level   = i->first;
//      const int counter = i->second;
//      
//      if (first) {
//        first = false;
//      }
//      else {
//        isb->add(",");
//      }
//      isb->add("L")->add(level)->add(":")->add(counter);
//    }
//    
//    std::string s = isb->getString();
//    delete isb;
//    return s;  
//#endif
//#ifdef JAVA_CODE
//    String res = "";
//    for (java.util.Map.Entry<Integer, Integer> i: map.entrySet()){
//		  final int level = i.getKey();
//		  final int counter = i.getValue();
//		  if (first){
//        first = false;
//		  }else{
//        res += ",";
//		  }
//		  res += "L" + level + ":" + counter;
//    }
//    return res;
//#endif
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
  const Camera*                _lastCamera;
#endif
#ifdef JAVA_CODE
  private Camera               _lastCamera;
#endif 

  std::vector<Tile*>     _topLevelTiles;
  
  ITimer* _lastSplitTimer;      // timer to start every time a tile get splitted into subtiles
  
  void clearTopLevelTiles();
  void createTopLevelTiles(const G3MContext* context);
  
  TilesStatistics _lastStatistics;
  
  bool _firstRender;

//  const InitializationContext* _initializationContext;
#ifdef C_CODE
  const G3MContext* _context;
#endif
#ifdef JAVA_CODE
  private G3MContext _context;
#endif

  void pruneTopLevelTiles();
  
public:
  TileRenderer(const TileTessellator* tessellator,
               TileTexturizer*  texturizer,
               LayerSet* layerSet,
               const TilesRenderParameters* parameters,
               bool showStatistics);
  
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

};


#endif

//
//  TileRenderer.h
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 12/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_TileRenderer_h
#define G3MiOSSDK_TileRenderer_h

#include "Renderer.hpp"

class Tile;
class TileTessellator;
class TileTexturizer;

#include "Sector.hpp"
//#include <vector>
#include <map>
#include <sstream>

#include "Tile.hpp"
#include "TileKey.hpp"
#include "Camera.hpp"


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
  
  const bool _isForcedFullRender;
  
  ITimer* _lastSplitTimer;      // timer to start every time a tile get splitted into subtiles
//  ITimer* _lastTexturizerTimer; // timer to start every time the texturizer is called
  
public:
  TileRenderContext(const TileTessellator*       tessellator,
                    TileTexturizer*              texturizer,
                    const TilesRenderParameters* parameters,
                    TilesStatistics*             statistics,
                    ITimer*                      lastSplitTimer,
//                    ITimer*                      lastTexturizerTimer,
                    bool                         isForcedFullRender) :
  _tessellator(tessellator),
  _texturizer(texturizer),
  _parameters(parameters),
  _statistics(statistics),
  _lastSplitTimer(lastSplitTimer),
//  _lastTexturizerTimer(lastTexturizerTimer),
  _isForcedFullRender(isForcedFullRender)
  {
    
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
  
//  ITimer* getLastTexturizerTimer() const {
//    return _lastTexturizerTimer;
//  }
  
  bool isForcedFullRender() const {
    return _isForcedFullRender;
  }
  
};


class TilesStatistics {
private:
  long               _tilesProcessed;
  std::map<int, int> _tilesProcessedByLevel;
  
  long               _tilesVisible;
  std::map<int, int> _tilesVisibleByLevel;
  
  long               _tilesRendered;
  std::map<int, int> _tilesRenderedByLevel;
  
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
  
  static std::string asLogString(std::map<int, int> map) {
    
    bool first = true;
#ifdef C_CODE
    std::ostringstream buffer;
    for(std::map<int, int>::const_iterator i = map.begin();
        i != map.end();
        ++i ) {
      const int level   = i->first;
      const int counter = i->second;
      
      if (first) {
        first = false;
      }
      else {
        buffer << ",";
      }
      buffer << "L" << level << ":" << counter;
    }
    
    return buffer.str();
#endif
#ifdef JAVA_CODE
    String res = "";
    for (java.util.Map.Entry<Integer, Integer> i: map.entrySet()){
		  final int level = i.getKey();
		  final int counter = i.getValue();
		  if (first){
        first = false;
		  }else{
        res += ",";
		  }
		  res += "L" + level + ":" + counter;
    }
    return res;
#endif
  }
  
  void log(const ILogger* logger) const {
    logger->logInfo("Tiles processed:%d (%s), visible:%d (%s), rendered:%d (%s).",
                    _tilesProcessed, asLogString(_tilesProcessedByLevel).c_str(),
                    _tilesVisible,   asLogString(_tilesVisibleByLevel).c_str(),
                    _tilesRendered,  asLogString(_tilesRenderedByLevel).c_str());
  }
  
};


class TileRenderer: public Renderer {
private:
  const TileTessellator*       _tessellator;
  TileTexturizer*              _texturizer;
  const TilesRenderParameters* _parameters;
  const bool                   _showStatistics;
  bool                         _topTilesJustCreated;
  const Camera*                _lastCamera;
  
  std::vector<Tile*>     _topLevelTiles;
  
  ITimer* _lastSplitTimer;      // timer to start every time a tile get splitted into subtiles
//  ITimer* _lastTexturizerTimer; // timer to start every time the texturizer is called
  
  void clearTopLevelTiles();
  void createTopLevelTiles(const InitializationContext* ic);
  
  TilesStatistics _lastStatistics;
  
#ifdef C_CODE  
  bool _firstRender;
  class DistanceToCenterTileComparison {
  private:
    const Camera* _camera;
    const Planet* _planet;
//    std::map<Geodetic2D, double> _distancesCache;
    std::map<TileKey, double> _distancesCache;
    
  public:
    DistanceToCenterTileComparison(const Camera *camera,
                                   const Planet *planet):
    _camera(camera),
    _planet(planet)
    {}
    
    void initialize() {
      _distancesCache.clear();
    }
    
    double getSquaredDistanceToCamera(const Tile* tile) {
//      const Geodetic2D center = tile->getSector().getCenter();
//      
//      double distance = _distancesCache[center];
//      if (distance == 0) {
//        const Vector3D cameraPosition = _camera->getPosition();
//        const Vector3D centerVec3 = _planet->toVector3D(center);
//        
//        distance = centerVec3.sub(cameraPosition).squaredLength();
//        
//        _distancesCache[center] = distance;
//      }
//      
//      return distance;
      
      const TileKey key = tile->getKey();
      double distance = _distancesCache[key];
      if (distance == 0) {
        const Geodetic2D center = tile->getSector().getCenter();
        const Vector3D cameraPosition = _camera->getPosition();
        const Vector3D centerVec3 = _planet->toVector3D(center);
        
        distance = centerVec3.sub(cameraPosition).squaredLength();
        
        _distancesCache[key] = distance;
      }
      
      return distance;

    }
    
    inline bool operator()(const Tile *t1,
                           const Tile *t2) {
      //      const Vector3D cameraPos = _camera->getPosition();
      //
      //      const Vector3D center1 = _planet->toVector3D(t1->getSector().getCenter());
      //      const Vector3D center2 = _planet->toVector3D(t2->getSector().getCenter());
      //
      //      const double dist1 = center1.sub(cameraPos).squaredLength();
      //      const double dist2 = center2.sub(cameraPos).squaredLength();
      const double dist1 = getSquaredDistanceToCamera(t1);
      const double dist2 = getSquaredDistanceToCamera(t2);
      return (dist1 < dist2);
    }
  };
#endif

#ifdef JAVA_CODE
    private static class DistanceToCenterTileComparison implements java.util.Comparator<Tile> {
        private final Camera _camera;
        private Planet _planet;
        
        public DistanceToCenterTileComparison(Camera camera, Planet planet) {
            _camera = camera;
            _planet = planet;
        }
      
        public void initialize() {}
        
        public int compare(Tile t1, Tile t2) {
            final Vector3D cameraPos = _camera.getPosition();
            
            final Vector3D center1 = _planet.toVector3D(t1.getSector().getCenter());
            final Vector3D center2 = _planet.toVector3D(t2.getSector().getCenter());
            
            final double dist1 = center1.sub(cameraPos).squaredLength();
            final double dist2 = center2.sub(cameraPos).squaredLength();
            
            if (dist1 < dist2) {
                return -1;
            }
            else if (dist1 > dist2) {
                return 1;
            }
            return 0;
        }
    }
#endif
  
  
  //bool isTile1ClosestToCameraThanTile2(Tile *t1, Tile *t2) const;
  
  
public:
  TileRenderer(const TileTessellator* tessellator,
               TileTexturizer*  texturizer,
               const TilesRenderParameters* parameters,
               bool showStatistics) :
  _tessellator(tessellator),
  _texturizer(texturizer),
  _parameters(parameters),
  _showStatistics(showStatistics),
  _lastStatistics(),
  _topTilesJustCreated(false),
  _lastSplitTimer(NULL),
//  _lastTexturizerTimer(NULL),
  _lastCamera(NULL),
  _firstRender(false)
  {
    
  }
  
  ~TileRenderer();
  
  void initialize(const InitializationContext* ic);
  
  int render(const RenderContext* rc);
  
  bool onTouchEvent(const EventContext* ec,
                    const TouchEvent* touchEvent);
  
  void onResizeViewportEvent(const EventContext* ec,
                             int width, int height) {
    
  }
  
  bool isReadyToRender(const RenderContext* rc);

  
  void start() {
    _firstRender = true;
  }
  
  void stop() {
    _firstRender = false;
  }

};


#endif

//
//  Tile.h
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 12/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_Tile_h
#define G3MiOSSDK_Tile_h

#include "Sector.hpp"
#include "Box.hpp"

#include <list>

class RenderContext;
class Mesh;
class TileTessellator;
class TileTexturizer;
class TilesRenderParameters;
class ITimer;
class TilesStatistics;
class TileRenderContext;
class TileKey;

class Tile {
private:
  const Sector _sector;
  const int    _level;
  const int    _row;
  const int    _column;
  
  Mesh* _tessellatorMesh;
  Mesh* _debugMesh;
  Mesh* _texturizerMesh;
  
  Tile* _parent;
  bool _textureSolved;
  std::vector<Tile*>* _subtiles;
  
  ITimer* _texturizerTimer;
  
  bool _justCreatedSubtiles;
  
  
  inline Mesh* getTessellatorMesh(const RenderContext* rc,
                                  const TileRenderContext* trc);
  
  Mesh* getDebugMesh(const RenderContext* rc,
                     const TileRenderContext* trc);
  
  inline bool isVisible(const RenderContext* rc,
                        const TileRenderContext* trc);
  
  inline bool meetsRenderCriteria(const RenderContext* rc,
                                  const TileRenderContext* trc);
  
  inline std::vector<Tile*>* createSubTiles();
  
  inline void rawRender(const RenderContext* rc,
                        const TileRenderContext* trc);
  
  void debugRender(const RenderContext* rc,
                   const TileRenderContext* trc);
  
  inline Tile* createSubTile(const Angle& lowerLat, const Angle& lowerLon,
                             const Angle& upperLat, const Angle& upperLon,
                             const int level,
                             const int row, const int column);
  
  
  inline std::vector<Tile*>* getSubTiles();
  
  inline void prune(const TileRenderContext* trc);
  
  Tile(const Tile& that);
  
public:
  Tile(Tile* parent,
       const Sector& sector,
       int level,
       int row,
       int column):
  _parent(parent),
  _sector(sector),
  _level(level),
  _row(row),
  _column(column),
  _tessellatorMesh(NULL),
  _debugMesh(NULL),
  _texturizerMesh(NULL),
  _textureSolved(false),
  _subtiles(NULL),
  _justCreatedSubtiles(false),
  _texturizerTimer(NULL)
  {
  }
  
  ~Tile();
  
  
  Sector getSector() const {
    return _sector;
  }
  
  int getLevel() const {
    return _level;
  }
  
  int getRow() const {
    return _row;
  }
  
  int getColumn() const {
    return _column;
  }
  
  Mesh* getTexturizerMesh() const {
    return _texturizerMesh;
  }
  
  void setTextureSolved(bool textureSolved);
  
  bool isTextureSolved() const {
    return _textureSolved;
  }
  
  Tile* getParent() const {
    return _parent;
  }
  
  void render(const RenderContext* rc,
              const TileRenderContext* trc,
              std::list<Tile*>* toVisitInNextIteration);
  
  const TileKey getKey() const;
};

#endif

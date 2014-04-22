//
//  LayerSet.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_LayerSet
#define G3MiOSSDK_LayerSet

#include <vector>
#include <string>

class Layer;
class ChangedListener;
class G3MContext;
class TileImageProvider;
class G3MRenderContext;
class LayerTilesRenderParameters;
class Tile;
class G3MEventContext;
class Geodetic3D;
class RenderState;
class Petition;


class LayerSet {
private:
  std::vector<Layer*> _layers;
  
  ChangedListener* _listener;
  
  std::vector<std::string> _errors;
  
  void layersChanged() const;

#ifdef C_CODE
  mutable const G3MContext* _context;
#endif
#ifdef JAVA_CODE
  private G3MContext _context;
#endif

  mutable TileImageProvider* _tileImageProvider;

  TileImageProvider* createTileImageProvider(const G3MRenderContext* rc,
                                             const LayerTilesRenderParameters* layerTilesRenderParameters,
                                             const Tile* tile) const;

public:
  LayerSet() :
  _listener(NULL),
  _context(NULL),
  _tileImageProvider(NULL)
  {
  }
  
  ~LayerSet();
  
  void removeAllLayers(const bool deleteLayers);
  
  void addLayer(Layer* layer);

  bool onTerrainTouchEvent(const G3MEventContext* ec,
                           const Geodetic3D& g3d,
                           const Tile* tile) const;
  
  RenderState getRenderState();
  
  void initialize(const G3MContext* context)const;
  
  int size() const {
    return _layers.size();
  }
  
  void layerChanged(const Layer* layer) const;
  
  void setChangeListener(ChangedListener* listener);

  Layer* getLayer(int index) const;

  Layer* getLayerByTitle(const std::string& title) const;

  LayerTilesRenderParameters* createLayerTilesRenderParameters(std::vector<std::string>& errors) const;

  bool isEquals(const LayerSet* that) const;

  void takeLayersFrom(LayerSet* that);

  void disableAllLayers();

  std::vector<Petition*> createTileMapPetitions(const G3MRenderContext* rc,
                                                const LayerTilesRenderParameters* layerTilesRenderParameters,
                                                const Tile* tile) const;

  TileImageProvider* getTileImageProvider(const G3MRenderContext* rc,
                                          const LayerTilesRenderParameters* layerTilesRenderParameters,
                                          const Tile* tile) const;

};

#endif

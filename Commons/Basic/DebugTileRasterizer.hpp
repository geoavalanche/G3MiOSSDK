//
//  DebugTileRasterizer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 7/8/13.
//
//

#ifndef __G3MiOSSDK__DebugTileRasterizer__
#define __G3MiOSSDK__DebugTileRasterizer__

#include "CanvasTileRasterizer.hpp"
#include "GFont.hpp"
#include "Color.hpp"

class ICanvas;
class Sector;

class DebugTileRasterizer : public CanvasTileRasterizer {
private:

#ifdef C_CODE
  const GFont _font;
#endif
#ifdef JAVA_CODE
  private final GFont _font;
#endif
  const Color _color;


  std::string getTileKeyLabel(const Tile* tile) const;

  std::string getSectorLabel1(const Sector& sector) const;
  std::string getSectorLabel2(const Sector& sector) const;
  std::string getSectorLabel3(const Sector& sector) const;
  std::string getSectorLabel4(const Sector& sector) const;

public:
  DebugTileRasterizer();

  ~DebugTileRasterizer();

  std::string getId() const {
    return "DebugTileRasterizer";
  }

  void rasterize(IImage* image,
                 const Tile* tile,
                 bool mercator,
                 IImageListener* listener,
                 bool autodelete) const;
  
};

#endif

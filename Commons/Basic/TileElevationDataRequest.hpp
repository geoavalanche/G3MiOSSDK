//
//  TileElevationDataListener.h
//  G3MiOSSDK
//
//  Created by Jose Miguel SN on 23/04/13.
//
//

#ifndef __G3MiOSSDK__TileElevationDataRequest__
#define __G3MiOSSDK__TileElevationDataRequest__

#include <iostream>

#include "ElevationDataProvider.hpp"
#include "Tile.hpp"
#include "Vector2I.hpp"

class TileElevationDataRequest {
private:
  Tile*                  _tile;
  long long              _requestID;
  Vector2I _resolution;
  ElevationDataProvider* _provider;
  
  
  class TileElevationDataRequestListener : public IElevationDataListener {
  public:
    TileElevationDataRequest* _request;
    
    TileElevationDataRequestListener(TileElevationDataRequest* request);
    
    void onData(const Sector& sector,
                const Vector2I& resolution,
                ElevationData* elevationData);
    
    void onError(const Sector& sector,
                 const Vector2I& resolution);
    
    void onCancel(const Sector& sector,
                  const Vector2I& resolution);
    
  };
  
  TileElevationDataRequestListener* _listener;
  
public:
  TileElevationDataRequest(Tile* tile,
                           const Vector2I& resolution,
                           ElevationDataProvider* provider);
  
  ~TileElevationDataRequest();
  
  void onData(const Sector& sector,
              const Vector2I& resolution,
              ElevationData* elevationData);
  
  void onError(const Sector& sector,
               const Vector2I& resolution);
  
  void onCancel(const Sector& sector,
                const Vector2I& resolution);
  
  void sendRequest();
  
  void cancelRequest();
  
};

#endif /* defined(__G3MiOSSDK__TileElevationDataRequest__) */

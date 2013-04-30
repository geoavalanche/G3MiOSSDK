//
//  WMSBillElevationDataProvider.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 2/17/13.
//
//

#ifndef __G3MiOSSDK__WMSBillElevationDataProvider__
#define __G3MiOSSDK__WMSBillElevationDataProvider__

#include "ElevationDataProvider.hpp"
#include <stddef.h>

#include "Vector2I.hpp"
#include "URL.hpp"
#include "Sector.hpp"

class IDownloader;

class WMSBillElevationDataProvider : public ElevationDataProvider {
private:
  IDownloader* _downloader;
  URL          _url;
  Sector       _sector;

public:
  WMSBillElevationDataProvider(const URL& url,
                               const Sector& sector) :
  _url(url),
  _sector(sector),
  _downloader(NULL) {

  }

  bool isReadyToRender(const G3MRenderContext* rc) {
    return true;
  }

  void initialize(const G3MContext* context);

  const long long requestElevationData(const Sector& sector,
                                       const Vector2I& extent,
                                       IElevationDataListener* listener,
                                       bool autodeleteListener);

  void cancelRequest(const long long requestId);
  
  std::vector<const Sector*> getSectors() const{
    std::vector<const Sector*> sectors;
    sectors.push_back(&_sector);
    return sectors;
  }
  
  const Vector2I getMinResolution() const{
    int WORKING_JM;
    return Vector2I::zero();
  }
  
//  ElevationData* createSubviewOfElevationData(ElevationData* elevationData,
//                                              const Sector& sector,
//                                              const Vector2I& extent) const;

};

#endif

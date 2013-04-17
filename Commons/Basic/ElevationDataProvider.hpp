//
//  ElevationDataProvider.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 2/17/13.
//
//

#ifndef __G3MiOSSDK__ElevationDataProvider__
#define __G3MiOSSDK__ElevationDataProvider__

class Sector;
class Vector2I;
class ElevationData;
class G3MContext;
class G3MRenderContext;

#include <vector.h>

class IElevationDataListener {
public:
#ifdef C_CODE
  virtual ~IElevationDataListener() { }
#endif
#ifdef JAVA_CODE
  public void dispose();
#endif

  virtual void onData(const Sector& sector,
                      const Vector2I& resolution,
                      ElevationData* elevationData) = 0;

  virtual void onError(const Sector& sector,
                       const Vector2I& resolution) = 0;

};


class ElevationDataProvider {
public:

  virtual ~ElevationDataProvider() {

  }

  virtual bool isReadyToRender(const G3MRenderContext* rc) = 0;

  virtual void initialize(const G3MContext* context) = 0;

  virtual const long long requestElevationData(const Sector& sector,
                                               const Vector2I& resolution,
                                               IElevationDataListener* listener,
                                               bool autodeleteListener) = 0;

  virtual void cancelRequest(const long long requestId) = 0;
  
  virtual std::vector<const Sector*> getSectors() const = 0;
  
  virtual Vector2I getMinResolution() const = 0;
  
  virtual ElevationData* createSubviewOfElevationData(ElevationData* elevationData,
                                                      const Sector& sector,
                                                      const Vector2I& resolution) const = 0;
  
};

#endif

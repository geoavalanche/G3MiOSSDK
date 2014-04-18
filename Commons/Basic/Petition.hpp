//
//  Petition.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 20/08/12.
//
//

#ifndef __G3MiOSSDK__Petition__
#define __G3MiOSSDK__Petition__

#include "Sector.hpp"
#include "URL.hpp"
class IImage;


class Petition {
private:
  const Sector _sector;
  IImage*      _image;
  const float  _layerTransparency;

#ifdef C_CODE
  const URL _url;
#endif
#ifdef JAVA_CODE
  final private URL _url;
#endif

  const long long _timeToCacheInMS;
  const bool      _readExpired;

  const bool _isTransparent;

  Petition(const Petition& that);

  void operator=(const Petition& that);

public:

  Petition(const Sector& sector,
           const URL& url,
           const TimeInterval& timeToCache,
           bool readExpired,
           bool isTransparent,
           float layerTransparency):
  _sector(sector),
  _url(url),
  _timeToCacheInMS(timeToCache._milliseconds),
  _readExpired(readExpired),
  _isTransparent(isTransparent),
  _image(NULL),
  _layerTransparency(layerTransparency)
  {
  }

  ~Petition() {
    releaseImage();
  }

  void releaseImage();

  const URL getURL() const {
    return _url;
  }

  const Sector getSector() const {
    return _sector;
  }

  void setImage(IImage* image) {
    releaseImage();
    _image = image;
  }

  IImage* getImage() const {
    return _image;
  }

  const TimeInterval getTimeToCache() const {
    return TimeInterval::fromMilliseconds(_timeToCacheInMS);
  }

  bool getReadExpired() const {
    return _readExpired;
  }

  bool isTransparent() const {
    return _isTransparent;
  }

  const std::string description() const;
#ifdef JAVA_CODE
  @Override
  public String toString() {
    return description();
  }
#endif

  float getLayerTransparency() const{
    return _layerTransparency;
  }
  
};

#endif

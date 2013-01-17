//
//  Petition.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 20/08/12.
//
//

#ifndef __G3MiOSSDK__Petition__
#define __G3MiOSSDK__Petition__

#include <string>
#include <vector>

#include "URL.hpp"
#include "Sector.hpp"
#include "IImage.hpp"

#include "TimeInterval.hpp"

class Tile;
class Rectangle;
class Sector;
class IFactory;
class MutableVector2D;

class Petition {
private:
  const Sector* _sector;
#ifdef C_CODE
  const URL     _url;
  const IImage* _image;
#endif
#ifdef JAVA_CODE
  final private URL _url; //Conversor creates class "Url"
  private IImage _image;
#endif

  const TimeInterval _timeToCache;
  
  Petition(const Petition& that);
  
  void operator=(const Petition& that);
  
public:
  
  Petition(const Sector& sector,
           const URL& url,
           const TimeInterval& timeToCache):
  _sector(new Sector(sector)),
  _url(url),
  _timeToCache(timeToCache),
  _image(NULL)
  {
  }
  
  ~Petition(){
#ifdef C_CODE
    delete _sector;
#endif
    releaseImage();
  }
  
  void releaseImage() {
    delete _image;
    _image = NULL;
  }
  
  bool hasImage() const {
    return (_image != NULL);
  }
  
  const URL getURL() const {
    return _url;
  }
  
  Sector getSector() const {
    return *_sector;
  }
  
  void setImage(const IImage* image) {
    releaseImage();
    _image = image;
  }
  
  const IImage* getImage() const {
    return _image;
  }

  const TimeInterval getTimeToCache() const {
    return _timeToCache;
  }

  const std::string description() const;
  
};

#endif

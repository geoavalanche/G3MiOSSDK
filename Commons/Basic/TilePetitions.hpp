//
//  TilePetitions.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 17/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_TilePetitions_hpp
#define G3MiOSSDK_TilePetitions_hpp

#include <string>
#include <vector>

#include "Sector.hpp"
#include "IDownloadListener.hpp"
#include "IDownloader.hpp"
#include "GLTextureID.hpp"

class Tile;
class Rectangle;
class Sector;
class IFactory;
class MutableVector2D;
class TileImagesTileTexturizer;

class Petition {
private:
  const URL         _url;
  const Sector*     _sector;
  const ByteBuffer* _buffer;
  long              _downloadID;
  const bool        _transparentImage;
  
public:
  
  Petition(Sector sector,
           URL url,
           bool transparent):
  _sector(new Sector(sector)),
  _url(url),
  _transparentImage(transparent),
  _buffer(NULL),
  _downloadID(-1)
  {
  }
  
  ~Petition(){ 
    delete _sector;
    releaseData();
  }
  
  void releaseData(){ 
    if (_buffer != NULL) {
      delete _buffer;
      _buffer = NULL;
    }
  }
  
  long getDownloadID() const {
    return _downloadID;
  }
  
  bool hasByteBuffer() const{
    return _buffer != NULL;
  }
  
  bool isTransparent() const{
    return _transparentImage;
  }
  
  void setDownloadID(long id){
    _downloadID = id;
  }

  const URL getURL() const {
    return _url;
  }
  
  Sector getSector() const {
    return *_sector;
  }
  
  void setByteBuffer(const ByteBuffer* buffer) {
    if (_buffer != NULL) {
      delete _buffer;
    }
    _buffer = buffer;
  }

  const ByteBuffer* getByteBuffer() const {
    return _buffer;
  }
  
  const std::string description() const;
  
};


class TilePetitions: public IDownloadListener {
  
  const int    _level;
  const int    _row;
  const int    _column;
  const Sector _tileSector;

  std::vector<Petition*> _petitions;
  
  GLTextureID _texID;      //TEXTURE ID ONCE IS FINISHED
  
  int _downloadsCounter;
  int _errorsCounter;
  
  TilePetitions(const TilePetitions& that);
  
  Rectangle* getImageRectangleInTexture(const Sector& wholeSector, 
                                       const Sector& imageSector,
                                       int texWidth, int texHeight) const;
  
  Petition* getPetition(int i) { return _petitions[i];}
  
  int getNumPetitions() { return _petitions.size();}
  
public:
  
  TilePetitions(const int level,
                const int row,
                const int column,
                const Sector sector,
                const std::vector<Petition*>& petitions):
  _level(level),
  _row(row),
  _column(column),
  _tileSector(sector),
  _downloadsCounter(0),
  _errorsCounter(0),
  _petitions(petitions),
  _texID(-1)
  {
    removeUnnecesaryPetitions();
  }
  
  ~TilePetitions()
  {
    for (int i = 0; i < _petitions.size(); i++) {
      delete _petitions[i];
    }
  }
  
  void requestToNet(IDownloader* downloader, int priority);
  //void requestToCache(Downloader& downloader);
  void cancelPetitions(IDownloader* downloader);
  
  int getLevel() const {
    return _level;
  }
  
  int getRow() const {
    return _row;
  }
  
  int getColumn() const {
    return _column;
  }
  
  Sector getSector() const{ 
    return _tileSector;
  }
  
  void createTexture(TexturesHandler* texturesHandler, const IFactory* factory, int width, int height);
  
  GLTextureID getTexID() const{ return _texID;}

  std::string getPetitionsID() const;
  
  bool allFinished() const;
  
  void onDownload(const Response* response);
  void onError(const Response* e);
  void onCancel(const URL* url);
  
  void onCanceledDownload(const Response* response) {
  }

  void removeUnnecesaryPetitions();
  
};

#endif

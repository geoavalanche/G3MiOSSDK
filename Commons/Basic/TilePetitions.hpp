//
//  TilePetitions.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 17/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_TilePetitions_hpp
#define G3MiOSSDK_TilePetitions_hpp

#include "IDownloadListener.hpp"
#include "MutableVector2D.hpp"
#include "TileRenderer.hpp"

#include <string>
#include <vector>

#include "Tile.hpp"
#include "IFactory.hpp"

class TileImagesTileTexturizer;

class Petition {
  const std::string _url;
  const Sector *_sector;
  ByteBuffer* _bb;
  long _downloadID;
  
public:
  
  Petition(Sector s, std::string url): _url(url), 
  _sector(new Sector(s)),
  _bb(NULL),
  _downloadID(-1)
  {}
  
  ~Petition(){ 
    delete _sector;
    releaseData();
  }
  
  void releaseData(){ 
    if (_bb != NULL) delete _bb;
    _bb = NULL;
  }
  
  long getDownloadID() const {
    return _downloadID;
  }
  
  bool isArrived() const{ 
    return _bb != NULL;
  }
  
  void setDownloadID(long id){
    _downloadID = id;
  }

  std::string getURL() const {
    return _url;
  }
  
  Sector getSector() const {
    return *_sector;
  }
  
  void setByteBuffer(ByteBuffer* bb) {
    if (_bb != NULL) delete _bb;
    _bb = bb;
  }

  const ByteBuffer* getByteBuffer() const {
    return _bb;
  }
};


class TilePetitions: public IDownloadListener {
  
  const int    _level;
  const int    _row;
  const int    _column;
  const Sector _tileSector;

  std::vector<Petition*> _petitions;
  
  int _texID;      //TEXTURE ID ONCE IS FINISHED
  
  int _downloadsCounter;
  int _errorsCounter;
  
  TilePetitions(const TilePetitions& that);
  
  Rectangle* getImageRectangleInTexture(const Sector& wholeSector, 
                                       const Sector& imageSector,
                                       int texWidth, int texHeight) const;
  
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
  }
  
  ~TilePetitions()
  {
    for (int i = 0; i < _petitions.size(); i++) {
      delete _petitions[i];
    }
  }
  
  void requestToNet(Downloader& downloader, int priority);
  void requestToCache(Downloader& downloader);
  void cancelPetitions(Downloader& downloader);
  
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
  
  void createTexture(TexturesHandler* texHandler, const IFactory* factory, int width, int height);
  
  int getTexID() const{ return _texID;}

  std::string getPetitionsID() const;

  Petition* getPetition(int i) { return _petitions[i];}
  int getNumPetitions() { return _petitions.size();}
  
  bool allFinished() const;
  
  void onDownload(const Response &response); 
  void onError(const Response& e);
  
  void onCancel(const std::string& url);
  
};

#endif

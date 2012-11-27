//
//  CachedDownloader.h
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 15/08/12.
//
//

#ifndef __G3MiOSSDK__CachedDownloader__
#define __G3MiOSSDK__CachedDownloader__


#include "IDownloader.hpp"
#include "IStorage.hpp"
#include "URL.hpp"

class CachedDownloader : public IDownloader {
private:
  IDownloader* _downloader;
  IStorage*    _storage;

  long _requestsCounter;
  long _cacheHitsCounter;
  long _savesCounter;

  const bool _saveInBackground;

public:
  CachedDownloader(IDownloader* downloader,
                   IStorage*    storage,
                   bool         saveInBackground) :
  _downloader(downloader),
  _storage(storage),
  _requestsCounter(0),
  _cacheHitsCounter(0),
  _savesCounter(0),
  _saveInBackground(saveInBackground)
  {

  }

  bool saveInBackground() const {
    return _saveInBackground;
  }

  void start();

  void stop();

  long long requestBuffer(const URL& url,
                          long long priority,
                          IBufferDownloadListener* listener,
                          bool deleteListener);

  long long requestImage(const URL& url,
                         long long priority,
                         IImageDownloadListener* listener,
                         bool deleteListener);

  void cancelRequest(long long requestId);

  virtual ~CachedDownloader() {
    delete _downloader;
  }

  const std::string statistics();

  void countSave() {
    _savesCounter++;
  }

  void onResume(const G3MContext* context);

  void onPause(const G3MContext* context);

  void onDestroy(const G3MContext* context);

  void initialize(const G3MContext* context);

};

#endif

//
//  CachedDownloader.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 15/08/12.
//
//

#include "CachedDownloader.hpp"
#include "IBufferDownloadListener.hpp"
#include "IImageDownloadListener.hpp"
#include "IStringBuilder.hpp"
#include "ILogger.hpp"
#include "IStorage.hpp"


class BufferSaverDownloadListener : public IBufferDownloadListener {
  CachedDownloader*        _downloader;
  IBufferDownloadListener* _listener;
  const bool               _deleteListener;
  IStorage*                _storage;
  
public:
  BufferSaverDownloadListener(CachedDownloader* downloader,
                              IBufferDownloadListener* listener,
                              bool deleteListener,
                              IStorage* storage) :
  _downloader(downloader),
  _listener(listener),
  _deleteListener(deleteListener),
  _storage(storage)
  {

  }

  void deleteListener() {
    if (_deleteListener) {
#ifdef C_CODE
      delete _listener;
#endif
      _listener = NULL;
    }
  }

  void saveBuffer(const URL& url,
                  const IByteBuffer* buffer) {
    if (buffer != NULL) {
      if (_storage->isAvailable()) {
        //if (!_cacheStorage->containsBuffer(url)) {
        _downloader->countSave();

        _storage->saveBuffer(url, buffer, _downloader->saveInBackground());
        //}
      }
      else {
        ILogger::instance()->logWarning("The cacheStorage is not available, skipping buffer save.");
      }
    }
  }

  void onDownload(const URL& url,
                  const IByteBuffer* data) {
    saveBuffer(url, data);

    _listener->onDownload(url, data);

    deleteListener();
  }

  void onError(const URL& url) {
    _listener->onError(url);

    deleteListener();
  }

  void onCanceledDownload(const URL& url,
                          const IByteBuffer* buffer) {
    saveBuffer(url, buffer);

    _listener->onCanceledDownload(url, buffer);

    // no deleteListener() call, onCanceledDownload() is always called before onCancel().
  }

  void onCancel(const URL& url) {
    _listener->onCancel(url);

    deleteListener();
  }

};

class ImageSaverDownloadListener : public IImageDownloadListener {
  CachedDownloader*       _downloader;
  IImageDownloadListener* _listener;
  const bool              _deleteListener;
  IStorage*               _storage;

public:
  ImageSaverDownloadListener(CachedDownloader* downloader,
                             IImageDownloadListener* listener,
                             bool deleteListener,
                             IStorage* storage) :
  _downloader(downloader),
  _listener(listener),
  _deleteListener(deleteListener),
  _storage(storage)
  {

  }

  void deleteListener() {
    if (_deleteListener) {
#ifdef C_CODE
      delete _listener;
#endif
      _listener = NULL;
    }
  }

  void saveImage(const URL& url,
                 const IImage* image) {
    if (image != NULL) {
      if (_storage->isAvailable()) {
        //if (!_cacheStorage->containsImage(url)) {
        _downloader->countSave();

        _storage->saveImage(url, image, _downloader->saveInBackground());
        //}
      }
      else {
        ILogger::instance()->logWarning("The cacheStorage is not available, skipping image save.");
      }
    }
  }

  void onDownload(const URL& url,
                  const IImage* image) {
    saveImage(url, image);

    _listener->onDownload(url, image);

    deleteListener();
  }

  void onError(const URL& url) {
    _listener->onError(url);

    deleteListener();
  }

  void onCanceledDownload(const URL& url,
                          const IImage* image) {
    saveImage(url, image);

    _listener->onCanceledDownload(url, image);

    // no deleteListener() call, onCanceledDownload() is always called before onCancel().
  }

  void onCancel(const URL& url) {
    _listener->onCancel(url);

    deleteListener();
  }

};

void CachedDownloader::start() {
  _downloader->start();
}

void CachedDownloader::stop() {
  _downloader->stop();
}

void CachedDownloader::cancelRequest(long long requestId) {
  _downloader->cancelRequest(requestId);
}

long long CachedDownloader::requestImage(const URL& url,
                                         long long priority,
                                         IImageDownloadListener* listener,
                                         bool deleteListener) {
  _requestsCounter++;

  const IImage* cachedImage = _storage->isAvailable() ? _storage->readImage(url) : NULL;
  if (cachedImage == NULL) {
    // cache miss
    return _downloader->requestImage(url,
                                     priority,
                                     new ImageSaverDownloadListener(this,
                                                                    listener,
                                                                    deleteListener,
                                                                    _storage),
                                     true);
  }

  // cache hit
  _cacheHitsCounter++;

  listener->onDownload(url, cachedImage);

  if (deleteListener) {
#ifdef C_CODE
    delete listener;
#else
    listener = NULL;
#endif
  }

  delete cachedImage;
  return -1;
}

long long CachedDownloader::requestBuffer(const URL& url,
                                          long long priority,
                                          IBufferDownloadListener* listener,
                                          bool deleteListener) {
  _requestsCounter++;

  const IByteBuffer* cachedBuffer = _storage->isAvailable() ? _storage->readBuffer(url) : NULL;
  if (cachedBuffer == NULL) {
    // cache miss
    return _downloader->requestBuffer(url,
                                      priority,
                                      new BufferSaverDownloadListener(this,
                                                                      listener,
                                                                      deleteListener,
                                                                      _storage),
                                      true);
  }

  // cache hit
  _cacheHitsCounter++;

  listener->onDownload(url, cachedBuffer);

  if (deleteListener) {
#ifdef C_CODE
    delete listener;
#else
    listener = NULL;
#endif
  }

  delete cachedBuffer;
  return -1;
}

const std::string CachedDownloader::statistics() {
  IStringBuilder *isb = IStringBuilder::newStringBuilder();
  isb->addString("CachedDownloader(cache hits=");
  isb->addInt(_cacheHitsCounter);
  isb->addString("/");
  isb->addInt(_requestsCounter);
  isb->addString(", saves=");
  isb->addInt(_savesCounter);
  isb->addString(", downloader=");
  //isb->addString(IDownloader::instance()->statistics());
  isb->addString(_downloader->statistics());
  const std::string s = isb->getString();
  delete isb;
  return s;
}

void CachedDownloader::onResume(const Context* context) {
  _downloader->onResume(context);
}

void CachedDownloader::onPause(const Context* context) {
  _downloader->onPause(context);
}

void CachedDownloader::onDestroy(const Context* context) {
  _downloader->onDestroy(context);
}

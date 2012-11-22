//
//  Downloader_iOS.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 27/07/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#ifndef G3MiOSSDK_Downloader_iOS_hpp
#define G3MiOSSDK_Downloader_iOS_hpp

#import "Downloader_iOS_WorkerThread.h"
#import "Downloader_iOS_Handler.h"

#include "IDownloader.hpp"


class Downloader_iOS : public IDownloader {
private:
  NSMutableArray*      _workers;
  
  NSLock*              _lock;                // synchronization helper
  NSMutableDictionary* _downloadingHandlers; // downloads current in progress
  NSMutableDictionary* _queuedHandlers;      // queued downloads
  
  long long _requestIdCounter;
  
  long long _requestsCounter;
  long long _cancelsCounter;
  
  NSString* toNSString(const std::string& cppStr) const {
    return [ NSString stringWithCString: cppStr.c_str()
                               encoding: NSUTF8StringEncoding ];
  }
  
  bool _started;
  
  long long request(const URL &url,
                    long long priority,
                    Downloader_iOS_Listener* iosListener);
  
  
public:
  
  void removeDownloadingHandlerForNSURL(const NSURL* url);
  
  Downloader_iOS(int maxConcurrentOperationCount);
  
  long long requestBuffer(const URL& url,
                          long long priority,
                          IBufferDownloadListener* listener,
                          bool deleteListener);
  
  long long requestImage(const URL& url,
                         long long priority,
                         IImageDownloadListener* listener,
                         bool deleteListener);
  
  void cancelRequest(long long requestId);
  
  Downloader_iOS_Handler* getHandlerToRun();
  
  virtual ~Downloader_iOS();
  
  void start();
  
  void stop();
  
  const std::string statistics();
  
  void onResume(const Context* context) {
    
  }
  
  void onPause(const Context* context) {
    
  }
  
  void onDestroy(const Context* context) {

  }

};


#endif

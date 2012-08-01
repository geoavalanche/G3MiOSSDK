//
//  Downloader_iOS.mm
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 27/07/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#include "Downloader_iOS.hpp"

#import "Downloader_iOS_Handler.h"


Downloader_iOS::~Downloader_iOS() {
  [_worker stop];
  
}

Downloader_iOS::Downloader_iOS(int memoryCapacity,
                               int diskCapacity,
                               std::string diskPath,
                               int maxConcurrentOperationCount) :
_requestIdCounter(0)
{
  _downloadingHandlers = [NSMutableDictionary dictionary];
  _queuedHandlers      = [NSMutableDictionary dictionary];
  
  NSURLCache *sharedCache = [[NSURLCache alloc] initWithMemoryCapacity: memoryCapacity
                                                          diskCapacity: diskCapacity
                                                              diskPath: toNSString(diskPath)];
  [NSURLCache setSharedURLCache:sharedCache];
  
  _lock = [[NSLock alloc] init];
  
  _worker = [Downloader_iOS_WorkerThread workerForDownloader: this];
  [_worker start];
}

void Downloader_iOS::cancelRequest(long requestId) {
  [_lock lock];

  [ _queuedHandlers enumerateKeysAndObjectsUsingBlock:^(id key,
                                                        id obj,
                                                        BOOL *stop) {
    NSURL*                  url     = key;
    Downloader_iOS_Handler* handler = obj;

    if ( [handler removeRequest: requestId] ) {
      if (![handler hasListeners]) {
        [_queuedHandlers removeObjectForKey:url];
      }
      
      *stop = YES;
    }
  } ];

  
  [_lock unlock];
}

Downloader_iOS_Handler* Downloader_iOS::getHandlerToRun() {
  
  __block Downloader_iOS_Handler* selectedHandler = nil;
  __block long                    selectedPriority = -1000000;
  __block NSURL*                  selectedURL = nil;
  
  [_lock lock];
  
  [ _queuedHandlers enumerateKeysAndObjectsUsingBlock:^(id key,
                                                        id obj,
                                                        BOOL *stop) {
    NSURL*                  url     = key;
    Downloader_iOS_Handler* handler = obj;
    
    const long priority = [handler priority];
    
    if (priority > selectedPriority) {
      selectedPriority = priority;
      selectedHandler  = handler;
      selectedURL      = url;
    }
  } ];
  
  if (selectedHandler) {
    // move the selected handler to _downloadingHandlers collection
    [_queuedHandlers removeObjectForKey: selectedURL];
    [_downloadingHandlers setObject: selectedHandler
                             forKey: selectedURL];
  }
  
  [_lock unlock];
  
  return selectedHandler;
}

long Downloader_iOS::request(const URL &url,
                             long priority,
                             IDownloadListener* cppListener) {
  int __TODO_new_downloader;
  
  NSURL* nsURL = [NSURL URLWithString: toNSString(url.getPath())];
  
  Downloader_iOS_Listener* iosListener = [[Downloader_iOS_Listener alloc] initWithCPPListener: cppListener];
  
  Downloader_iOS_Handler* handler = nil;
  
  [_lock lock];

  const long requestId = _requestIdCounter++;

  handler = [_downloadingHandlers objectForKey: nsURL];
  if (handler) {
    // the URL is being downloaded, just add the new listener.
    [handler addListener: iosListener
                priority: priority
               requestId: requestId];
    return requestId;
  }
  
  handler = [_queuedHandlers objectForKey: nsURL];
  if (handler) {
    // the URL is queued for future download, just add the new listener.
    [handler addListener: iosListener
                priority: priority
               requestId: requestId];
    return requestId;
  }
  
  
  // new handler, queue it
  handler = [[Downloader_iOS_Handler alloc] initWithUrl: nsURL
                                               listener: iosListener
                                               priority: priority
                                              requestId: requestId];
  [_queuedHandlers setObject: handler
                      forKey: nsURL];
  
  [_lock unlock];
  
  return requestId;
}
//
//  IDownloader.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 27/07/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#ifndef G3MiOSSDK_IDownloader_hpp
#define G3MiOSSDK_IDownloader_hpp

#include "URL.hpp"
#include "IDownloadListener.hpp"

class IDownloader {
public:
  
  virtual long request(const URL& url,
                       long priority,
                       IDownloadListener* listener) = 0;
  
  virtual void cancelRequest(long requestId) = 0;
  
  virtual ~IDownloader() {
    
  }
};

#endif
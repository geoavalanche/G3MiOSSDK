//
//  Storage.h
//  G3MiOSSDK
//
//  Created by José Miguel S N on 26/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_Storage_h
#define G3MiOSSDK_Storage_h

#include "URL.hpp"
#include "IByteBuffer.hpp"
#include "IImage.hpp"

class IStorage {
public:
  
  virtual bool contains(const URL& url) = 0;

  virtual bool containsImage(const URL& url) = 0;
  
  virtual void save(const URL& url,
                    const IByteBuffer& buffer) = 0;
  
  virtual void saveImage(const URL& url,
                         const IImage& buffer) = 0;
  
  virtual const IByteBuffer* read(const URL& url) = 0;
  
  virtual const IImage* readImage(const URL& url) = 0;
  
#ifdef C_CODE
  virtual ~IStorage() {}
#endif
};

#endif

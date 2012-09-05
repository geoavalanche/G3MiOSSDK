//
//  IBuffer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 05/09/12.
//
//

#ifndef __G3MiOSSDK__IBuffer__
#define __G3MiOSSDK__IBuffer__

class IBuffer {
public:
  
  virtual ~IBuffer() {
    
  }
  
  /**
   Answer the size (the count of elements) of the buffer
   **/
  virtual int size() const = 0;
  
  /**
   Answer the timestamp of the buffer.
   
   This number will be different each time the buffer changes its contents.
   It provides a fast method to check if the Buffer has changed.
   **/
  virtual long long timestamp() const = 0;
  
};

#endif

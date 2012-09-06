//
//  IntBuffer_iOS.h
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 06/09/12.
//
//

#ifndef __G3MiOSSDK__IntBuffer_iOS__
#define __G3MiOSSDK__IntBuffer_iOS__

#include "IIntBuffer.hpp"

class IntBuffer_iOS : public IIntBuffer {
private:
  const int _size;
  int*    _values;
  
  int _timestamp;

public:
  IntBuffer_iOS(int size) :
  _size(size),
  _timestamp(0)
  {
    _values = new int[size];
  }

  virtual ~IntBuffer_iOS() {
    delete [] _values;
  }
  
  int size() const {
    return _size;
  }
  
  long long timestamp() const {
    return _timestamp;
  }

  
  int get(int i) const {
    return _values[i];
  }
  
  void put(int i, int value) {
    if (_values[i] != value) {
      _values[i] = value;
      _timestamp++;
    }
  }

};

#endif


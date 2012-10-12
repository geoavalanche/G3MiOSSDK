//
//  Factory_iOS.h
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_Factory_iOS_h
#define G3MiOSSDK_Factory_iOS_h

#include "IFactory.hpp"

#include "Timer_iOS.hpp"
#include "Image_iOS.hpp"

#include "ByteBuffer_iOS.hpp"
#include "FloatBuffer_iOS.hpp"
#include "IntBuffer_iOS.hpp"

class Factory_iOS: public IFactory {
public:
  
  ITimer* createTimer() const {
    return new Timer_iOS();
  }
  
  void deleteTimer(const ITimer* timer) const {
    delete timer;
  }
  
  IImage* createImageFromSize(int width, int height) const
  {
    return new Image_iOS(width, height);
  }
  
  IImage* createImageFromFileName(const std::string filename) const {
    NSString *fn = [NSString stringWithCString:filename.c_str()
                                      encoding:[NSString defaultCStringEncoding]];
    
    UIImage* image = [UIImage imageNamed:fn];
    if (!image) {
      printf("Can't read image %s\n", filename.c_str());
      
      return NULL;
    }
    
    return new Image_iOS(image, NULL);
  }
  
  IImage* createImageFromBuffer(const IByteBuffer* buffer) const {
    
    ByteBuffer_iOS* buffer_iOS = (ByteBuffer_iOS*) buffer;
    
    NSData* data = [NSData dataWithBytes: buffer_iOS->getPointer()
                                  length: buffer_iOS->size()];
    
    UIImage* uiImage = [UIImage imageWithData:data];
    if (uiImage) {
      return new Image_iOS(uiImage, data);
    }
    else {
      printf("Can't read image from IByteBuffer %s\n", buffer->description().c_str());
      return NULL;
    }
  }
  
  void deleteImage(const IImage* image) const {
    delete image;
  }
  
  IByteBuffer* createByteBuffer(unsigned char data[], int length) const{
    return new ByteBuffer_iOS(data, length);
  }
  
  IByteBuffer* createByteBuffer(int size) const{
    return new ByteBuffer_iOS(size);
  }
  
  IFloatBuffer* createFloatBuffer(int size) const {
    return new FloatBuffer_iOS(size);
  }
  
  IFloatBuffer* createFloatBuffer(float f0,
                                  float f1,
                                  float f2,
                                  float f3,
                                  float f4,
                                  float f5,
                                  float f6,
                                  float f7,
                                  float f8,
                                  float f9,
                                  float f10,
                                  float f11,
                                  float f12,
                                  float f13,
                                  float f14,
                                  float f15) const {
    return new FloatBuffer_iOS(f0,
                               f1,
                               f2,
                               f3,
                               f4,
                               f5,
                               f6,
                               f7,
                               f8,
                               f9,
                               f10,
                               f11,
                               f12,
                               f13,
                               f14,
                               f15);
  }
  
  
  IIntBuffer* createIntBuffer(int size) const {
    return new IntBuffer_iOS(size);
  }
  
};

#endif

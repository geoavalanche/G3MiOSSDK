//
//  IFactory.h
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_IFactory_h
#define G3MiOSSDK_IFactory_h

#include "ILogger.hpp"

#ifdef C_CODE
#define GFactory (*IFactory::instance())
#else
#define GFactory IFactory.instance() //FOR JAVA CONVERTER
#endif

class ITimer;
class IImage;
class IFloatBuffer;
class IIntBuffer;
class IByteBuffer;
class ILogger;
class IImageListener;

class IFactory {
private:
  static IFactory* _instance;

public:
  static void setInstance(IFactory* factory) {
    if (_instance != NULL) {
      ILogger::instance()->logWarning("IFactory instance already set!");
    }
    _instance = factory;
  }

  static IFactory* instance() {
    return _instance;
  }

  virtual ~IFactory() {

  }

//  virtual IImage* createImageFromFileName(const std::string& filename) const = 0;
//
//  virtual IImage* createImageFromBuffer(const IByteBuffer* buffer) const = 0;
//
//  virtual IImage* createImageFromSize(int width, int height) const = 0;

  virtual void createImageFromFileName(const std::string& filename,
                                       IImageListener* listener,
                                       bool autodelete) const = 0;

  virtual void createImageFromBuffer(const IByteBuffer* buffer,
                                     IImageListener* listener,
                                     bool autodelete) const = 0;

  virtual void createImageFromSize(int width, int height,
                                   IImageListener* listener,
                                   bool autodelete) const = 0;

  virtual void deleteImage(const IImage* image) const = 0;

  virtual ITimer* createTimer() const = 0;

  virtual void deleteTimer(const ITimer* timer) const = 0;

  virtual IFloatBuffer* createFloatBuffer(int size) const = 0;

  /* special factory method for creating floatbuffers from matrix */
  virtual IFloatBuffer* createFloatBuffer(float f0,
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
                                          float f15) const = 0;

  virtual IIntBuffer* createIntBuffer(int size) const = 0;

  virtual IByteBuffer* createByteBuffer(int length) const = 0;

  virtual IByteBuffer* createByteBuffer(unsigned char data[], int length) const = 0;

};

#endif

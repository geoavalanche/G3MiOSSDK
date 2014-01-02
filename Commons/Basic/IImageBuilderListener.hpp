//
//  IImageBuilderListener.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 1/2/14.
//
//

#ifndef __G3MiOSSDK__IImageBuilderListener__
#define __G3MiOSSDK__IImageBuilderListener__

class IImage;

#include <string>

class IImageBuilderListener {
public:
  virtual ~IImageBuilderListener() {
  }

  virtual void imageCreated(const IImage*      image,
                            const std::string& imageName) = 0;

  virtual void onError(const std::string& error) = 0;

};

#endif

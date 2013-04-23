//
//  IImage.h
//  G3MiOSSDK
//
//  Created by José Miguel S N on 01/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_IImage_h
#define G3MiOSSDK_IImage_h

//#include <vector>
//#include "RectangleI.hpp"
#include "Vector2I.hpp"
//class RectangleI;
//class IImageListener;

class IImage {
public:
  virtual ~IImage() {}
  
  virtual int getWidth() const = 0;
  virtual int getHeight() const = 0;
  virtual Vector2I getExtent() const = 0;
  
//  
//  virtual void subImage(const RectangleI& rect,
//                        IImageListener* listener,
//                        bool autodelete) const = 0;
//  
//  virtual void scale(int width, int height,
//                     IImageListener* listener,
//                     bool autodelete) const = 0;
  
  virtual const std::string description() const = 0;
  
  virtual IImage* shallowCopy() const = 0;
  
//  
//  virtual void combineWith(const RectangleI& thisSourceRect,
//                           const IImage& other,
//                           const RectangleI& sourceRect,
//                           const RectangleI& destRect,
//                           const Vector2I& destSize,
//                           IImageListener* listener,
//                           bool autodelete) const = 0;
//  
//  virtual void combineWith(const RectangleI& thisSourceRect,
//                           const std::vector<const IImage*>& images,
//                           const std::vector<RectangleI*>& sourceRects,
//                           const std::vector<RectangleI*>& destRects,
//                           const Vector2I& size,
//                           IImageListener* listener,
//                           bool autodelete) const = 0;
  
};

#endif

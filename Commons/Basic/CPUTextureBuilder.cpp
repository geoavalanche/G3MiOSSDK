//
//  CPUTextureBuilder.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "CPUTextureBuilder.hpp"

const IImage* CPUTextureBuilder::createTextureFromImage(GL* gl,
                                                        const IFactory* factory,
                                                        const IImage* image,
                                                        int width,
                                                        int height) const{
  if (image == NULL) {
    ILogger::instance()->logWarning("Creating blank Image");
    return factory->createImageFromSize(width, height);
  }

  if (image->getHeight() == height && image->getWidth() == width) {
    return image->shallowCopy();
  }

  return image->scale(width, height);
}

const IImage* CPUTextureBuilder::createTextureFromImages(GL* gl,
                                                         const IFactory* factory,
                                                         const std::vector<const IImage*> images,
                                                         int width,
                                                         int height) const{

  const int imagesSize = images.size();

  if (imagesSize == 0) {
    ILogger::instance()->logWarning("Creating blank Image");
    return factory->createImageFromSize(width, height);
  }

  const IImage* im = images[0]->shallowCopy();
  const IImage* im2 = NULL;
  for (int i = 1; i < imagesSize; i++) {
    const IImage* imTrans = images[i];
    im2 = im->combineWith(*imTrans, width, height);
    delete im;
    im = im2;
  }
  return im;
}

const IImage* CPUTextureBuilder::createTextureFromImages(GL* gl,
                                                         const IFactory* factory,
                                                         const std::vector<const IImage*> images,
                                                         const std::vector<const RectangleD*> rectangles,
                                                         int width,
                                                         int height) const{

  const int imagesSize = images.size();

  if (imagesSize == 0 || images.size() != rectangles.size()) {
    ILogger::instance()->logWarning("Creating blank Image");
    return factory->createImageFromSize(width, height);
  }

  const IImage* base;
  int i;
  const RectangleD baseRec(0,0, width, height);
  if (rectangles.size() > 0 && rectangles[0]->equalTo(baseRec)){
    base = images[0]->shallowCopy();
    i = 1;
  }
  else {
    base = factory->createImageFromSize(width, height);
    i = 0;
  }

  for (; i < images.size(); i++) {
    const IImage* newIm = images[i];
    const RectangleD* newRect = rectangles[i];

    IImage* im2 = base->combineWith(*newIm, *newRect, width, height);
    delete base;
    base = im2;
  }
  return base;

}

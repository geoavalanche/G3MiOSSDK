//
//  CPUTextureBuilder.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "CPUTextureBuilder.hpp"

const GLTextureID CPUTextureBuilder::createTextureFromImages(GL * gl,
                                                             const std::vector<const IImage*> images,
                                                             int width, int height) const {
  const int imagesSize = images.size();
  
  if (imagesSize == 0) {
    return GLTextureID::invalid();
  }
  
  const IImage* im = images[0];
  const IImage* im2 = NULL;
  for (int i = 1; i < imagesSize; i++) {
    const IImage* imTrans = images[i];
    im2 = im->combineWith(*imTrans, width, height);
    if (i > 1) {
      delete im;
    }
    im = im2;
  }
  
  const GLTextureID texID = gl->uploadTexture(im, width, height);
  
  if (imagesSize > 1) {
    delete im;
  }
  
  return texID;
}

const GLTextureID CPUTextureBuilder::createTextureFromImages(GL * gl, const IFactory* factory,
                                                             const std::vector<const IImage*> images,
                                                             const std::vector<const Rectangle*> rectangles,
                                                             int width, int height) const {
  const IImage* base;
  int i = 0; //First image to merge
  Rectangle baseRec(0,0, width, height);
  if (rectangles[0]->equalTo(baseRec)) {
    base = images[0];
    i = 1;
  }
  else {
    base = factory->createImageFromSize(width, height);
  }
  
  for (; i < images.size(); i++) {
    IImage* im2 = base->combineWith(*images[i], *rectangles[i], width, height);
    
    if (base != images[0]) {
      delete base;
    }
    
    base = im2;
  }
  
  const GLTextureID texID = gl->uploadTexture(base, width, height);
  
  if (base != images[0]) {
    delete base;
  }
  
  return texID;
}

//
//  StaticImageLayer.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 26/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "StaticImageLayer.hpp"

#include <sstream>

#include "IStorage.hpp"

#include "Tile.hpp"

std::vector<Petition*> StaticImageLayer::getTilePetitions(const Tile& tile, int width, int height) const
{
  std::vector<Petition*> res;
  
  if (!_bbox.fullContains(tile.getSector())) {
    return res;
  }
  
  Sector imageSector = tile.getSector();
  
  //CREATING ID FOR PETITION
  std::ostringstream oss;
  oss << _layerID << "_" << imageSector.lower().latitude().degrees() << "_";
  oss << imageSector.lower().longitude().degrees() << "_" << imageSector.upper().latitude().degrees();
  oss << "_" << imageSector.upper().longitude().degrees();
  std::string id = oss.str();
  
  Petition *pet = new Petition(tile.getSector(), id);

  if (_storage != NULL)
  {
    if (_storage->contains(id)){
      ByteBuffer* bb = _storage->read(id);
      pet->setByteBuffer(bb);        //FILLING DATA
      res.push_back(pet);
      return res;
    }
  }
  
  double widthUV = imageSector.getDeltaLongitude().degrees() / _bbox.getDeltaLongitude().degrees();
  double heightUV = imageSector.getDeltaLatitude().degrees() / _bbox.getDeltaLatitude().degrees();
  
  Vector2D p = _bbox.getUVCoordinates(imageSector.lower().latitude(), imageSector.lower().longitude());
  Vector2D pos(p.x(), p.y() - heightUV);
  
  Rectangle r(pos.x() * _image->getWidth(), pos.y() * _image->getHeight(), widthUV * _image->getWidth(), heightUV * _image->getHeight());
  
  IImage* subImage = _image->subImage(r);
  
  ByteBuffer* bb = subImage->getEncodedImage(); //Image Encoding PNG
  pet->setByteBuffer(bb);        //FILLING DATA
  delete subImage;

  res.push_back(pet);
  
  if (_storage != NULL){
    _storage->save(id, *bb);
  }
  
  return res;
}
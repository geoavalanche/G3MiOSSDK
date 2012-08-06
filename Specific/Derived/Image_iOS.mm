//
//  Image_iOS.mm
//  G3MiOSSDK
//
//  Created by José Miguel S N on 13/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Image_iOS.hpp"

Image_iOS::Image_iOS(int width, int height)
{
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  void *imageData = new unsigned char[height * width * 4 ];
  
  CGContextRef context = CGBitmapContextCreate(imageData,
                                               width, height,
                                               8, 4 * width,
                                               colorSpace,
                                               kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
  CGColorSpaceRelease( colorSpace );
  CGContextClearRect( context, CGRectMake( 0, 0, width, height ) );
  
  CGImageRef imgRef = CGBitmapContextCreateImage(context);
  _image = [UIImage imageWithCGImage:imgRef];
  CGImageRelease(imgRef);
  CGContextRelease(context);
}

IImage* Image_iOS::combineWith(const IImage& transparent, int width, int height) const
{
  UIImage* transIm = ((Image_iOS&)transparent).getUIImage();
  
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  void *imageData = new unsigned char[height * width * 4];
  
  CGContextRef context = CGBitmapContextCreate(imageData,
                                               width, height,
                                               8, 4 * width,
                                               colorSpace,
                                               kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
  CGColorSpaceRelease( colorSpace );
  CGRect bounds = CGRectMake( 0, 0, width, height );
  CGContextClearRect( context, bounds );
  
  //We draw the images one over the other
  CGContextDrawImage( context, bounds, _image.CGImage );
  CGContextDrawImage( context, bounds, transIm.CGImage );
  
  CGImageRef imgRef = CGBitmapContextCreateImage(context);
  UIImage* img = [UIImage imageWithCGImage:imgRef];
  CGImageRelease(imgRef);
  CGContextRelease(context);
  
  return new Image_iOS(img);
}

IImage* Image_iOS::combineWith(const IImage& other, const Rectangle& rect, int width, int height) const
{
  UIImage* otherIm = ((Image_iOS&)other).getUIImage();
  
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  void *imageData = new unsigned char[height * width * 4 ];
  
  CGContextRef context = CGBitmapContextCreate(imageData,
                                               width, height,
                                               8, 4 * width,
                                               colorSpace,
                                               kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
  CGColorSpaceRelease( colorSpace );
  CGContextClearRect( context, CGRectMake( 0, 0, width, height ) );
  
  //We draw the images one over the other
  CGContextDrawImage( context, CGRectMake( 0, 0, width, height ), _image.CGImage );
  CGContextDrawImage( context, CGRectMake((float) rect._x,
                                          (float) rect._y,
                                          (float) rect._width,
                                          (float) rect._height ),
                     otherIm.CGImage );
  
  //SAVING IMAGE
  CGImageRef imgRef = CGBitmapContextCreateImage(context);
  UIImage* img = [UIImage imageWithCGImage:imgRef];
  CGImageRelease(imgRef);
  CGContextRelease(context);
  
  return new Image_iOS(img);
}

IImage* Image_iOS::subImage(const Rectangle& rect) const
{
  CGRect cropRect = CGRectMake((float) rect._x,
                               (float) rect._y,
                               (float) rect._width,
                               (float) rect._height);
  
  //Cropping image
  CGImageRef imageRef = CGImageCreateWithImageInRect([this->_image CGImage], cropRect);
  
  Image_iOS* image = new Image_iOS([UIImage imageWithCGImage:imageRef]); //Create IImage
  
  CGImageRelease(imageRef);
  return image;
}

ByteBuffer* Image_iOS::getEncodedImage() const
{
  NSData* readData = UIImagePNGRepresentation(_image);
  NSUInteger length = [readData length];
  
  unsigned char* data = new unsigned char[length];
  [readData getBytes:data length:length ];
  
  return new ByteBuffer(data, length);
}

void Image_iOS::fillWithRGBA(unsigned char imageData[], int width, int height) const
{
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
  CGContextRef context = CGBitmapContextCreate(imageData,
                                               width, height,
                                               8, 4 * width,
                                               colorSpace,
                                               kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
  
  CGColorSpaceRelease( colorSpace );
  CGRect bounds = CGRectMake( 0, 0, width, height );
  CGContextClearRect( context, bounds );
  CGContextDrawImage( context, bounds, _image.CGImage );
  
  CGContextRelease(context);
}

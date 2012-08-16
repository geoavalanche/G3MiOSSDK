//
//  FileSystemStorage_iOS.mm
//  G3MiOSSDK
//
//  Created by José Miguel S N on 26/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "FileSystemStorage_iOS.hpp"

FileSystemStorage_iOS::FileSystemStorage_iOS(const URL& root)
{
  _root = [[NSString alloc] initWithCString: root.getPath().c_str()
                                   encoding: NSUTF8StringEncoding];
}

bool FileSystemStorage_iOS::contains(const URL& url)
{
  NSString *file = generateFileName(url);
  
  return [[NSFileManager defaultManager] fileExistsAtPath:file];
}

void FileSystemStorage_iOS::save(const URL& url,
                                 const ByteBuffer& buffer) {
  
  NSString *fullPath = generateFileName(url);
  NSFileManager *fileManager = [NSFileManager defaultManager];
  NSData *writeData = [[NSData alloc] initWithBytes: buffer.getData()
                                             length: buffer.getLength()];
  
  if (![fileManager createFileAtPath: fullPath
                            contents: writeData
                          attributes: nil]) {
    //ASSUMING DIRECTORY MISSING
    NSString* dir = [fullPath stringByDeletingLastPathComponent];
    [fileManager createDirectoryAtPath: dir
           withIntermediateDirectories: YES
                            attributes: nil
                                 error: nil];
    if (![fileManager createFileAtPath: fullPath
                              contents: writeData
                            attributes: nil]) {
      NSLog(@"ERROR %d (%s) saving file %@",
            errno,
            strerror(errno),
            [fullPath description]);
    }
  }
}

const ByteBuffer* FileSystemStorage_iOS::read(const URL& url)
{
  NSString *file = generateFileName(url);
  NSData *readData = [[NSData alloc] initWithContentsOfFile:file];
  
  if (readData == nil) {
    return NULL;
  }
  
  NSUInteger length = [readData length];
  unsigned char* data = new unsigned char[length];
  [readData getBytes: data
              length: length];
  
  return new ByteBuffer(data, length);
}

NSString* FileSystemStorage_iOS::generateFileName(const URL& url) {
  NSString* fileName = [[NSString alloc] initWithCString: url.getPath().c_str()
                                                encoding: NSUTF8StringEncoding];
  
  fileName = [fileName stringByReplacingOccurrencesOfString: @"/"
                                                 withString: @"_"];
  
  //NSLog(@"%@", fileName);
  
  return [_root stringByAppendingPathComponent:fileName];
}

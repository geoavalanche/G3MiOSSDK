//
//  SQLiteStorage_iOS.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 16/08/12.
//
//

#ifndef __G3MiOSSDK__SQLiteStorage_iOS__
#define __G3MiOSSDK__SQLiteStorage_iOS__

#include "IStorage.hpp"

#import "SQDatabase.h"

#include <string>

class SQLiteStorage_iOS : public IStorage {
private:
  const std::string _databaseName;
  
  SQDatabase* _readDB;
  SQDatabase* _writeDB;
  
  NSLock* _lock;
  
  NSString* toNSString(const std::string& cppStr) const {
    return [ NSString stringWithCString: cppStr.c_str()
                               encoding: NSUTF8StringEncoding ];
  }
  
  NSString* getDBPath() const;
  
  void showStatistics() const;
  
public:
  void rawSave(NSString* table,
               NSString* name,
               NSData* contents,
               const TimeInterval& timeToExpires);
  
  SQLiteStorage_iOS(const std::string &databaseName);
  
  virtual ~SQLiteStorage_iOS() {
  }
  
  void saveBuffer(const URL& url,
                  const IByteBuffer* buffer,
                  const TimeInterval& timeToExpires,
                  bool saveInBackground);

  IByteBuffer* readBuffer(const URL& url);

  void saveImage(const URL& url,
                 const IImage* buffer,
                 const TimeInterval& timeToExpires,
                 bool saveInBackground);
  
  IImage* readImage(const URL& url);
  
  void onResume(const G3MContext* context) {
    
  }
  
  void onPause(const G3MContext* context) {
    
  }

  void onDestroy(const G3MContext* context) {

  }
  
  bool isAvailable() {
    return (_readDB != NULL) && (_writeDB != NULL);
  }
  
};

#endif

//
//  IJSONParser.hpp
//  G3MiOSSDK
//
//  Created by Oliver Koehler on 02/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_IJSONParser_hpp
#define G3MiOSSDK_IJSONParser_hpp

#include "JSONBaseObject.hpp"
#include "IByteBuffer.hpp"
#include "ILogger.hpp"

class IJSONParser{
private:
  static IJSONParser* _instance;

public:

  static void setInstance(IJSONParser* parser){
    if (_instance != NULL){
      ILogger::instance()->logWarning("IJSONParser instance already set!");
      delete _instance;
    }
    _instance = parser;
  }

  static IJSONParser* instance(){
    return _instance;
  }

  virtual ~IJSONParser(){}

  virtual JSONBaseObject* parse(const std::string& json) = 0;

  virtual JSONBaseObject* parse(IByteBuffer* buffer) = 0;

  virtual void deleteJSONData(JSONBaseObject* object) {
    delete object;
  }
  
};



#endif

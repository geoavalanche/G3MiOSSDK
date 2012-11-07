//
//  JSONString.hpp
//  G3MiOSSDK
//
//  Created by Oliver Koehler on 03/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_JSONString_hpp
#define G3MiOSSDK_JSONString_hpp

#include "JSONBaseObject.hpp"

class JSONString : public JSONBaseObject{
private:
  const std::string _value;
  
public:
  ~JSONString() { }

  JSONString(const std::string& value) :
  _value(value)
  {
  }

  const std::string value() const {
    return _value;
  }

  JSONString* asString() {
    return this;
  }
  
};

#endif

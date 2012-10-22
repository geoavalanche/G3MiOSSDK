//
//  URL.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 27/07/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#ifndef G3MiOSSDK_URL_hpp
#define G3MiOSSDK_URL_hpp

#include <string>

class URL {
private:
  const std::string _path;
  URL& operator=(const URL& that);
  
public:
  
  URL(const URL& that) :
  _path(that._path) {
    
  }
  
  URL():_path(""){}
  
  explicit URL(const std::string& path, const bool escape):
  _path(escapeURL(path, escape))
  {
  };
  
  URL(const URL& parent,
      const std::string& path):
  _path(parent.getPath() + "/" + path)
  {
  };
  
  std::string getPath() const {
    return _path;
  }
  
  static URL nullURL() {
    return URL("__NULL__", false);
  }
  
  bool isNull() const {
    return (_path == "__NULL__");
  }
  
  bool isEqualsTo(const URL& that) const {
    return (_path == that._path);
  }
  
  const std::string description() const;
    
    const std::string escapeURL(const std::string& path, const bool escape) const;
    
#ifdef C_CODE
    bool operator<(const URL& that) const {
        if (_path < that._path) {
            return true;
        }
        return false;
    }
#endif    
    
#ifdef JAVA_CODE
    @Override
	public int hashCode() {
		return _path.hashCode();
	}
    
	@Override
	public boolean equals(Object obj) {
		if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final URL other = (URL) obj;
        if (_path.equals(other._path)) {
            return true;
        }
        return false;
	}
#endif
  
};


#endif

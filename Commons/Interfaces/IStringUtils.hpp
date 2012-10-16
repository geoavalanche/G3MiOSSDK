//
//  IStringUtils.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 23/08/12.
//
//

#ifndef __G3MiOSSDK__IStringUtils__
#define __G3MiOSSDK__IStringUtils__

#include <vector>
#include <string>

#include "ILogger.hpp"

class IStringUtils {
private:
#ifdef C_CODE
  static const IStringUtils* _instance;
#else
  static IStringUtils* _instance;
#endif
  
public:
  static void setInstance(const IStringUtils* instance) {
    if (_instance != NULL) {
      ILogger::instance()->logWarning("Warning, IStringUtils instance set two times\n");
    }
    _instance = instance;
  }
  
  static const IStringUtils* instance() {
    return _instance;
  }
  
  virtual ~IStringUtils() {
    
  }
  
  virtual std::string createString(unsigned char data[],
                                   int           length) const = 0;
  
  virtual std::vector<std::string> splitLines(const std::string& string) const = 0;
  
  virtual bool beginsWith(const std::string& string,
                          const std::string& prefix) const = 0;
  
  virtual int indexOf(const std::string& string,
                      const std::string& search) const = 0;
  
  /*
   Returns a new string that is a substring of this string. The substring begins at the
   specified beginIndex and extends to the character at index endIndex - 1. Thus the length
   of the substring is endIndex-beginIndex.
   */
  virtual std::string substring(const std::string& string,
                                int beginIndex,
                                int endIndex) const = 0;
  
  virtual std::string substring(const std::string& string,
                                int beginIndex) const {
//    return substring(string, beginIndex, string.size() + 1);
      return substring(string, beginIndex, string.size());
  }
  
  virtual std::string replaceSubstring(const std::string& originalString, const std::string& toReplace, const std::string& replaceWith) const {
    int startIndex = indexOf(originalString, toReplace);
    //The part to replace was not found. Return original String
    if (startIndex==-1) {
      return originalString;
    }
    int endIndex = startIndex+toReplace.size();
    std::string left = substring(originalString, 0, startIndex);
    std::string right = substring(originalString, endIndex);
    std::string result = left+replaceWith+right;
    startIndex = indexOf(result, toReplace);
    if (startIndex != -1){
      //recursive call to replace other ocurrences
      return replaceSubstring(result, toReplace, replaceWith);
    }
    else{
      return result;
    } 
  }
  
  virtual std::string left(const std::string& string,
                           int endIndex) const {
    return substring(string, 0, endIndex);
  }
  
  virtual std::string rtrim(const std::string& string) const = 0;
  
  virtual std::string ltrim(const std::string& string) const = 0;
  
  virtual std::string trim(const std::string& string) const {
    return rtrim(ltrim(string));
  }
  
};

#endif

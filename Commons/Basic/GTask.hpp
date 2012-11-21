//
//  GTask.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 10/16/12.
//
//

#ifndef __G3MiOSSDK__GTask__
#define __G3MiOSSDK__GTask__

class InitializationContext;

class GTask {
public:
  virtual ~GTask() {
    
  }
  
  virtual void run(const InitializationContext* ic) = 0;
  
};


#endif

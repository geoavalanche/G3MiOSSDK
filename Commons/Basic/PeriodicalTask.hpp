//
//  PeriodicalTask.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 23/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_PeriodicalTask_hpp
#define G3MiOSSDK_PeriodicalTask_hpp

#include "GTask.hpp"
#include "IFactory.hpp"
#include "TimeInterval.hpp"
#include "ITimer.hpp"

class PeriodicalTask {
private:
  long long _intervalMS;
  long long _lastExecutionMS;
  GTask*    _task;
  
  ITimer*   _timer;
  
  ITimer* getTimer() {
    if (_timer == NULL) {
      _timer = IFactory::instance()->createTimer();
    }
    return _timer;
  }
  
public:
  PeriodicalTask(const TimeInterval& interval,
                 GTask* task):
  _intervalMS(interval.milliseconds()),
  _task(task),
  _lastExecutionMS(0),
  _timer(NULL)
  {
  }
  
  ~PeriodicalTask() {
    if (_task != NULL) {
      delete _task;
    }
    
    if (_timer != NULL) {
      IFactory::instance()->deleteTimer(_timer);
    }
  }
  
  void executeIfNecessary() {
    long long now = getTimer()->now().milliseconds();
    
    long long interval = now - _lastExecutionMS;
    
    if (interval >= _intervalMS) {
      _task->run();
      _lastExecutionMS = now;
    }
  }
  
};

#endif
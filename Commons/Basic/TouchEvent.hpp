//
//  TouchEvent.h
//  G3MiOSSDK
//
//  Created by José Miguel S N on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_TouchEvent_h
#define G3MiOSSDK_TouchEvent_h

#include <vector>

#include "Vector2D.hpp"


class Touch {
private:
  const Vector2D _pos;
  const Vector2D _prevPos;

public:
  Touch(const Touch& other) : _pos(other._pos), _prevPos(other._prevPos) {
    
  }
  
  Touch(const Vector2D& pos, const Vector2D& prev) : _pos(pos), _prevPos(prev) { }
  
  const Vector2D getPos() const { return _pos; }
  const Vector2D getPrevPos() const { return _prevPos; }
  ~Touch(){}
};


enum TouchEventType {
  Down,
  Up,
  Move,
  LongPress,
  Scroll
};


class TouchEvent {
private:
  const TouchEventType             _eventType;
  const std::vector<const Touch*>  _touchs;
  const bool                       _shiftPressed;
  const bool                       _ctrlPressed;
  const double                     _wheelDelta;
  
  
  TouchEvent(const TouchEventType& type,
             const std::vector<const Touch*> touchs,
             bool shift,
             bool ctrl,
             double wheelDelta) :
  _eventType(type),
  _touchs(touchs),
  _shiftPressed(shift),
  _ctrlPressed(ctrl),
  _wheelDelta(wheelDelta)
  {
    if (_touchs.size()==2)
      printf ("creando touchevents. touch0=%.0f,%.0f-->%.0f,%.0f.   touch1=%.0f,%.0f-->%.0f,%.0f\n",
              _touchs[0]->getPrevPos().x(), touchs[0]->getPrevPos().y(),
              _touchs[0]->getPos().x(), touchs[0]->getPos().y(),
              _touchs[1]->getPrevPos().x(), touchs[1]->getPrevPos().y(),
              _touchs[1]->getPos().x(), touchs[1]->getPos().y());
  }
  
public:
  TouchEvent(const TouchEvent& other):
  _eventType(other._eventType),
  _touchs(other._touchs),
  _shiftPressed(other._shiftPressed),
  _ctrlPressed(other._ctrlPressed),
  _wheelDelta(other._wheelDelta) {
  }
  
  static TouchEvent* create(const TouchEventType& type,
                           const std::vector<const Touch*> touchs) {
    return new TouchEvent(type, touchs, false, false, 0.0);
  }
  
  static TouchEvent* create(const TouchEventType& type,
                           const std::vector<const Touch*> touchs,
                           bool shift,
                           bool ctrl,
                           double wheelDelta) {
    return new TouchEvent(type, touchs, shift, ctrl, wheelDelta);
  }
  
  static TouchEvent* create(const TouchEventType& type,
                           const Touch* touch) {
    //const Touch * pa[] = { touch };
    //const std::vector<const Touch*> touchs = std::vector<const Touch*>(pa, &pa[1);
    const std::vector<const Touch*> touchs(1, touch);
    
    return create(type, touchs);
  }
  
  static TouchEvent* create(const TouchEventType& type,
                           const Touch* touch,
                           bool shift,
                           bool ctrl,
                           double wheelDelta) {
    const std::vector<const Touch*> touchs(1, touch);
    
    return create(type, touchs, shift, ctrl, wheelDelta);
  }
  
  TouchEventType getType() const {
    return _eventType;
  }
  
  const Touch* getTouch(int i) const {
    return _touchs[i];
  }
  
  int getTouchCount() const {
    return _touchs.size();
  }
  
  ~TouchEvent() {
    for (int i = 0; i < _touchs.size(); i++) {
      delete _touchs[i];
    }
  }
  
};

#endif

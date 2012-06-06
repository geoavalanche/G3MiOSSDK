//
//  TouchEvent.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_TouchEvent_h
#define G3MiOSSDK_TouchEvent_h

#include <vector>

#include "Vector2D.hpp"

#include "TouchEvent.hpp"
//
//
//class Touch {
//private:
//  const Vector2D _pos;
//  const Vector2D _prevPos;
//  
//  Touch& operator=(const Touch& v);
//  
//public:
//  Touch(const Touch& other) : _pos(other._pos), _prevPos(other._prevPos) {
//    
//  }
//  
//  Touch(const Vector2D& pos, const Vector2D& prev) : _pos(pos), _prevPos(prev) { }
//  
//  const Vector2D getPos() const { return _pos; }
//  const Vector2D getPrevPos() const { return _prevPos; }
//};
//
//
//enum TouchEventType {
//  Down,
//  Up,
//  Move,
//  LongPress
//};
//
//
//class TouchEvent {
//private:
//  const TouchEventType       _eventType;
//  const std::vector<Touch> _touchs;
//  
//  TouchEvent(const TouchEventType& type,
//             const std::vector<Touch> Touchs): _eventType(type), _touchs(Touchs) { }
//  
//public:
//  TouchEvent(const TouchEvent& other): _eventType(other._eventType), _touchs(other._touchs) {
//    
//  }
//  
//  static TouchEvent create(const TouchEventType& type,
//                           const std::vector<Touch> Touchs) {
//    return TouchEvent(type, Touchs);
//  }
//  
//  static TouchEvent create(const TouchEventType& type,
//                           const Touch& Touch) {
//    const class Touch pa[] = { Touch };
//    const std::vector<class Touch> Touchs = std::vector<class Touch>(pa, pa+1);
//    return TouchEvent::create(type, Touchs);
//  }
//  
//  TouchEventType getType() const {
//    return _eventType;
//  }  
//  
//  Touch getTouch(int i){ return _touchs[i];}
//};
//
//
//class TapEvent : Vector2D{
//public:
//  TapEvent(double x, double y): Vector2D(x,y) {}
//};


#endif
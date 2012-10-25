//
//  Effects.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 12/06/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#include "Effects.hpp"

#include "Context.hpp"

void EffectsScheduler::initialize(const InitializationContext* ic) {
  _factory = ic->getFactory();
  _timer = _factory->createTimer();
}

void EffectsScheduler::cancellAllEffectsFor(EffectTarget* target) {
  std::vector<int> indicesToRemove;
  const TimeInterval now = _timer->now();

  for (int i = 0; i < _effectsRuns.size(); i++) {
    EffectRun* effectRun = _effectsRuns[i];
    
    if (effectRun->_started == true) {
      if (effectRun->_target == target) {
        effectRun->_effect->cancel(now);
        
        indicesToRemove.push_back(i);
      }
    }
  }
  
  // backward iteration, to remove from bottom to top
  for (int i = indicesToRemove.size() - 1; i >= 0; i--) {
    const int indexToRemove = indicesToRemove[i];
    delete _effectsRuns[indexToRemove];
    
#ifdef C_CODE
    _effectsRuns.erase(_effectsRuns.begin() + indexToRemove);
#endif
#ifdef JAVA_CODE
    _effectsRuns.remove(indexToRemove);
#endif
  }

}

void EffectsScheduler::processFinishedEffects(const RenderContext *rc,
                                              const TimeInterval& now) {
  std::vector<int> indicesToRemove;
  for (int i = 0; i < _effectsRuns.size(); i++) {
    EffectRun* effectRun = _effectsRuns[i];
    
    if (effectRun->_started == true) {
      if (effectRun->_effect->isDone(rc, now)) {
        effectRun->_effect->stop(rc, now);
        
        indicesToRemove.push_back(i);
      }
    }
  }
  
  // backward iteration, to remove from bottom to top
  for (int i = indicesToRemove.size() - 1; i >= 0; i--) {
    const int indexToRemove = indicesToRemove[i];
    delete _effectsRuns[indexToRemove];
    
#ifdef C_CODE
    _effectsRuns.erase(_effectsRuns.begin() + indexToRemove);
#endif
#ifdef JAVA_CODE
    _effectsRuns.remove(indexToRemove);
#endif
  }
}

void EffectsScheduler::doOneCyle(const RenderContext *rc) {
  const TimeInterval now = _timer->now();
  
  
  processFinishedEffects(rc, now);
  
  
  for (int i = 0; i < _effectsRuns.size(); i++) {
    EffectRun* effectRun = _effectsRuns[i];
    
    if (effectRun->_started == false) {
      effectRun->_effect->start(rc, now);
      effectRun->_started = true;
    }
    
    effectRun->_effect->doStep(rc, now);
  }
}

void EffectsScheduler::startEffect(Effect* effect,
                                   EffectTarget* target) {
  _effectsRuns.push_back(new EffectRun(effect, target));
}

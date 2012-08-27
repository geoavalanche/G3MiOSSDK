//
//  InitializationContext.h
//  G3MiOSSDK
//
//  Created by José Miguel S N on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_InitializationContext_h
#define G3MiOSSDK_InitializationContext_h


class IFactory;
class Camera;
class Planet;
class TexturesHandler;
class IDownloader;
class ILogger;
class GL;
class EffectsScheduler;
class ITimer;
class IStringUtils;

class Context {
protected:
  const IFactory*     _factory;
  const IStringUtils* _stringUtils;
  const ILogger*      _logger;
  const Planet*       _planet;
  IDownloader*        _downloader;
  EffectsScheduler*   _effectsScheduler;
  
  Context(const IFactory *factory,
          const IStringUtils* stringUtils,
          const ILogger* logger,
          const Planet* planet,
          IDownloader* downloader,
          EffectsScheduler* effectsScheduler) :
  _factory(factory),
  _stringUtils(stringUtils),
  _logger(logger),
  _planet(planet),
  _downloader(downloader),
  _effectsScheduler(effectsScheduler)
  {
  }
  
public:
  
  const IFactory* getFactory() const {
    return _factory;
  }
  
  const IStringUtils* getStringUtils() const {
    return _stringUtils;
  }
  
  const ILogger* getLogger() const {
    return _logger;
  }
  
  const Planet* getPlanet() const {
    return _planet;
  }
  
  IDownloader* getDownloader() const {
    return _downloader;
  }
  
  EffectsScheduler* getEffectsScheduler() const {
    return _effectsScheduler;
  }
};

//************************************************************


class InitializationContext: public Context {
public:
  InitializationContext(IFactory *factory,
                        const IStringUtils* stringUtils,
                        ILogger* logger,
                        const Planet* planet,
                        IDownloader* downloader,
                        EffectsScheduler* effectsScheduler) :
  Context(factory, stringUtils, logger, planet, downloader, effectsScheduler) {
  }
};

//************************************************************

class EventContext: public Context {
public:
  EventContext(IFactory *factory,
               const IStringUtils* stringUtils,
               ILogger* logger,
               const Planet* planet,
               IDownloader* downloader,
               EffectsScheduler* scheduler) :
  Context(factory, stringUtils, logger, planet, downloader, scheduler) {
  }
};

//************************************************************


class FrameTasksExecutor;


class RenderContext: public Context {
private:
  FrameTasksExecutor* _frameTasksExecutor;
  GL*                 _gl;
  const Camera*       _currentCamera;
  Camera*             _nextCamera;
  TexturesHandler*    _texturesHandler;
  ITimer*             _frameStartTimer;
  
public:
  RenderContext(FrameTasksExecutor* frameTasksExecutor,
                IFactory *factory,
                const IStringUtils* stringUtils,
                ILogger* logger,
                const Planet* planet,
                GL *gl,
                const Camera* currentCamera,
                Camera* nextCamera,
                TexturesHandler* texturesHandler,
                IDownloader* downloader,
                EffectsScheduler* scheduler,
                ITimer* frameStartTimer) :
  Context(factory, stringUtils, logger, planet, downloader, scheduler),
  _frameTasksExecutor(frameTasksExecutor),
  _gl(gl),
  _currentCamera(currentCamera),
  _nextCamera(nextCamera),
  _texturesHandler(texturesHandler),
  _frameStartTimer(frameStartTimer) {
    
  }
  
  GL* getGL() const {
    return _gl;
  }
  
  const Camera* getCurrentCamera() const {
    return _currentCamera;
  }
  
  Camera* getNextCamera() const {
    return _nextCamera;
  }
  
  TexturesHandler* getTexturesHandler() const {
    return _texturesHandler;
  }
  
  const ITimer* getFrameStartTimer() const {
    return _frameStartTimer;
  }
  
  FrameTasksExecutor* getFrameTasksExecutor() const {
    return _frameTasksExecutor;
  }
  
  ~RenderContext();
  
};



#endif

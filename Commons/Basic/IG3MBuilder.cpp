//
//  IG3MBuilder.cpp
//  G3MiOSSDK
//
//  Created by Mari Luz Mateo on 20/11/12.
//
//

#include "IG3MBuilder.hpp"
#include "G3MWidget.hpp"
#include "GL.hpp"
#include "IStorage.hpp"
#include "IDownloader.hpp"
#include "IThreadUtils.hpp"
#include "GInitializationTask.hpp"
#include "PeriodicalTask.hpp"
#include "CameraSingleDragHandler.hpp"
#include "CameraDoubleDragHandler.hpp"
#include "CameraRotationHandler.hpp"
#include "CameraDoubleTapHandler.hpp"
#include "TileRendererBuilder.hpp"
#include "BusyMeshRenderer.hpp"
#include "CompositeRenderer.hpp"
#include "SimpleCameraConstrainer.hpp"

IG3MBuilder::IG3MBuilder() :
_gl(NULL),
_storage(NULL),
_downloader(NULL),
_threadUtils(NULL),
_planet(NULL),
_cameraConstraints(NULL),
_cameraRenderer(NULL), 
_backgroundColor(NULL),
_tileRendererBuilder(NULL),
_busyRenderer(NULL),
_renderers(NULL),
_initializationTask(NULL),
_autoDeleteInitializationTask(true),
_periodicalTasks(NULL),
_logFPS(false),
_logDownloaderStatistics(false),
_userData(NULL)
{
}

IG3MBuilder::~IG3MBuilder() {
  delete _gl;
  delete _storage;
  delete _downloader;
  delete _threadUtils;
  delete _planet;
  if (_cameraConstraints) {
    for (int i = 0; i < _cameraConstraints->size(); i++) {
      delete _cameraConstraints->at(i);
    }
    delete _cameraConstraints;
  }
  delete _cameraRenderer;
  if (_renderers) {
    for (int i = 0; i < _renderers->size(); i++) {
      delete _renderers->at(i);
    }
    delete _renderers;
  }
  delete _busyRenderer;
  delete _backgroundColor;
  delete _initializationTask;
  if (_periodicalTasks) {
    for (int i = 0; i < _periodicalTasks->size(); i++) {
      delete _periodicalTasks->at(i);
    }
    delete _periodicalTasks;
  }
  delete _userData;
  delete _tileRendererBuilder;
}

/**
 * Returns the _gl.
 *
 * @return _gl: GL*
 */
GL* IG3MBuilder::getGL() {
  if (!_gl) {
    ILogger::instance()->logError("Logic Error: _gl not initialized");
  }
  
  return _gl;
}

/**
 * Returns the _storage. If it does not exist, it will be default initializated.
 *
 * @return _storage: IStorage*
 */
IStorage* IG3MBuilder::getStorage() {
  if (!_storage) {
    _storage = createDefaultStorage();
  }
  
  return _storage;
}

/**
 * Returns the _downloader. If it does not exist, it will be default initializated.
 *
 * @return _downloader: IDownloader*
 */
IDownloader* IG3MBuilder::getDownloader() {
  if (!_downloader) {
    _downloader = createDefaultDownloader();
  }
  
  return _downloader;
}

/**
 * Returns the _threadUtils. If it does not exist, it will be default initializated.
 *
 * @return _threadUtils: IThreadUtils*
 */
IThreadUtils* IG3MBuilder::getThreadUtils() {
  if (!_threadUtils) {
    _threadUtils = createDefaultThreadUtils();
  }
  
  return _threadUtils;
}

/**
 * Returns the _planet. If it does not exist, it will be default initializated.
 *
 * @return _planet: const Planet*
 */
const Planet* IG3MBuilder::getPlanet() {
  if (!_planet) {
    _planet = Planet::createEarth();
  }
  return _planet;
}

/**
 * Returns the _cameraConstraints list. If it does not exist, it will be default initializated.
 * @see IG3MBuilder#createDefaultCameraConstraints() 
 *
 * @return _cameraConstraints: std::vector<ICameraConstrainer*>
 */
std::vector<ICameraConstrainer*>* IG3MBuilder::getCameraConstraints() {
  if (!_cameraConstraints) {
    _cameraConstraints = createDefaultCameraConstraints();
  }
  
  return _cameraConstraints;
}

/**
 * Returns the _cameraRenderer. If it does not exist, it will be default initializated.
 * @see IG3MBuilder#createDefaultCameraRenderer()
 *
 * @return _cameraRenderer: CameraRenderer*
 */
CameraRenderer* IG3MBuilder::getCameraRenderer() {
  if (!_cameraRenderer) {
    _cameraRenderer = createDefaultCameraRenderer();
  }
  
  return _cameraRenderer;
}

/**
 * Returns the _busyRenderer. If it does not exist, it will be default initializated.
 *
 * @return _busyRenderer: Renderer*
 */
Renderer* IG3MBuilder::getBusyRenderer() {
  if (!_busyRenderer) {
    _busyRenderer = new BusyMeshRenderer(Color::newFromRGBA((float)0, (float)0, (float)0, (float)1));
  }
  
  return _busyRenderer;
}

/**
 * Returns the _backgroundColor. If it does not exist, it will be default initializated.
 *
 * @return _backgroundColor: Color*
 */
Color* IG3MBuilder::getBackgroundColor() {
  if (!_backgroundColor) {
    _backgroundColor = Color::newFromRGBA((float)0, (float)0.1, (float)0.2, (float)1);
  }

  return _backgroundColor;
}

/**
 * Returns the _tileRendererBuilder. If it does not exist, it will be default initializated. 
 *
 * @return _tileRendererBuilder: TileRendererBuilder*
 */
TileRendererBuilder* IG3MBuilder::getTileRendererBuilder() {
  if (!_tileRendererBuilder) {
    _tileRendererBuilder = new TileRendererBuilder();
  }
  
  return _tileRendererBuilder;
}

/**
 * Returns the renderers list. If it does not exist, it will be default initializated.
 * @see IG3MBuilder#createDefaultRenderers()
 *
 * @return _renderers: std::vector<Renderer*>
 */
std::vector<Renderer*>* IG3MBuilder::getRenderers() {
  if (!_renderers) {
    _renderers = createDefaultRenderers();
  }
  return _renderers;
}

/**
 * Returns the value of _logFPS flag.
 *
 * @return _logFPS: bool
 */
bool IG3MBuilder::getLogFPS() {
  return _logFPS;
}

/**
 * Returns the value of _logDownloaderStatistics flag.
 *
 * @return _logDownloaderStatistics: bool
 */
bool IG3MBuilder::getLogDownloaderStatistics() {
  return _logDownloaderStatistics;
}

/**
 * Returns the initialization task.
 *
 * @return _logDownloaderStatistics: GInitializationTask*
 */
GInitializationTask* IG3MBuilder::getInitializationTask() {
  return _initializationTask;
}

/**
 * Returns the value of _autoDeleteInitializationTask flag.
 *
 * @return _autoDeleteInitializationTask: bool
 */
bool IG3MBuilder::getAutoDeleteInitializationTask() {
  return _autoDeleteInitializationTask;
}

/**
 * Returns the array of periodical tasks. If it does not exist, it will be default initializated.
 * @see IG3MBuilder#createDefaultPeriodicalTasks()
 *
 * @return _periodicalTasks: std::vector<PeriodicalTask*>
 */
std::vector<PeriodicalTask*>* IG3MBuilder::getPeriodicalTasks() {
  if (!_periodicalTasks) {
    _periodicalTasks = createDefaultPeriodicalTasks();
  }
  return _periodicalTasks;
}

/**
 * Returns the user data.
 *
 * @return _userData: WidgetUserData*
 */
WidgetUserData* IG3MBuilder::getUserData() {
  return _userData;
}

/**
 * Sets the _gl.
 *
 * @param gl - cannot be NULL.
 */
void IG3MBuilder::setGL(GL *gl) {
  if (_gl) {
    ILogger::instance()->logError("LOGIC ERROR: _gl already initialized");
    return;
  }
  if (!gl) {
    ILogger::instance()->logError("LOGIC ERROR: _gl cannot be NULL");
    return;
  }
  _gl = gl;
}

/**
 * Sets the _storage.
 *
 * @param storage
 */
void IG3MBuilder::setStorage(IStorage *storage) {
  if (_storage) {
    ILogger::instance()->logError("LOGIC ERROR: _storage already initialized");
    return;
  }
  _storage = storage;
}

/**
 * Sets the _downloader
 *
 * @param downloader - cannot be NULL.
 */
void IG3MBuilder::setDownloader(IDownloader *downloader) {
  if (_downloader) {
    ILogger::instance()->logError("LOGIC ERROR: _downloader already initialized");
    return;
  }
  if (!downloader) {
    ILogger::instance()->logError("LOGIC ERROR: _downloader cannot be NULL");
    return;
  }
  _downloader = downloader;
}

/**
 * Sets the _threadUtils
 *
 * @param threadUtils - cannot be NULL.
 */
void IG3MBuilder::setThreadUtils(IThreadUtils *threadUtils) {
  if (_threadUtils) {
    ILogger::instance()->logError("LOGIC ERROR: _threadUtils already initialized");
    return;
  }
  if (!threadUtils) {
    ILogger::instance()->logError("LOGIC ERROR: _threadUtils cannot be NULL");
    return;
  }
  _threadUtils = threadUtils;
}

/**
 * Sets the _planet
 *
 * @param planet - cannot be NULL.
 */
void IG3MBuilder::setPlanet(const Planet *planet) {
  if (_planet) {
    ILogger::instance()->logError("LOGIC ERROR: _planet already initialized");
    return;
  }
  if (!planet) {
    ILogger::instance()->logError("LOGIC ERROR: _planet cannot be NULL");
    return;
  }
  _planet = planet;
}

/**
 * Adds a new camera constraint to the constraints list.
 * The camera constraint list will be initializated with a default constraints set.
 * @see IG3MBuilder#createDefaultCameraConstraints()
 *
 * @param cameraConstraint - cannot be NULL.
 */
void IG3MBuilder::addCameraConstraint(ICameraConstrainer* cameraConstraint) {
  if (!cameraConstraint) {
    ILogger::instance()->logError("LOGIC ERROR: trying to add a NULL cameraConstraint object");
    return;
  }
  getCameraConstraints()->push_back(cameraConstraint);
}

/**
 * Sets the camera constraints list, ignoring the default camera constraints list 
 * and the camera constraints previously added, if added.
 *
 * @param cameraConstraints - std::vector<ICameraConstrainer*>
 */
void IG3MBuilder::setCameraConstrainsts(std::vector<ICameraConstrainer*> cameraConstraints) {
  if (_cameraConstraints) {
    ILogger::instance()->logWarning("LOGIC WARNING: camera contraints previously set will be ignored and deleted");
    for (unsigned int i = 0; i < _cameraConstraints->size(); i++) {
      delete _cameraConstraints->at(i);
    }
    _cameraConstraints->clear();
  }
  else {
    _cameraConstraints = new std::vector<ICameraConstrainer*>;
  }
  for (unsigned int i = 0; i < cameraConstraints.size(); i++) {
    _cameraConstraints->push_back(cameraConstraints[i]);
  }
}

/**
 * Sets the _cameraRenderer
 *
 * @param cameraRenderer - cannot be NULL.
 */
void IG3MBuilder::setCameraRenderer(CameraRenderer *cameraRenderer) {
  if (_cameraRenderer) {
    ILogger::instance()->logError("LOGIC ERROR: _cameraRenderer already initialized");
    return;
  }
  if (!cameraRenderer) {
    ILogger::instance()->logError("LOGIC ERROR: _cameraRenderer cannot be NULL");
    return;
  }
  _cameraRenderer = cameraRenderer;
}

/**
 * Sets the _backgroundColor
 *
 * @param backgroundColor - cannot be NULL.
 */
void IG3MBuilder::setBackgroundColor(Color* backgroundColor) {
  if (_backgroundColor) {
    ILogger::instance()->logError("LOGIC ERROR: _backgroundColor already initialized");
    return;
  }
  if (!backgroundColor) {
    ILogger::instance()->logError("LOGIC ERROR: _backgroundColor cannot be NULL");
    return;
  }
  _backgroundColor = backgroundColor;
}

/**
 * Sets the _busyRenderer
 *
 * @param busyRenderer - cannot be NULL.
 */
void IG3MBuilder::setBusyRenderer(Renderer *busyRenderer) {
  if (_busyRenderer) {
    ILogger::instance()->logError("LOGIC ERROR: _busyRenderer already initialized");
    return;
  }
  if (!busyRenderer) {
    ILogger::instance()->logError("LOGIC ERROR: _busyRenderer cannot be NULL");
    return;
  }
  _busyRenderer = busyRenderer;
}

/**
 * Adds a new renderer to the renderers list.
 * The renderers list will be initializated with a default renderers set (empty set at the moment).
 *
 * @param renderer - cannot be either NULL or an instance of TileRenderer
 */
void IG3MBuilder::addRenderer(Renderer *renderer) {
  if (!renderer) {
    ILogger::instance()->logError("LOGIC ERROR: trying to add a NULL renderer object");
    return;
  }
  if (renderer->isTileRenderer()) {
    ILogger::instance()->logError("LOGIC ERROR: a new TileRenderer is not expected to be added");
    return;
  }
  getRenderers()->push_back(renderer);
}

/**
 * Sets the renderers list, ignoring the default renderers list and the renderers
 * previously added, if added.
 * The renderers list must contain at least an instance of the TileRenderer class.
 *
 * @param renderers - std::vector<Renderer*>
 */
void IG3MBuilder::setRenderers(std::vector<Renderer*> renderers) {
  if (!containsTileRenderer(renderers)) {
    ILogger::instance()->logError("LOGIC ERROR: renderers list must contain at least an instance of the TileRenderer class");
    return;
  }
  if (_renderers) {
    ILogger::instance()->logWarning("LOGIC WARNING: renderers previously set will be ignored and deleted");
    for (unsigned int i = 0; i < _renderers->size(); i++) {
      delete _renderers->at(i);
    }
    _renderers->clear();
  }
  else {
    _renderers = new std::vector<Renderer*>;
  }
  for (unsigned int i = 0; i < renderers.size(); i++) {
    _renderers->push_back(renderers[i]);
  }
}

void IG3MBuilder::pvtSetInitializationTask(GInitializationTask *initializationTask,
                                           const bool autoDeleteInitializationTask) {
  if (_initializationTask) {
    ILogger::instance()->logError("LOGIC ERROR: _initializationTask already initialized");
    return;
  }
  if (!initializationTask) {
    ILogger::instance()->logError("LOGIC ERROR: initializationTask cannot be NULL");
    return;
  }
  _initializationTask = initializationTask;
  _autoDeleteInitializationTask = autoDeleteInitializationTask;
}

/**
 * Adds a new periodical task to the periodical tasks list.
 * The periodical tasks list will be initializated with a default periodical task set (empty set at the moment).
 *
 * @param periodicalTasks - cannot be NULL
 */
void IG3MBuilder::addPeriodicalTask(PeriodicalTask* periodicalTask) {
  if (!periodicalTask) {
    ILogger::instance()->logError("LOGIC ERROR: trying to add a NULL periodicalTask object");
    return;
  }
  getPeriodicalTasks()->push_back(periodicalTask);
}

/**
 * Sets the periodical tasks list, ignoring the default periodical tasks list and the
 * periodical tasks previously added, if added.
 *
 * @param periodicalTasks - std::vector<PeriodicalTask*>
 */
void IG3MBuilder::setPeriodicalTasks(std::vector<PeriodicalTask*> periodicalTasks) {
  if (_periodicalTasks) {
    ILogger::instance()->logWarning("LOGIC WARNING: periodical tasks previously set will be ignored and deleted");
    for (unsigned int i = 0; i < _periodicalTasks->size(); i++) {
      delete _periodicalTasks->at(i);
    }
    _periodicalTasks->clear();
  }
  else {
    _periodicalTasks = new std::vector<PeriodicalTask*>;
  }
  for (unsigned int i = 0; i < periodicalTasks.size(); i++) {
    _periodicalTasks->push_back(periodicalTasks[i]);
  }
}

/**
 * Sets the _logFPS
 *
 * @param logFPS
 */
void IG3MBuilder::setLogFPS(const bool logFPS) {
  _logFPS = logFPS;
}

/**
 * Sets the _logDownloaderStatistics
 *
 * @param logDownloaderStatistics
 */
void IG3MBuilder::setLogDownloaderStatistics(const bool logDownloaderStatistics) {
  _logDownloaderStatistics = logDownloaderStatistics;
}

/**
 * Sets the _userData
 *
 * @param userData - cannot be NULL.
 */
void IG3MBuilder::setUserData(WidgetUserData *userData) {
  if (_userData) {
    ILogger::instance()->logError("LOGIC ERROR: _userData already initialized");
    return;
  }
  if (!userData) {
    ILogger::instance()->logError("LOGIC ERROR: userData cannot be NULL");
    return;
  }
  _userData = userData;
}

/**
 * Creates the generic widget using all the parameters previously set.
 *
 * @return G3MWidget*
 */
G3MWidget* IG3MBuilder::create() {
  /**
   * If any renderers were set or added, the main renderer will be a composite renderer.
   *    If the renderers list does not contain a tileRenderer, it will be created and added.
   *    The renderers contained in the list, will be added to the main renderer.
   * If not, the main renderer will be made up of an only renderer (tileRenderer).
   */
  Renderer* mainRenderer = NULL;
  if (getRenderers()->size() > 0) {
    mainRenderer = new CompositeRenderer();
    if (!containsTileRenderer(*getRenderers())) {
      ((CompositeRenderer *) mainRenderer)->addRenderer(getTileRendererBuilder()->create());
    }
    for (unsigned int i = 0; i < getRenderers()->size(); i++) {
      ((CompositeRenderer *) mainRenderer)->addRenderer(getRenderers()->at(i));
    }
  }
  else {
    mainRenderer = getTileRendererBuilder()->create();
  }
  
  G3MWidget * g3mWidget = G3MWidget::create(getGL(), //
                                            getStorage(), //
                                            getDownloader(), //
                                            getThreadUtils(), //
                                            getPlanet(), //
                                            *getCameraConstraints(), //
                                            getCameraRenderer(), //
                                            mainRenderer, //
                                            getBusyRenderer(), //
                                            *getBackgroundColor(), //
                                            getLogFPS(), //
                                            getLogDownloaderStatistics(), //
                                            getInitializationTask(), //
                                            getAutoDeleteInitializationTask(), //
                                            *getPeriodicalTasks());
  
  g3mWidget->setUserData(getUserData());
  
  _gl = NULL;
  _storage = NULL;
  _downloader = NULL;
  _threadUtils = NULL;
  _planet = NULL;
  delete _cameraConstraints;
  _cameraConstraints = NULL;
  _cameraRenderer = NULL;
  delete _renderers;
  _renderers = NULL;
  _busyRenderer = NULL;
  _initializationTask = NULL;
  delete _periodicalTasks;
  _periodicalTasks = NULL;
  _userData = NULL;
  
  return g3mWidget;
}

std::vector<ICameraConstrainer*>* IG3MBuilder::createDefaultCameraConstraints() {
  std::vector<ICameraConstrainer*>* cameraConstraints = new std::vector<ICameraConstrainer*>;
  SimpleCameraConstrainer* scc = new SimpleCameraConstrainer();
  cameraConstraints->push_back(scc);
  
  return cameraConstraints;
}

CameraRenderer* IG3MBuilder::createDefaultCameraRenderer() {
  CameraRenderer* cameraRenderer = new CameraRenderer();
  const bool useInertia = true;
  cameraRenderer->addHandler(new CameraSingleDragHandler(useInertia));
  const bool processRotation = true;
  const bool processZoom = true;
  cameraRenderer->addHandler(new CameraDoubleDragHandler(processRotation,
                                                         processZoom));
  cameraRenderer->addHandler(new CameraRotationHandler());
  cameraRenderer->addHandler(new CameraDoubleTapHandler());
  
  return cameraRenderer;
}

std::vector<PeriodicalTask*>* IG3MBuilder::createDefaultPeriodicalTasks() {
  std::vector<PeriodicalTask*>* periodicalTasks = new std::vector<PeriodicalTask*>;
  
  return periodicalTasks;
}

std::vector<Renderer*>* IG3MBuilder::createDefaultRenderers() {
  std::vector<Renderer*>* renderers = new std::vector<Renderer*>;
  
  return renderers;
}

/**
 * Returns TRUE if the given renderer list contains, at least, an instance of 
 * the TileRenderer class. Returns FALSE if not.
 *
 * @return bool
 */
bool IG3MBuilder::containsTileRenderer(std::vector<Renderer*> renderers) {
  for (unsigned int i = 0; i < renderers.size(); i++) {
    if (renderers[i]->isTileRenderer()) {
      return true;
    }
  }
  return false;
}

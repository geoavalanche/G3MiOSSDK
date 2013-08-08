//
//  MapBooBuilder.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 5/25/13.
//
//

#include "MapBooBuilder.hpp"

#include "ILogger.hpp"
#include "CompositeRenderer.hpp"
#include "PlanetRenderer.hpp"

#include "EllipsoidalTileTessellator.hpp"
#include "MultiLayerTileTexturizer.hpp"
#include "TilesRenderParameters.hpp"
#include "DownloadPriority.hpp"
#include "G3MWidget.hpp"
#include "SimpleCameraConstrainer.hpp"
#include "CameraRenderer.hpp"
#include "CameraSingleDragHandler.hpp"
#include "CameraDoubleDragHandler.hpp"
#include "CameraRotationHandler.hpp"
#include "CameraDoubleTapHandler.hpp"
#include "BusyMeshRenderer.hpp"
#include "GInitializationTask.hpp"
#include "PeriodicalTask.hpp"
#include "IDownloader.hpp"
#include "IBufferDownloadListener.hpp"
#include "IJSONParser.hpp"
#include "JSONObject.hpp"
#include "JSONString.hpp"
#include "JSONArray.hpp"
#include "MapBooSceneDescription.hpp"
#include "IThreadUtils.hpp"
#include "OSMLayer.hpp"
#include "MapQuestLayer.hpp"
#include "BingMapsLayer.hpp"
#include "CartoDBLayer.hpp"
#include "MapBoxLayer.hpp"
#include "WMSLayer.hpp"
#include "LayerTilesRenderParameters.hpp"
#include "IWebSocketListener.hpp"
#include "IWebSocket.hpp"

MapBoo_Scene::~MapBoo_Scene() {
  delete _baseLayer;
  delete _overlayLayer;
}


MapBooBuilder::MapBooBuilder(const URL& serverURL,
                             const URL& tubesURL,
                             bool useWebSockets,
                             const std::string& applicationId,
                             MapBooApplicationChangeListener* applicationListener) :
_serverURL(serverURL),
_tubesURL(tubesURL),
_useWebSockets(useWebSockets),
_applicationId(applicationId),
_applicationName(""),
_applicationDescription(""),
_applicationTimestamp(-1),
//_sceneBaseLayer(NULL),
//_sceneOverlayLayer(NULL),
//_sceneUser(""),
//_sceneBackgroundColor( Color::newFromRGBA(0, 0, 0, 1) ),
_gl(NULL),
_g3mWidget(NULL),
_storage(NULL),
_threadUtils(NULL),
_layerSet( new LayerSet() ),
_downloader(NULL),
_applicationListener(applicationListener),
_gpuProgramManager(NULL),
_isApplicationTubeOpen(false),
_applicationTubeWebSocket(NULL),
_currentScene(0)
{

}

GPUProgramManager* MapBooBuilder::getGPUProgramManager() {
  if (_gpuProgramManager == NULL) {
    _gpuProgramManager = createGPUProgramManager();
  }
  return _gpuProgramManager;
}

IDownloader* MapBooBuilder::getDownloader() {
  if (_downloader == NULL) {
    _downloader = createDownloader();
  }
  return _downloader;
}

IThreadUtils* MapBooBuilder::getThreadUtils() {
  if (_threadUtils == NULL) {
    _threadUtils = createThreadUtils();
  }
  return _threadUtils;
}

void MapBooBuilder::setGL(GL *gl) {
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

GL* MapBooBuilder::getGL() {
  if (!_gl) {
    ILogger::instance()->logError("Logic Error: _gl not initialized");
  }

  return _gl;
}

PlanetRenderer* MapBooBuilder::createPlanetRenderer() {
  const TileTessellator* tessellator = new EllipsoidalTileTessellator(true);

  ElevationDataProvider* elevationDataProvider = NULL;
  const float verticalExaggeration = 1;
  TileTexturizer* texturizer = new MultiLayerTileTexturizer();
  TileRasterizer* tileRasterizer = NULL;

  const bool renderDebug = false;
  const bool useTilesSplitBudget = true;
  const bool forceFirstLevelTilesRenderOnStart = true;
  const bool incrementalTileQuality = false;

  const TilesRenderParameters* parameters = new TilesRenderParameters(renderDebug,
                                                                      useTilesSplitBudget,
                                                                      forceFirstLevelTilesRenderOnStart,
                                                                      incrementalTileQuality);

  const bool showStatistics = false;
  long long texturePriority = DownloadPriority::HIGHER;

  return new PlanetRenderer(tessellator,
                            elevationDataProvider,
                            verticalExaggeration,
                            texturizer,
                            tileRasterizer,
                            _layerSet,
                            parameters,
                            showStatistics,
                            texturePriority);
}

const Planet* MapBooBuilder::createPlanet() {
  return Planet::createEarth();
}

std::vector<ICameraConstrainer*>* MapBooBuilder::createCameraConstraints() {
  std::vector<ICameraConstrainer*>* cameraConstraints = new std::vector<ICameraConstrainer*>;
  SimpleCameraConstrainer* scc = new SimpleCameraConstrainer();
  cameraConstraints->push_back(scc);

  return cameraConstraints;
}

CameraRenderer* MapBooBuilder::createCameraRenderer() {
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

Renderer* MapBooBuilder::createBusyRenderer() {
  return new BusyMeshRenderer(Color::newFromRGBA(0, 0, 0, 1));
}

MapQuestLayer* MapBooBuilder::parseMapQuestLayer(const JSONObject* jsonBaseLayer,
                                                 const TimeInterval& timeToCache) const {
  const std::string imagery = jsonBaseLayer->getAsString("imagery", "<imagery not present>");
  if (imagery.compare("OpenAerial") == 0) {
    return MapQuestLayer::newOpenAerial(timeToCache);
  }

  // defaults to OSM
  return MapQuestLayer::newOSM(timeToCache);
}

BingMapsLayer* MapBooBuilder::parseBingMapsLayer(const JSONObject* jsonBaseLayer,
                                                 const TimeInterval& timeToCache) const {
  const std::string key = jsonBaseLayer->getAsString("key", "");
  const std::string imagerySet = jsonBaseLayer->getAsString("imagerySet", "Aerial");

  return new BingMapsLayer(imagerySet, key, timeToCache);
}

CartoDBLayer* MapBooBuilder::parseCartoDBLayer(const JSONObject* jsonBaseLayer,
                                               const TimeInterval& timeToCache) const {
  const std::string userName = jsonBaseLayer->getAsString("userName", "");
  const std::string table    = jsonBaseLayer->getAsString("table",    "");

  return new CartoDBLayer(userName, table, timeToCache);
}

MapBoxLayer* MapBooBuilder::parseMapBoxLayer(const JSONObject* jsonBaseLayer,
                                             const TimeInterval& timeToCache) const {
  const std::string mapKey = jsonBaseLayer->getAsString("mapKey", "");

  return new MapBoxLayer(mapKey, timeToCache);
}

WMSLayer* MapBooBuilder::parseWMSLayer(const JSONObject* jsonBaseLayer) const {

  const std::string mapLayer = jsonBaseLayer->getAsString("layerName", "");
  const URL mapServerURL = URL(jsonBaseLayer->getAsString("server", ""), false);
  const std::string versionStr = jsonBaseLayer->getAsString("version", "");
  WMSServerVersion mapServerVersion = WMS_1_1_0;
  if (versionStr.compare("WMS_1_3_0") == 0) {
    mapServerVersion = WMS_1_3_0;
  }
  const std::string queryLayer = jsonBaseLayer->getAsString("queryLayer", "");
  const std::string style = jsonBaseLayer->getAsString("style", "");
  const URL queryServerURL = URL("", false);
  const WMSServerVersion queryServerVersion = mapServerVersion;
  const double lowerLat = jsonBaseLayer->getAsNumber("lowerLat", -90.0);
  const double lowerLon = jsonBaseLayer->getAsNumber("lowerLon", -180.0);
  const double upperLat = jsonBaseLayer->getAsNumber("upperLat", 90.0);
  const double upperLon = jsonBaseLayer->getAsNumber("upperLon", 180.0);
  const Sector sector = Sector(Geodetic2D(Angle::fromDegrees(lowerLat), Angle::fromDegrees(lowerLon)),
                               Geodetic2D(Angle::fromDegrees(upperLat), Angle::fromDegrees(upperLon)));
  std::string imageFormat = jsonBaseLayer->getAsString("imageFormat", "image/png");
  if (imageFormat.compare("JPG") == 0) {
    imageFormat = "image/jpeg";
  }
  const std::string srs = jsonBaseLayer->getAsString("projection", "EPSG_4326");
  LayerTilesRenderParameters* layerTilesRenderParameters = NULL;
  if (srs.compare("EPSG_4326") == 0) {
    layerTilesRenderParameters = LayerTilesRenderParameters::createDefaultNonMercator(Sector::fullSphere());
  }
  else if (srs.compare("EPSG_900913") == 0) {
    layerTilesRenderParameters = LayerTilesRenderParameters::createDefaultMercator(0, 17);
  }
  const bool isTransparent = jsonBaseLayer->getAsBoolean("transparent", false);
  const double expiration = jsonBaseLayer->getAsNumber("expiration", 0);
  const long long milliseconds = IMathUtils::instance()->round(expiration);
  const TimeInterval timeToCache = TimeInterval::fromMilliseconds(milliseconds);
  const bool readExpired = jsonBaseLayer->getAsBoolean("acceptExpiration", false);

  return new WMSLayer(mapLayer,
                      mapServerURL,
                      mapServerVersion,
                      queryLayer,
                      queryServerURL,
                      queryServerVersion,
                      sector,
                      imageFormat,
                      (srs.compare("EPSG_4326") == 0) ? "EPSG:4326" : "EPSG:900913",
                      style,
                      isTransparent,
                      NULL,
                      timeToCache,
                      readExpired,
                      layerTilesRenderParameters);
}


Layer* MapBooBuilder::parseLayer(const JSONBaseObject* jsonBaseObjectLayer) const {

  if (jsonBaseObjectLayer->asNull() != NULL) {
    return NULL;
  }

  const TimeInterval defaultTimeToCache = TimeInterval::fromDays(30);

  const JSONObject* jsonBaseLayer = jsonBaseObjectLayer->asObject();
  if (jsonBaseLayer == NULL) {
    ILogger::instance()->logError("Layer is not a json object");
    return NULL;
  }

  const std::string layerType = jsonBaseLayer->getAsString("layer", "<layer not present>");
  if (layerType.compare("OSM") == 0) {
    return new OSMLayer(defaultTimeToCache);
  }
  else if (layerType.compare("MapQuest") == 0) {
    return parseMapQuestLayer(jsonBaseLayer, defaultTimeToCache);
  }
  else if (layerType.compare("BingMaps") == 0) {
    return parseBingMapsLayer(jsonBaseLayer, defaultTimeToCache);
  }
  else if (layerType.compare("CartoDB") == 0) {
    return parseCartoDBLayer(jsonBaseLayer, defaultTimeToCache);
  }
  else if (layerType.compare("MapBox") == 0) {
    return parseMapBoxLayer(jsonBaseLayer, defaultTimeToCache);
  }
  else if (layerType.compare("WMS") == 0) {
    return parseWMSLayer(jsonBaseLayer);
  }
  else {
    ILogger::instance()->logError("Unsupported layer type \"%s\"", layerType.c_str());
    return NULL;
  }
}


void MapBooBuilder::parseApplicationDescription(const std::string& json,
                                                const URL& url) {
  const JSONBaseObject* jsonBaseObject = IJSONParser::instance()->parse(json, true);

  if (jsonBaseObject == NULL) {
    ILogger::instance()->logError("Can't parse SceneJSON from %s",
                                  url.getPath().c_str());
  }
  else {
    const JSONObject* jsonObject = jsonBaseObject->asObject();
    if (jsonObject == NULL) {
      ILogger::instance()->logError("Invalid SceneJSON (1)");
    }
    else {
      const JSONString* error = jsonObject->getAsString("error");
      if (error == NULL) {
        const int timestamp = (int) jsonObject->getAsNumber("timestamp", 0);

        if (getApplicationTimestamp() != timestamp) {
//          const JSONString* jsonUser = jsonObject->getAsString("user");
//          if (jsonUser != NULL) {
//            setApplicationUser(jsonUser->value());
//          }

          //id

          const JSONString* jsonName = jsonObject->getAsString("name");
          if (jsonName != NULL) {
            setApplicationName(jsonName->value());
          }

          const JSONString* jsonDescription = jsonObject->getAsString("description");
          if (jsonDescription != NULL) {
            setApplicationDescription(jsonDescription->value());
          }

//          const JSONString* jsonBGColor = jsonObject->getAsString("bgColor");
//          if (jsonBGColor != NULL) {
//            const Color* bgColor = Color::parse(jsonBGColor->value());
//            if (bgColor == NULL) {
//              ILogger::instance()->logError("Invalid format in attribute 'bgColor' (%s)",
//                                            jsonBGColor->value().c_str());
//            }
//            else {
//              setApplicationBackgroundColor(*bgColor);
//              delete bgColor;
//            }
//          }

//          const JSONBaseObject* jsonBaseLayer = jsonObject->get("baseLayer");
//          if (jsonBaseLayer != NULL) {
//            setApplicationBaseLayer( parseLayer(jsonBaseLayer) );
//          }
//
//          const JSONBaseObject* jsonOverlayLayer = jsonObject->get("overlayLayer");
//          if (jsonOverlayLayer != NULL) {
//            setApplicationOverlayLayer( parseLayer(jsonOverlayLayer) );
//          }

          //tags

          setApplicationTimestamp(timestamp);
        }
      }
      else {
        ILogger::instance()->logError("Server Error: %s",
                                      error->value().c_str());
      }
    }
    
    delete jsonBaseObject;
  }
  
}


class MapBooBuilder_SceneDescriptionBufferListener : public IBufferDownloadListener {
private:
  MapBooBuilder* _builder;

public:
  MapBooBuilder_SceneDescriptionBufferListener(MapBooBuilder* builder) :
  _builder(builder)
  {
  }


  void onDownload(const URL& url,
                  IByteBuffer* buffer,
                  bool expired) {

    _builder->parseApplicationDescription(buffer->getAsString(), url);
    delete buffer;
  }

  void onError(const URL& url) {
    ILogger::instance()->logError("Can't download SceneJSON from %s",
                                  url.getPath().c_str());
  }

  void onCancel(const URL& url) {
    // do nothing
  }

  void onCanceledDownload(const URL& url,
                          IByteBuffer* buffer,
                          bool expired) {
    // do nothing
  }

};


class MapBooBuilder_PollingScenePeriodicalTask : public GTask {
private:
  MapBooBuilder* _builder;

  long long _requestId;


  URL getURL() const {
    const int applicationTimestamp = _builder->getApplicationTimestamp();

    const URL _sceneDescriptionURL = _builder->createPollingApplicationDescriptionURL();

    if (applicationTimestamp < 0) {
      return _sceneDescriptionURL;
    }

    IStringBuilder* ib = IStringBuilder::newStringBuilder();

    ib->addString(_sceneDescriptionURL.getPath());
    ib->addString("?lastTs=");
    ib->addInt(applicationTimestamp);

    const std::string path = ib->getString();

    delete ib;

    return URL(path, false);
  }


public:
  MapBooBuilder_PollingScenePeriodicalTask(MapBooBuilder* builder) :
  _builder(builder),
  _requestId(-1)
  {

  }

  void run(const G3MContext* context) {
    IDownloader* downloader = context->getDownloader();
    if (_requestId >= 0) {
      downloader->cancelRequest(_requestId);
    }

    _requestId = downloader->requestBuffer(getURL(),
                                           DownloadPriority::HIGHEST,
                                           TimeInterval::zero(),
                                           true,
                                           new MapBooBuilder_SceneDescriptionBufferListener(_builder),
                                           true);
  }
};


//void MapBooBuilder::recreateLayerSet() {
//  _layerSet->removeAllLayers(false);
//
//  if (_sceneBaseLayer != NULL) {
//    _layerSet->addLayer(_sceneBaseLayer);
//  }
//
//  if (_sceneOverlayLayer != NULL) {
//    _layerSet->addLayer(_sceneOverlayLayer);
//  }
//}

//void MapBooBuilder::setSceneBaseLayer(Layer* baseLayer) {
//  if (baseLayer == NULL) {
//    ILogger::instance()->logError("Base Layer can't be NULL");
//    return;
//  }
//
//  if (_sceneBaseLayer != baseLayer) {
//    delete _sceneBaseLayer;
//    _sceneBaseLayer = baseLayer;
//
//    recreateLayerSet();
//
//    if (_applicationListener != NULL) {
//      _applicationListener->onBaseLayerChanged(_sceneBaseLayer);
//    }
//  }
//}
//
//void MapBooBuilder::setSceneOverlayLayer(Layer* overlayLayer) {
//  if (_sceneOverlayLayer != overlayLayer) {
//    delete _sceneOverlayLayer;
//    _sceneOverlayLayer = overlayLayer;
//
//    recreateLayerSet();
//
//    if (_applicationListener != NULL) {
//      _applicationListener->onOverlayLayerChanged(_sceneOverlayLayer);
//    }
//  }
//}

const URL MapBooBuilder::createApplicationTubeURL() const {
  const std::string tubesPath = _tubesURL.getPath();

  return URL(tubesPath + "/application/" + _applicationId + "/runtime", false);
}

//const URL MapBooBuilder::createApplicationDescriptionURL() const {
//  const std::string serverPath = _serverURL.getPath();
//
//  return URL(serverPath + "/application/" + _applicationId + "/runtime", false);
//}


class MapBooBuilder_TubeWatchdogPeriodicalTask : public GTask {
private:
  MapBooBuilder* _builder;
  bool _firstRun;

public:
  MapBooBuilder_TubeWatchdogPeriodicalTask(MapBooBuilder* builder) :
  _builder(builder),
  _firstRun(true)
  {
  }

  void run(const G3MContext* context) {
    if (_firstRun) {
      _firstRun = false;
    }
    else {
      if (!_builder->isApplicationTubeOpen()) {
        _builder->openApplicationTube(context);
      }
    }
  }

};


std::vector<PeriodicalTask*>* MapBooBuilder::createPeriodicalTasks() {
  std::vector<PeriodicalTask*>* periodicalTasks = new std::vector<PeriodicalTask*>();

  if (_useWebSockets) {
    periodicalTasks->push_back(new PeriodicalTask(TimeInterval::fromSeconds(2),
                                                  new MapBooBuilder_TubeWatchdogPeriodicalTask(this)));
  }
  else {
    periodicalTasks->push_back(new PeriodicalTask(TimeInterval::fromSeconds(2),
                                                  new MapBooBuilder_PollingScenePeriodicalTask(this)));
  }

  return periodicalTasks;
}

IStorage* MapBooBuilder::getStorage() {
  if (_storage == NULL) {
    _storage = createStorage();
  }
  return _storage;
}

class MapBooBuilder_ApplicationTubeListener : public IWebSocketListener {
private:
  MapBooBuilder* _builder;

public:
  MapBooBuilder_ApplicationTubeListener(MapBooBuilder* builder) :
  _builder(builder)
  {
  }

  ~MapBooBuilder_ApplicationTubeListener() {
  }

  void onOpen(IWebSocket* ws) {
    ILogger::instance()->logError("Tube '%s' opened!",
                                  ws->getURL().getPath().c_str());
    _builder->setApplicationTubeOpened(true);
  }

  void onError(IWebSocket* ws,
               const std::string& error) {
    ILogger::instance()->logError("Error '%s' on Tube '%s'",
                                  error.c_str(),
                                  ws->getURL().getPath().c_str());
  }

  void onMesssage(IWebSocket* ws,
                  const std::string& message) {
    _builder->parseApplicationDescription(message, ws->getURL());
  }

  void onClose(IWebSocket* ws) {
    ILogger::instance()->logError("Tube '%s' closed!",
                                  ws->getURL().getPath().c_str());
    _builder->setApplicationTubeOpened(false);
  }
};

class MapBooBuilder_SceneTubeConnector : public GInitializationTask {
private:
  MapBooBuilder* _builder;

public:
  MapBooBuilder_SceneTubeConnector(MapBooBuilder* builder) :
  _builder(builder)
  {
  }

  void run(const G3MContext* context) {
    _builder->openApplicationTube(context);
  }

  bool isDone(const G3MContext* context) {
    return true;
  }
};

void MapBooBuilder::openApplicationTube(const G3MContext* context) {
  const bool autodeleteListener  = true;
  const bool autodeleteWebSocket = true;

  _applicationTubeWebSocket = context->getFactory()->createWebSocket(createApplicationTubeURL(),
                                                                     new MapBooBuilder_ApplicationTubeListener(this),
                                                                     autodeleteListener,
                                                                     autodeleteWebSocket);
}


GInitializationTask* MapBooBuilder::createInitializationTask() {
  return _useWebSockets ? new MapBooBuilder_SceneTubeConnector(this) : NULL;
}

const MapBoo_Scene* MapBooBuilder::getCurrentScene() {
  const int applicationScenesSize = _applicationScenes.size();
  if ((applicationScenesSize == 0) ||
      (_currentScene < 0) ||
      (_currentScene >= applicationScenesSize)) {
    return NULL;
  }
  return _applicationScenes[_currentScene];
}

Color MapBooBuilder::getBackgroundColor() {
  const MapBoo_Scene* scene = getCurrentScene();
  return (scene == NULL) ? Color::black() : scene->getBackgroundColor();
}

G3MWidget* MapBooBuilder::create() {
  if (_g3mWidget != NULL) {
    ILogger::instance()->logError("The G3MWidget was already created, can't be created more than once");
    return NULL;
  }


  CompositeRenderer* mainRenderer = new CompositeRenderer();

  PlanetRenderer* planetRenderer = createPlanetRenderer();
  mainRenderer->addRenderer(planetRenderer);

  std::vector<ICameraConstrainer*>* cameraConstraints = createCameraConstraints();

  GInitializationTask* initializationTask = createInitializationTask();

  std::vector<PeriodicalTask*>* periodicalTasks = createPeriodicalTasks();

  ICameraActivityListener* cameraActivityListener = NULL;

  _g3mWidget = G3MWidget::create(getGL(),
                                 getStorage(),
                                 getDownloader(),
                                 getThreadUtils(),
                                 cameraActivityListener,
                                 createPlanet(),
                                 *cameraConstraints,
                                 createCameraRenderer(),
                                 mainRenderer,
                                 createBusyRenderer(),
//                                 *_sceneBackgroundColor,
                                 getBackgroundColor(),
                                 false,      // logFPS
                                 false,      // logDownloaderStatistics
                                 initializationTask,
                                 true,       // autoDeleteInitializationTask
                                 *periodicalTasks,
                                 getGPUProgramManager());
  delete cameraConstraints;
  delete periodicalTasks;

  return _g3mWidget;
}

//class MapBooBuilder_ScenesDescriptionsBufferListener : public IBufferDownloadListener {
//private:
//  MapBooBuilderScenesDescriptionsListener* _listener;
//  const bool _autoDelete;
//
//public:
//  MapBooBuilder_ScenesDescriptionsBufferListener(MapBooBuilderScenesDescriptionsListener* listener,
//                                                 bool autoDelete) :
//  _listener(listener),
//  _autoDelete(autoDelete)
//  {
//
//  }
//
//
//  void onDownload(const URL& url,
//                  IByteBuffer* buffer,
//                  bool expired) {
//
//    const JSONBaseObject* jsonBaseObject = IJSONParser::instance()->parse(buffer);
//
//    if (jsonBaseObject == NULL) {
//      ILogger::instance()->logError("Can't parse ScenesDescriptionJSON from %s",
//                                    url.getPath().c_str());
//      onError(url);
//    }
//    else {
//      const JSONArray* jsonScenesDescriptions = jsonBaseObject->asArray();
//      if (jsonScenesDescriptions == NULL) {
//        ILogger::instance()->logError("ScenesDescriptionJSON: invalid format (1)");
//        onError(url);
//      }
//      else {
//        std::vector<MapBooSceneDescription*>* scenesDescriptions = new std::vector<MapBooSceneDescription*>();
//
//        const int size = jsonScenesDescriptions->size();
//
//        for (int i = 0; i < size; i++) {
//          const JSONObject* jsonSceneDescription = jsonScenesDescriptions->getAsObject(i);
//          if (jsonSceneDescription == NULL) {
//            ILogger::instance()->logError("ScenesDescriptionJSON: invalid format (2) at index #%d", i);
//          }
//          else {
//            const std::string id          = jsonSceneDescription->getAsString("id",          "<invalid id>");
//            const std::string user        = jsonSceneDescription->getAsString("user",        "<invalid user>");
//            const std::string name        = jsonSceneDescription->getAsString("name",        "<invalid name>");
//            const std::string description = jsonSceneDescription->getAsString("description", "");
//            const std::string iconURL     = jsonSceneDescription->getAsString("iconURL",     "<invalid iconURL>");
//
//            std::vector<std::string> tags;
//            const JSONArray* jsonTags = jsonSceneDescription->getAsArray("tags");
//            if (jsonTags == NULL) {
//              ILogger::instance()->logError("ScenesDescriptionJSON: invalid format (3) at index #%d", i);
//            }
//            else {
//              const int tagsCount = jsonTags->size();
//              for (int j = 0; j < tagsCount; j++) {
//                const std::string tag = jsonTags->getAsString(j, "");
//                if (tag.size() > 0) {
//                  tags.push_back(tag);
//                }
//              }
//            }
//
//            scenesDescriptions->push_back( new MapBooSceneDescription(id,
//                                                                      user,
//                                                                      name,
//                                                                      description,
//                                                                      iconURL,
//                                                                      tags) );
//
//          }
//        }
//
//        _listener->onDownload(scenesDescriptions);
//        if (_autoDelete) {
//          delete _listener;
//        }
//      }
//
//      delete jsonBaseObject;
//    }
//
//    delete buffer;
//  }
//
//  void onError(const URL& url) {
//    _listener->onError();
//    if (_autoDelete) {
//      delete _listener;
//    }
//  }
//
//  void onCancel(const URL& url) {
//    // do nothing
//  }
//
//  void onCanceledDownload(const URL& url,
//                          IByteBuffer* buffer,
//                          bool expired) {
//    // do nothing
//  }
//
//};

//const URL MapBooBuilder::createApplicationsDescriptionsURL() const {
//  const std::string serverPath = _serverURL.getPath();
//
//  return URL(serverPath + "/scenes/", false);
//}


//void MapBooBuilder::requestScenesDescriptions(MapBooBuilderScenesDescriptionsListener* listener,
//                                              bool autoDelete) {
//  getDownloader()->requestBuffer(createScenesDescriptionsURL(),
//                                 DownloadPriority::HIGHEST,
//                                 TimeInterval::zero(),
//                                 true,
//                                 new MapBooBuilder_ScenesDescriptionsBufferListener(listener, autoDelete),
//                                 true);
//}

int MapBooBuilder::getApplicationTimestamp() const {
  return _applicationTimestamp;
}

void MapBooBuilder::setApplicationTimestamp(const int timestamp) {
  _applicationTimestamp = timestamp;
}
//
//void MapBooBuilder::setApplicationUser(const std::string& user) {
//  if (_applicationUser.compare(user) != 0) {
//    _applicationUser = user;
//
//    if (_applicationListener != NULL) {
//      _applicationListener->onUserChanged(_sceneUser);
//    }
//  }
//}

void MapBooBuilder::setApplicationName(const std::string& name) {
  if (_applicationName.compare(name) != 0) {
    _applicationName = name;

    if (_applicationListener != NULL) {
      _applicationListener->onNameChanged(_applicationName);
    }
  }
}

void MapBooBuilder::setApplicationDescription(const std::string& description) {
  if (_applicationDescription.compare(description) != 0) {
    _applicationDescription = description;

    if (_applicationListener != NULL) {
      _applicationListener->onDescriptionChanged(_applicationDescription);
    }
  }
}

//void MapBooBuilder::setSceneBackgroundColor(const Color& backgroundColor) {
//  if (!_sceneBackgroundColor->isEqualsTo(backgroundColor)) {
//    delete _sceneBackgroundColor;
//    _sceneBackgroundColor = new Color(backgroundColor);
//
//    if (_g3mWidget != NULL) {
//      _g3mWidget->setBackgroundColor(*_sceneBackgroundColor);
//    }
//
//    if (_applicationListener != NULL) {
//      _applicationListener->onBackgroundColorChanged(*_sceneBackgroundColor);
//    }
//  }
//}

//class MapBooBuilder_ChangeSceneIdTask : public GTask {
//private:
//  MapBooBuilder*    _builder;
//  const std::string _applicationId;
//
//public:
//  MapBooBuilder_ChangeSceneIdTask(MapBooBuilder* builder,
//                                  const std::string& applicationId) :
//  _builder(builder),
//  _applicationId(applicationId)
//  {
//  }
//
//  void run(const G3MContext* context) {
//    _builder->rawChangeApplication(_applicationId);
//  }
//};
//
//void MapBooBuilder::changeApplication(const std::string& applicationId) {
//  if (applicationId.compare(_applicationId) != 0) {
//    getThreadUtils()->invokeInRendererThread(new MapBooBuilder_ChangeSceneIdTask(this, applicationId),
//                                             true);
//  }
//}

//void MapBooBuilder::resetApplication(const std::string& applicationId) {
//  _applicationId = applicationId;
//
//  _applicationTimestamp = -1;
//
////  delete _sceneBaseLayer;
////  _sceneBaseLayer = NULL;
////
////  delete _sceneOverlayLayer;
////  _sceneOverlayLayer = NULL;
//
////  _sceneUser = "";
//
//  _applicationName = "";
//
//  _applicationDescription = "";
//
////  delete _sceneBackgroundColor;
////  _sceneBackgroundColor = Color::newFromRGBA(0, 0, 0, 1);
//}

//void MapBooBuilder::resetG3MWidget() {
//  _layerSet->removeAllLayers(false);
//
//  if (_g3mWidget != NULL) {
//    _g3mWidget->setBackgroundColor(*_sceneBackgroundColor);
//
//    // force inmediate ejecution of PeriodicalTasks
//    _g3mWidget->resetPeriodicalTasksTimeouts();
//  }
//}

void MapBooBuilder::setApplicationTubeOpened(bool open) {
  if (_isApplicationTubeOpen != open) {
    _isApplicationTubeOpen = open;
    if (!_isApplicationTubeOpen) {
      _applicationTubeWebSocket = NULL;
    }
  }
}

//void MapBooBuilder::rawChangeApplication(const std::string& applicationId) {
//  if (applicationId.compare(_applicationId) != 0) {
//    resetApplication(applicationId);
//    
//    resetG3MWidget();
//    
//    if (_applicationListener != NULL) {
//      _applicationListener->onApplicationChanged(applicationId);
//    }
//    
//    if (_sceneTubeWebSocket != NULL) {
//      _sceneTubeWebSocket->close();
//    }
//  }
//}

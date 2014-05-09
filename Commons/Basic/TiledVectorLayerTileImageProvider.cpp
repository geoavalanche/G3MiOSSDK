//
//  TiledVectorLayerTileImageProvider.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 4/30/14.
//
//

#include "TiledVectorLayerTileImageProvider.hpp"
#include "TiledVectorLayer.hpp"
#include "Tile.hpp"
#include "IDownloader.hpp"
#include "TileImageListener.hpp"
#include "TileImageContribution.hpp"
#include "GEOJSONParser.hpp"
#include "GEOObject.hpp"
#include "GEORasterSymbolizer.hpp"
#include "IFactory.hpp"
#include "ICanvas.hpp"
#include "GEORasterProjection.hpp"


TiledVectorLayerTileImageProvider::GEOJSONBufferParser::~GEOJSONBufferParser() {
  if (_imageAssembler != NULL) {
    _imageAssembler->deletedParser();
  }

  delete _buffer;
  delete _geoObject;
#ifdef JAVA_CODE
  super.dispose();
#endif
}

void TiledVectorLayerTileImageProvider::GEOJSONBufferParser::cancel() {
  _imageAssembler = NULL;
}

void TiledVectorLayerTileImageProvider::GEOJSONBufferParser::runInBackground(const G3MContext* context) {
  if ((_imageAssembler != NULL) && (_buffer != NULL)) {
    _geoObject = GEOJSONParser::parseJSON(_buffer);
  }
}

void TiledVectorLayerTileImageProvider::GEOJSONBufferParser::onPostExecute(const G3MContext* context) {
  if (_imageAssembler != NULL) {
    GEOObject* geoObject = _geoObject;
    _geoObject = NULL; // moves ownership of _geoObject to _imageAssembler
    _imageAssembler->parsedGEOObject(geoObject);
  }
}


void TiledVectorLayerTileImageProvider::GEOJSONBufferDownloadListener::onDownload(const URL& url,
                                                                                  IByteBuffer* buffer,
                                                                                  bool expired) {
  if (_imageAssembler == NULL) {
    delete buffer;
  }
  else {
    _imageAssembler->bufferDownloaded(buffer);
  }
}

void TiledVectorLayerTileImageProvider::GEOJSONBufferDownloadListener::onError(const URL& url) {
  if (_imageAssembler != NULL) {
    _imageAssembler->bufferDownloadError(url);
  }
}

void TiledVectorLayerTileImageProvider::GEOJSONBufferDownloadListener::onCancel(const URL& url) {
  if (_imageAssembler != NULL) {
    _imageAssembler->bufferDownloadCanceled();
  }
}



TiledVectorLayerTileImageProvider::ImageAssembler::ImageAssembler(TiledVectorLayerTileImageProvider* tileImageProvider,
                                                                  const Tile*                        tile,
                                                                  const TileImageContribution*       contribution,
                                                                  TileImageListener*                 listener,
                                                                  bool                               deleteListener,
                                                                  const Vector2I&                    imageResolution,
                                                                  IDownloader*                       downloader,
                                                                  const IThreadUtils*                threadUtils) :
_tileImageProvider(tileImageProvider),
_tileId(tile->_id),
_tileSector(tile->_sector),
_tileIsMercator(tile->_mercator),
_tileLevel(tile->_level),
_contribution(contribution),
_listener(listener),
_deleteListener(deleteListener),
_imageWidth(imageResolution._x),
_imageHeight(imageResolution._y),
_downloader(downloader),
_threadUtils(threadUtils),
_canceled(false),
_downloadListener(NULL),
_downloadRequestId(-1),
_parser(NULL),
_symbolizer(NULL)
{
}

void TiledVectorLayerTileImageProvider::ImageAssembler::start(const TiledVectorLayer* layer,
                                                              const Tile*             tile,
                                                              long long               tileDownloadPriority,
                                                              bool                    logDownloadActivity) {
  _downloadListener = new GEOJSONBufferDownloadListener(this);

  _symbolizer = layer->symbolizerCopy();

  _downloadRequestId = layer->requestGEOJSONBuffer(tile,
                                                   _downloader,
                                                   tileDownloadPriority,
                                                   logDownloadActivity,
                                                   _downloadListener,
                                                   true /* deleteListener */);

}

TiledVectorLayerTileImageProvider::ImageAssembler::~ImageAssembler() {
  if (_downloadListener != NULL) {
    _downloadListener->deletedImageAssembler();
  }
  if (_parser != NULL) {
    _parser->deletedImageAssembler();
  }
  if (_deleteListener) {
    delete _listener;
  }
  TileImageContribution::releaseContribution(_contribution);
  delete _symbolizer;
}

void TiledVectorLayerTileImageProvider::ImageAssembler::cancel() {
  _canceled = true;
  if (_downloadRequestId >= 0) {
    _downloader->cancelRequest(_downloadRequestId);
    _downloadRequestId = -1;
  }
  if (_parser != NULL) {
    _parser->cancel();
  }

#warning TODO call listener cancel
  _listener->imageCreationCanceled(_tileId);
  _tileImageProvider->requestFinish(_tileId);
}

void TiledVectorLayerTileImageProvider::ImageAssembler::bufferDownloaded(IByteBuffer* buffer) {
  _downloadListener = NULL;
  _downloadRequestId = -1;

  if (_canceled) {
    delete buffer;
  }
  else {
    _parser = new GEOJSONBufferParser(this, buffer);
    _threadUtils->invokeAsyncTask(_parser,
                                  true);
  }
}

void TiledVectorLayerTileImageProvider::ImageAssembler::bufferDownloadError(const URL& url) {
  _downloadListener = NULL;
  _downloadRequestId = -1;

  _listener->imageCreationError(_tileId,
                                "Download error - " + url._path);
  _tileImageProvider->requestFinish(_tileId);
}

void TiledVectorLayerTileImageProvider::ImageAssembler::bufferDownloadCanceled() {
  _downloadListener = NULL;
  _downloadRequestId = -1;
}

void TiledVectorLayerTileImageProvider::CanvasImageListener::imageCreated(const IImage* image) {
  _imageAssembler->imageCreated(image);
}

void TiledVectorLayerTileImageProvider::ImageAssembler::imageCreated(const IImage* image) {
#warning compose imageId

//  if (_canceled) {
//    printf("**** break point\n");
//  }

  // retain the _contribution before calling the listener, as it takes full ownership of the contribution
  TileImageContribution::retainContribution(_contribution);

  _listener->imageCreated(_tileId,
                          image,
                          "VectorTiles" + _tileId,
                          _contribution);
  _tileImageProvider->requestFinish(_tileId);
}

void TiledVectorLayerTileImageProvider::ImageAssembler::parsedGEOObject(GEOObject* geoObject) {
  if (geoObject == NULL) {
    _listener->imageCreationError(_tileId, "GEOJSON parser error");
    if (_deleteListener) {
      delete _listener;
    }
  }
  else {
#warning Diego at work!
    ICanvas* canvas = IFactory::instance()->createCanvas();

    canvas->initialize(_imageWidth, _imageHeight);

    const GEORasterProjection* projection = new GEORasterProjection(_tileSector,
                                                                    _tileIsMercator,
                                                                    _imageWidth,
                                                                    _imageHeight);;
    geoObject->rasterize(_symbolizer,
                         canvas,
                         projection,
                         _tileLevel);

    delete projection;
    delete geoObject;

    canvas->createImage(new CanvasImageListener(this),
                        true /* autodelete */);

    delete canvas;

  }
}

void TiledVectorLayerTileImageProvider::ImageAssembler::deletedParser() {
  _parser = NULL;
}

const TileImageContribution* TiledVectorLayerTileImageProvider::contribution(const Tile* tile) {
  return _layer->contribution(tile);
}

void TiledVectorLayerTileImageProvider::create(const Tile* tile,
                                               const TileImageContribution* contribution,
                                               const Vector2I& resolution,
                                               long long tileDownloadPriority,
                                               bool logDownloadActivity,
                                               TileImageListener* listener,
                                               bool deleteListener,
                                               FrameTasksExecutor* frameTasksExecutor) {

  ImageAssembler* assembler = new ImageAssembler(this,
                                                 tile,
                                                 contribution,
                                                 listener,
                                                 deleteListener,
                                                 resolution,
                                                 _downloader,
                                                 _threadUtils);

  _assemblers[tile->_id] = assembler;

  assembler->start(_layer,
                   tile,
                   tileDownloadPriority,
                   logDownloadActivity);
}

void TiledVectorLayerTileImageProvider::cancel(const std::string& tileId) {
#ifdef C_CODE
  if (_assemblers.find(tileId) != _assemblers.end()) {
    ImageAssembler* assembler = _assemblers[tileId];

    assembler->cancel();
  }
#endif
#ifdef JAVA_CODE
  final ImageAssembler assembler = _assemblers.get(tileId);
  if (assembler != null) {
    assembler.cancel();
  }
#endif
}

void TiledVectorLayerTileImageProvider::requestFinish(const std::string& tileId) {
#ifdef C_CODE
  if (_assemblers.find(tileId) != _assemblers.end()) {
    ImageAssembler* assembler = _assemblers[tileId];

    _assemblers.erase(tileId);

    delete assembler;
  }
#endif
#ifdef JAVA_CODE
  final ImageAssembler assembler = _assemblers.remove(tileId);
  if (assembler != null) {
    assembler.dispose();
  }
#endif
}

//
//  StenciledMultiTexturedHUDQuadWidget.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 12/17/13.
//
//

#include "StenciledMultiTexturedHUDQuadWidget.hpp"

#include "Context.hpp"
#include "IDownloader.hpp"
#include "IImageDownloadListener.hpp"
#include "TexturesHandler.hpp"
#include "Camera.hpp"
#include "Vector3D.hpp"
#include "FloatBufferBuilderFromCartesian3D.hpp"
#include "FloatBufferBuilderFromCartesian2D.hpp"
#include "DirectMesh.hpp"
#include "TexturedMesh.hpp"
#include "HUDPosition.hpp"

class StenciledMultiTexturedHUDQuadWidget_ImageDownloadListener : public IImageDownloadListener {
private:
  StenciledMultiTexturedHUDQuadWidget* _quadWidget;
public:
  StenciledMultiTexturedHUDQuadWidget_ImageDownloadListener(StenciledMultiTexturedHUDQuadWidget* quadWidget) :
  _quadWidget(quadWidget)
  {
  }

  void onDownload(const URL& url,
                  IImage* image,
                  bool expired)  {
    _quadWidget->onImageDownload(image, url);
  }

  void onError(const URL& url) {
    _quadWidget->onImageDownloadError(url);
  }

  void onCancel(const URL& url) {
    // do nothing
  }

  void onCanceledDownload(const URL& url,
                          IImage* image,
                          bool expired) {
    // do nothing
  }

};


StenciledMultiTexturedHUDQuadWidget::~StenciledMultiTexturedHUDQuadWidget() {
  delete _image;
  delete _mesh;

  delete _x;
  delete _y;
}

Mesh* StenciledMultiTexturedHUDQuadWidget::createMesh(const G3MRenderContext* rc) {
  if (_image == NULL) {
    return NULL;
  }

  const TextureIDReference* texId = rc->getTexturesHandler()->getTextureIDReference(_image,
                                                                                    GLFormat::rgba(),
                                                                                    _imageURL.getPath(),
                                                                                    false);

  if (texId == NULL) {
    rc->getLogger()->logError("Can't upload texture to GPU");
    return NULL;
  }

  const TextureIDReference* texId2 = rc->getTexturesHandler()->getTextureIDReference(_image2,
                                                                                     GLFormat::rgba(),
                                                                                     _imageURL2.getPath(),
                                                                                     false);

  if (texId2 == NULL) {
    rc->getLogger()->logError("Can't upload texture to GPU");
    return NULL;
  }

  const TextureIDReference* stencilTexId = rc->getTexturesHandler()->getTextureIDReference(_stencilImage,
                                                                                     GLFormat::rgba(),
                                                                                     _imageURL2.getPath(),
                                                                                     false);

  if (stencilTexId == NULL) {
    rc->getLogger()->logError("Can't upload texture to GPU");
    return NULL;
  }


  const int viewPortWidth  = rc->getCurrentCamera()->getWidth();
  const int viewPortHeight = rc->getCurrentCamera()->getHeight();

  const float width = _width;
  const float height = _height;
  const float x = _x->getPosition(viewPortWidth, viewPortHeight, width, height);
  const float y = _y->getPosition(viewPortWidth, viewPortHeight, width, height);

  FloatBufferBuilderFromCartesian3D* vertices = FloatBufferBuilderFromCartesian3D::builderWithoutCenter();
  vertices->add( x,       height+y, 0 );
  vertices->add( x,       y,        0 );
  vertices->add( width+x, height+y, 0 );
  vertices->add( width+x, y,        0 );

  FloatBufferBuilderFromCartesian2D texCoords;
  texCoords.add( 0, 0 );
  texCoords.add( 0, 1 );
  texCoords.add( 1, 0 );
  texCoords.add( 1, 1 );

  DirectMesh* dm = new DirectMesh(GLPrimitive::triangleStrip(),
                                  true,
                                  vertices->getCenter(),
                                  vertices->create(),
                                  1,
                                  1);

  delete vertices;

//  _mtMapping = new MultiTextureMapping(texId,
//                                       texCoords.create(),
//                                       true,
//                                       true,
//                                       texId2,
//                                       texCoords.create(),
//                                       true,
//                                       true);

  _mtMapping = new StenciledMultiTextureMapping(texId,
                                       texCoords.create(),
                                       true,
                                       true,
                                       texId2,
                                       texCoords.create(),
                                       true,
                                       true,
                                       stencilTexId,
                                       texCoords.create(),
                                       true);

  //Transforms only for Texture 0
  _mtMapping->setTranslation(_texCoordsTranslationU,
                             _texCoordsTranslationV);

  _mtMapping->setScale(_texCoordsScaleU,
                       _texCoordsScaleV);

  _mtMapping->setRotation(_texCoordsRotationInRadians,
                          _texCoordsRotationCenterU,
                          _texCoordsRotationCenterV);


  return new TexturedMesh(dm, true, _mtMapping, true, true);
}

void StenciledMultiTexturedHUDQuadWidget::setTexCoordsTranslation(float u, float v) {
  _texCoordsTranslationU = u;
  _texCoordsTranslationV = v;

  if (_mtMapping != NULL) {
    _mtMapping->setTranslation(_texCoordsTranslationU,
                               _texCoordsTranslationV);
  }
}

void StenciledMultiTexturedHUDQuadWidget::setTexCoordsScale(float u, float v) {
  _texCoordsScaleU = u;
  _texCoordsScaleV = v;

  if (_mtMapping != NULL) {
    _mtMapping->setScale(_texCoordsScaleU,
                         _texCoordsScaleV);
  }
}

void StenciledMultiTexturedHUDQuadWidget::setTexCoordsRotation(float angleInRadians,
                                                      float centerU,
                                                      float centerV) {
  _texCoordsRotationInRadians = angleInRadians;
  _texCoordsRotationCenterU = centerU;
  _texCoordsRotationCenterV = centerV;

  if (_mtMapping != NULL) {
    _mtMapping->setRotation(_texCoordsRotationInRadians,
                            _texCoordsRotationCenterU,
                            _texCoordsRotationCenterV);
  }
}


void StenciledMultiTexturedHUDQuadWidget::initialize(const G3MContext* context) {
  if (!_downloadingImages && (_image == NULL)  && (_image2 == NULL)  && (_stencilImage == NULL)) {
    _downloadingImages = true;
    IDownloader* downloader = context->getDownloader();
    downloader->requestImage(_imageURL,
                             1000000, // priority
                             TimeInterval::fromDays(30),
                             true, // readExpired
                             new StenciledMultiTexturedHUDQuadWidget_ImageDownloadListener(this),
                             true);

    downloader->requestImage(_imageURL2,
                             1000000, // priority
                             TimeInterval::fromDays(30),
                             true, // readExpired
                             new StenciledMultiTexturedHUDQuadWidget_ImageDownloadListener(this),
                             true);

    downloader->requestImage(_stencilURL,
                             1000000, // priority
                             TimeInterval::fromDays(30),
                             true, // readExpired
                             new StenciledMultiTexturedHUDQuadWidget_ImageDownloadListener(this),
                             true);
  }
}

void StenciledMultiTexturedHUDQuadWidget::cleanMesh() {
#warning TO DIEGO : ONLY = NULL ??? -> check in all widgets
  _mtMapping = NULL;

  delete _mesh;
  _mesh = NULL;
}

void StenciledMultiTexturedHUDQuadWidget::onResizeViewportEvent(const G3MEventContext* ec,
                                                       int width,
                                                       int height) {
  cleanMesh();
}

void StenciledMultiTexturedHUDQuadWidget::onImageDownload(IImage* image, const URL& url) {

  if (url.isEquals(_imageURL)){
    _image = image;
  }
  if (url.isEquals(_imageURL2)){
    _image2 = image;
  }

  if (url.isEquals(_stencilURL)){
    _stencilImage = image;
  }

  if (_image != NULL && _image2 != NULL && _stencilImage != NULL){
    _downloadingImages = false;
  }
}

void StenciledMultiTexturedHUDQuadWidget::onImageDownloadError(const URL& url) {
  _errors.push_back("StenciledMultiTexturedHUDQuadWidget: Error downloading \"" + url.getPath() + "\"");
}

RenderState StenciledMultiTexturedHUDQuadWidget::getRenderState(const G3MRenderContext* rc) {
  if (!_errors.empty()) {
    return RenderState::error(_errors);
  }
  else if (_downloadingImages) {
    return RenderState::busy();
  }
  else {
    return RenderState::ready();
  }
}

Mesh* StenciledMultiTexturedHUDQuadWidget::getMesh(const G3MRenderContext* rc) {
  if (_mesh == NULL) {
    _mesh = createMesh(rc);
  }
  return _mesh;
}

void StenciledMultiTexturedHUDQuadWidget::rawRender(const G3MRenderContext* rc,
                                           GLState* glState) {
  Mesh* mesh = getMesh(rc);
  if (mesh != NULL) {
    mesh->render(rc, glState);
  }
}
//
//  SGLayerNode.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/9/12.
//
//

#include "SGLayerNode.hpp"

#include "IGLTextureId.hpp"
#include "GL.hpp"
#include "Context.hpp"
#include "IDownloader.hpp"
#include "SGShape.hpp"
#include "IImageDownloadListener.hpp"
#include "TexturesHandler.hpp"
#include "IStringBuilder.hpp"

#include "GPUProgramManager.hpp"
#include "GPUProgram.hpp"
#include "GPUProgramState.hpp"


#define TEXTURES_DOWNLOAD_PRIORITY 1000000


class SGLayerNode_ImageDownloadListener : public IImageDownloadListener {
private:
  SGLayerNode* _layerNode;

public:
  SGLayerNode_ImageDownloadListener(SGLayerNode* layerNode) :
  _layerNode(layerNode)
  {

  }

  void onDownload(const URL& url,
                  IImage* image,
                  bool expired) {
    _layerNode->onImageDownload(image);
  }

  void onError(const URL& url) {
    ILogger::instance()->logWarning("Can't download texture \"%s\"",
                                    url.getPath().c_str());
  }

  void onCancel(const URL& url) {

  }

  void onCanceledDownload(const URL& url,
                          IImage* image,
                          bool expired) {

  }
};

bool SGLayerNode::isReadyToRender(const G3MRenderContext* rc) {
  if (!_initialized) {
    _initialized = true;
    requestImage(rc);
  }

  const IGLTextureId* textureId = getTextureId(rc);
  return (textureId != NULL);
}

void SGLayerNode::onImageDownload(IImage* image) {
  if (_downloadedImage != NULL) {
    IFactory::instance()->deleteImage(_downloadedImage);
  }
  _downloadedImage = image;
}

URL SGLayerNode::getURL() const {
  IStringBuilder *isb = IStringBuilder::newStringBuilder();
  isb->addString(_shape->getURIPrefix());
  isb->addString(_uri);
  const std::string path = isb->getString();
  delete isb;

  return URL(path, false);
}

void SGLayerNode::requestImage(const G3MRenderContext* rc) {
  if (_uri.compare("") == 0) {
    return;
  }

  rc->getDownloader()->requestImage(getURL(),
                                    TEXTURES_DOWNLOAD_PRIORITY,
                                    TimeInterval::fromDays(30),
                                    true,
                                    new SGLayerNode_ImageDownloadListener(this),
                                    true);
}

const IGLTextureId* SGLayerNode::getTextureId(const G3MRenderContext* rc) {
  if (_textureId == NULL) {
    if (_downloadedImage != NULL) {
      const bool hasMipMap = false;
      _textureId = rc->getTexturesHandler()->getGLTextureId(_downloadedImage,
                                                            GLFormat::rgba(),
                                                            getURL().getPath(),
                                                            hasMipMap);

      IFactory::instance()->deleteImage(_downloadedImage);
      _downloadedImage = NULL;
    }
  }
  return _textureId;
}

GLState* SGLayerNode::createGLState(const G3MRenderContext* rc, GLState* parentGLState) {
  if (!_initialized) {
    _initialized = true;
    requestImage(rc);
  }

  const IGLTextureId* textureId = getTextureId(rc);
  if (textureId == NULL) {
    return NULL;
  }
  
  _glState.setParent(parentGLState);

  _glState.getGPUProgramState()->setUniformValue("EnableTexture", true);
  _glState.getGPUProgramState()->setAttributeEnabled("TextureCoord", true);
  //_glState.getGLGlobalState()->enableTexture2D();
  _glState.getGLGlobalState()->enableBlend();
  int __WORKING;

//  GL* gl = rc->getGL();
//  gl->bindTexture(textureId);
  _glState.getGLGlobalState()->bindTexture(textureId);

  return &_glState;
}

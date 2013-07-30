//
//  QuadShape.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 10/28/12.
//
//

#include "QuadShape.hpp"

#include "IGLTextureId.hpp"
#include "IImage.hpp"
#include "IFactory.hpp"
#include "TexturesHandler.hpp"
#include "FloatBufferBuilderFromCartesian3D.hpp"
#include "ShortBufferBuilder.hpp"
#include "FloatBufferBuilderFromCartesian2D.hpp"
#include "DirectMesh.hpp"
#include "GLConstants.hpp"
#include "TexturedMesh.hpp"
#include "IDownloader.hpp"
#include "IImageDownloadListener.hpp"
#include "Color.hpp"

const IGLTextureId* QuadShape::getTextureId(const G3MRenderContext* rc) {
  if (_textureImage == NULL) {
    return NULL;
  }

  const IGLTextureId* texId = rc->getTexturesHandler()->getGLTextureId(_textureImage,
                                                                       GLFormat::rgba(),
                                                                       _textureURL.getPath(),
                                                                       false);

  rc->getFactory()->deleteImage(_textureImage);
  _textureImage = NULL;

  if (texId == NULL) {
    rc->getLogger()->logError("Can't load texture %s", _textureURL.getPath().c_str());
  }

  return texId;
}

class QuadShape_IImageDownloadListener : public IImageDownloadListener {
private:
  QuadShape* _quadShape;

public:

  QuadShape_IImageDownloadListener(QuadShape* quadShape) :
  _quadShape(quadShape)
  {

  }

  void onDownload(const URL& url,
                  IImage* image,
                  bool expired)  {
    _quadShape->imageDownloaded(image);
  }

  void onError(const URL& url) {

  }

  void onCancel(const URL& url) {

  }

  void onCanceledDownload(const URL& url,
                          IImage* image,
                          bool expired)  {

  }
};

void QuadShape::imageDownloaded(IImage* image) {
  _textureImage = image;

  cleanMesh();
}

QuadShape::~QuadShape() {
  delete _color;
}

Mesh* QuadShape::createMesh(const G3MRenderContext* rc) {
  if (!_textureRequested) {
    _textureRequested = true;
    if (_textureURL.getPath().length() != 0) {
      rc->getDownloader()->requestImage(_textureURL,
                                        1000000,
                                        TimeInterval::fromDays(30),
                                        true,
                                        new QuadShape_IImageDownloadListener(this),
                                        true);
    }
  }

  const float halfWidth  = _width / 2.0f;
  const float halfHeight = _height / 2.0f;

  const float left   = -halfWidth;
  const float right  = +halfWidth;
  const float bottom = -halfHeight;
  const float top    = +halfHeight;

//  FloatBufferBuilderFromCartesian3D vertices(CenterStrategy::noCenter(), Vector3D::zero);
  FloatBufferBuilderFromCartesian3D vertices = FloatBufferBuilderFromCartesian3D::builderWithoutCenter();
  vertices.add(left,  bottom, 0);
  vertices.add(right, bottom, 0);
  vertices.add(left,  top,    0);
  vertices.add(right, top,    0);

//  const Vector3D center = Vector3D::zero;

  Color* color = (_color == NULL) ? NULL : new Color(*_color);

  Mesh* im = new DirectMesh(GLPrimitive::triangleStrip(),
                            true,
                            vertices.getCenter(),
                            vertices.create(),
                            1,
                            1,
                            color);

  const IGLTextureId* texId = getTextureId(rc);
  if (texId == NULL) {
    return im;
  }

  FloatBufferBuilderFromCartesian2D texCoords;
  texCoords.add(0, 1);
  texCoords.add(1, 1);
  texCoords.add(0, 0);
  texCoords.add(1, 0);

  TextureMapping* texMap = new SimpleTextureMapping(texId,
                                                    texCoords.create(),
                                                    true,
                                                    true);

  return new TexturedMesh(im, true, texMap, true, true);
}

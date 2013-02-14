//
//  EllipsoidShape.cpp
//  G3MiOSSDK
//
//  Created by Agustin Trujillo Pino on 02/13/13.
//
//

#include <math.h>

#include "EllipsoidShape.hpp"

#include "ShortBufferBuilder.hpp"
#include "IndexedMesh.hpp"
#include "GLConstants.hpp"
#include "CompositeMesh.hpp"
#include "Color.hpp"
#include "FloatBufferBuilderFromCartesian3D.hpp"
#include "FloatBufferBuilderFromCartesian2D.hpp"
#include "IDownloader.hpp"
#include "IImageDownloadListener.hpp"
#include "TexturesHandler.hpp"
#include "TexturedMesh.hpp"

EllipsoidShape::~EllipsoidShape() {
  delete _surfaceColor;
  delete _borderColor;
}

const IGLTextureId* EllipsoidShape::getTextureId(const G3MRenderContext* rc) {
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


Mesh* EllipsoidShape::createBorderMesh(const G3MRenderContext* rc,
                                       FloatBufferBuilderFromCartesian3D *vertices) {

  // create border indices for horizontal lines
  ShortBufferBuilder indices;
  short delta = (short) (2*_resolution - 1);
  for (short j=1; j<_resolution-1; j++) {
    for (short i=0; i<2*_resolution-2; i++) {
      indices.add((short) (i+j*delta));
      indices.add((short) (i+1+j*delta));
    }
  }

  // create border indices for vertical lines
  for (short i=0; i<2*_resolution-2; i++) {
    for (short j=0; j<_resolution-1; j++) {
      indices.add((short) (i+j*delta));
      indices.add((short) (i+(j+1)*delta));
    }
  }

  Color* borderColor;
  if (_borderColor != NULL) {
    borderColor = new Color(*_borderColor);
  }
  else {
    if (_surfaceColor != NULL) {
      borderColor = new Color(*_surfaceColor);
    }
    else {
      borderColor = Color::newFromRGBA(1, 1, 1, 1);
    }
  }

  return new IndexedMesh(GLPrimitive::lines(),
                         true,
                         vertices->getCenter(),
                         vertices->create(),
                         indices.create(),
                         _borderWidth,
                         1,
                         borderColor);
}

Mesh* EllipsoidShape::createSurfaceMesh(const G3MRenderContext* rc,
                                        FloatBufferBuilderFromCartesian3D* vertices,
                                        FloatBufferBuilderFromCartesian2D* texCoords) {

  // create surface indices
  ShortBufferBuilder indices;
  short delta = (short) (2*_resolution - 1);
  for (short j=0; j<_resolution-1; j++) {
    if (j>0) indices.add((short) (j*delta));
    for (short i=0; i<2*_resolution-1; i++) {
      indices.add((short) (i+j*delta));
      indices.add((short) (i+(j+1)*delta));
    }
    indices.add((short) ((j+2)*delta-1));
  }

  Color* surfaceColor = (_surfaceColor == NULL) ? NULL : new Color(*_surfaceColor);

  Mesh* im = new IndexedMesh(GLPrimitive::triangleStrip(),
                             true,
                             vertices->getCenter(),
                             vertices->create(),
                             indices.create(),
                             _borderWidth,
                             1,
                             surfaceColor);

  const IGLTextureId* texId = getTextureId(rc);
  if (texId == NULL) {
    return im;
  }

  TextureMapping* texMap = new SimpleTextureMapping(texId,
                                                    texCoords->create(),
                                                    true,
                                                    true);

  return new TexturedMesh(im, true, texMap, true, true);

}

class EllipsoidShape_IImageDownloadListener : public IImageDownloadListener {
private:
  EllipsoidShape* _ellipsoidShape;

public:

  EllipsoidShape_IImageDownloadListener(EllipsoidShape* ellipsoidShape) :
  _ellipsoidShape(ellipsoidShape)
  {

  }

  void onDownload(const URL& url,
                  IImage* image)  {
    _ellipsoidShape->imageDownloaded(image);
  }

  void onError(const URL& url) {

  }

  void onCancel(const URL& url) {

  }

  void onCanceledDownload(const URL& url,
                          IImage* image)  {
    
  }
};

void EllipsoidShape::imageDownloaded(IImage* image) {
  _textureImage = image;

  cleanMesh();
}


Mesh* EllipsoidShape::createMesh(const G3MRenderContext* rc) {
  if (!_textureRequested) {
    _textureRequested = true;
    if (_textureURL.getPath().length() != 0) {
      rc->getDownloader()->requestImage(_textureURL,
                                        1000000,
                                        TimeInterval::fromDays(30),
                                        new EllipsoidShape_IImageDownloadListener(this),
                                        true);
    }
  }

  FloatBufferBuilderFromCartesian3D vertices(CenterStrategy::noCenter(), Vector3D::zero());
  FloatBufferBuilderFromCartesian2D texCoords;

  const double pi = IMathUtils::instance()->pi();
  const double incAngle = pi/(_resolution-1);
  for (int j=0; j<_resolution; j++) {
    const double lat = pi/2 - j*incAngle;
    const double s = sin(lat);
    const double c = cos(lat);
    const double z = _radiusZ * s;
    for (int i=0; i<2*_resolution-1; i++) {
      const double lon = -pi + i*incAngle;
      const double x = _radiusX * c * cos(lon);
      const double y = _radiusY * c * sin(lon);
      vertices.add(x, y, z);

      const float u = (float) i / (2*_resolution-2);
      const float v = (_cozzi)? (float)(1-s)/2 : (float)j/(_resolution-1);
      texCoords.add(u, v);
    }
  }

  Mesh* surfaceMesh = createSurfaceMesh(rc, &vertices, &texCoords);

  if (_borderWidth > 0) {
    CompositeMesh* compositeMesh = new CompositeMesh();
    compositeMesh->addMesh(surfaceMesh);
    compositeMesh->addMesh(createBorderMesh(rc, &vertices));
    return compositeMesh;
  }
  
  return surfaceMesh;
}

//
//  EllipsoidShape.hpp
//  G3MiOSSDK
//
//  Created by Agustin Trujillo Pino on 02/13/13.
//
//

#ifndef __G3MiOSSDK__EllipsoidShape__
#define __G3MiOSSDK__EllipsoidShape__

#include "AbstractMeshShape.hpp"
#include "Ellipsoid.hpp"
#include "Planet.hpp"


class Color;
class FloatBufferBuilderFromGeodetic;
class FloatBufferBuilderFromCartesian2D;
class FloatBufferBuilderFromCartesian3D;

class IGLTextureId;

#include "URL.hpp"


class EllipsoidShape : public AbstractMeshShape {
private:
  
#ifdef C_CODE
  const Ellipsoid _ellipsoid;
#endif
#ifdef JAVA_CODE
  private final Ellipsoid _ellipsoid;
#endif

  URL _textureURL;

  /*const double _radiusX;
  const double _radiusY;
  const double _radiusZ;*/

  const short _resolution;

  const float _borderWidth;
  
  const bool _texturedInside;

  const bool _mercator;

  const bool _withNormals;

  Color* _surfaceColor;
  Color* _borderColor;

  Mesh* createBorderMesh(const G3MRenderContext* rc,
                         FloatBufferBuilderFromGeodetic *vertices);
  Mesh* createSurfaceMesh(const G3MRenderContext* rc,
                          FloatBufferBuilderFromGeodetic* vertices,
                          FloatBufferBuilderFromCartesian2D* texCoords,
                          FloatBufferBuilderFromCartesian3D* normals);

  bool _textureRequested;
  IImage* _textureImage;
  const IGLTextureId* getTextureId(const G3MRenderContext* rc);

protected:
  Mesh* createMesh(const G3MRenderContext* rc);

public:
  EllipsoidShape(Geodetic3D* position,
                 AltitudeMode altitudeMode,
                 const Planet* planet,
                 const Vector3D& radius,
                 short resolution,
                 float borderWidth,
                 bool texturedInside,
                 bool mercator,
                 const Color& surfaceColor,
                 Color* borderColor = NULL,
                 bool withNormals = true) :
  _ellipsoid(planet->toCartesian(*position), radius),
  AbstractMeshShape(position, altitudeMode),
  _textureURL(URL("", false)),
  _resolution(resolution < 3 ? 3 : resolution),
  _borderWidth(borderWidth),
  _texturedInside(texturedInside),
  _mercator(mercator),
  _surfaceColor(new Color(surfaceColor)),
  _borderColor(borderColor),
  _textureRequested(false),
  _textureImage(NULL),
  _withNormals(withNormals)
  {

  }

  EllipsoidShape(Geodetic3D* position,
                 AltitudeMode altitudeMode,
                 const Planet* planet,
                 const URL& textureURL,
                 const Vector3D& radius,
                 short resolution,
                 float borderWidth,
                 bool texturedInside,
                 bool mercator,
                 bool withNormals = true) :
  _ellipsoid(planet->toCartesian(*position), radius),
  AbstractMeshShape(position, altitudeMode),
  _textureURL(textureURL),
  _resolution(resolution < 3 ? 3 : resolution),
  _borderWidth(borderWidth),
  _texturedInside(texturedInside),
  _mercator(mercator),
  _surfaceColor(NULL),
  _borderColor(NULL),
  _textureRequested(false),
  _textureImage(NULL),
  _withNormals(withNormals)
  {
    
  }


  ~EllipsoidShape();

  void imageDownloaded(IImage* image);
  
  std::vector<double> intersectionsDistances(const Vector3D& origin,
                                             const Vector3D& direction) const {
    std::vector<double> intersections;
    return intersections;
  }

};

#endif

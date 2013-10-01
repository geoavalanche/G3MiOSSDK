//
//  BoxShape.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/16/12.
//
//

#ifndef __G3MiOSSDK__BoxShape__
#define __G3MiOSSDK__BoxShape__

#include "AbstractMeshShape.hpp"
#include "Color.hpp"
#include "Planet.hpp"


class BoxShape : public AbstractMeshShape {
private:
  double _extentX;
  double _extentY;
  double _extentZ;
  
  const Planet* _planet;

  float _borderWidth;

  bool _useNormals;

  Color* _surfaceColor;
  Color* _borderColor;

  Mesh* createBorderMesh(const G3MRenderContext* rc);
  Mesh* createSurfaceMesh(const G3MRenderContext* rc);

  Mesh* createSurfaceMeshWithNormals(const G3MRenderContext* rc);

protected:
  Mesh* createMesh(const G3MRenderContext* rc);

public:
  BoxShape(Geodetic3D* position,
           AltitudeMode altitudeMode,
           const Planet* planet,
           const Vector3D& extent,
           float borderWidth,
           const Color& surfaceColor,
           Color* borderColor = NULL,
           bool useNormals = true) :
  AbstractMeshShape(position, altitudeMode),
  _extentX(extent._x),
  _extentY(extent._y),
  _extentZ(extent._z),
  _borderWidth(borderWidth),
  _surfaceColor(new Color(surfaceColor)),
  _borderColor(borderColor),
  _useNormals(useNormals),
  _planet(planet)
  {

  }

  ~BoxShape() {
    delete _surfaceColor;
    delete _borderColor;
    
#ifdef JAVA_CODE
  super.dispose();
#endif

  }

  void setExtent(const Vector3D& extent) {
    if ((_extentX != extent._x) ||
        (_extentY != extent._y) ||
        (_extentZ != extent._z)) {
      _extentX = extent._x;
      _extentY = extent._y;
      _extentZ = extent._z;
      cleanMesh();
    }
  }

  Vector3D getExtent() const {
    return Vector3D(_extentX, _extentY, _extentZ);
  }

  void setSurfaceColor(Color* color) {
    delete _surfaceColor;
    _surfaceColor = color;
    cleanMesh();
  }

  void setBorderColor(Color* color) {
    delete _borderColor;
    _borderColor = color;
    cleanMesh();
  }

  void setBorderWidth(float borderWidth) {
    if (_borderWidth != borderWidth) {
      _borderWidth = borderWidth;
      cleanMesh();
    }
  }
  
  std::vector<double> intersectionsDistances(const Vector3D& origin,
                                             const Vector3D& direction) const {
    std::vector<double> intersections;
    return intersections;
  }

};

#endif

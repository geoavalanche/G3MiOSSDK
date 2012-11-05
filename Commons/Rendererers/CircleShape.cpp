//
//  CircleShape.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/5/12.
//
//

#include "CircleShape.hpp"

#include "FloatBufferBuilderFromCartesian3D.hpp"
#include "IntBufferBuilder.hpp"
#include "IndexedMesh.hpp"
#include "GLConstants.hpp"
#include "IMathUtils.hpp"

Mesh* CircleShape::createMesh(const RenderContext* rc) {

  FloatBufferBuilderFromCartesian3D vertices(CenterStrategy::noCenter(), Vector3D::zero());

  IntBufferBuilder indices;

  // first is the center
  vertices.add(0.0, 0.0, 0.0);
  indices.add(0);

  
  const double twicePi = GMath.pi() * 2;

  for (int i = 0; i <= _steps; i++) {
    const double angle = i * twicePi / _steps;
    const double x = _radius * GMath.cos(angle);
    const double y = _radius * GMath.sin(angle);
    vertices.add(x, y, 0);

    indices.add(i + 1);
  }

  IndexedMesh* mesh = new IndexedMesh(GLPrimitive::triangleFan(),
                                      true,
                                      Vector3D::zero(),
                                      vertices.create(),
                                      indices.create(),
                                      1,
                                      new Color(_color));
  
  return mesh;
}

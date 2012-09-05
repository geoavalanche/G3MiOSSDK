//
//  EllipsoidalTileTessellator.cpp
//  G3MiOSSDK
//
//  Created by Agustin Trujillo Pino on 12/07/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

#include "EllipsoidalTileTessellator.hpp"

#include "Tile.hpp"
#include "Context.hpp"
#include "IndexedMesh.hpp"
#include "TextureMapping.hpp"
#include "TexturedMesh.hpp"

#include "FloatBufferBuilder.hpp"
#include "IntBufferBuilder.hpp"

Mesh* EllipsoidalTileTessellator::createMesh(const RenderContext* rc,
                                             const Tile* tile) const {
  
  const Sector sector = tile->getSector();
  const Planet* planet = rc->getPlanet();
  
  const int resolution = _resolution;
  const int resolutionMinus1 = resolution - 1;
  
  // create vertices coordinates
  FloatBufferBuilder vertices;
  for (int j = 0; j < resolution; j++) {
    for (int i = 0; i < resolution; i++) {
      vertices.add(planet->toCartesian(sector.getInnerPoint((double) i / resolutionMinus1,
                                                            (double) j / resolutionMinus1)));
      
    }
  }
  
  // create indexes
  IntBufferBuilder indexes;
  for (int j = 0; j < resolutionMinus1; j++) {
    if (j > 0) {
      indexes.add(j*resolution);
    }
    for (int i = 0; i < resolution; i++) {
      indexes.add(j*resolution + i);
      indexes.add(j*resolution + i + resolution);
    }
    indexes.add(j*resolution + 2*resolutionMinus1);
  }
  
  // create skirts
  if (_skirted) {
    
    // compute skirt height
    const Vector3D sw = planet->toCartesian(sector.getSW());
    const Vector3D nw = planet->toCartesian(sector.getNW());
    const double skirtHeight = nw.sub(sw).length() * 0.05;
    
    indexes.add(0);
    int posS = resolution * resolution;
    
    // west side
    for (int j = 0; j < resolutionMinus1; j++) {
      const Geodetic3D g(sector.getInnerPoint(0, (double)j/resolutionMinus1),
                         -skirtHeight);
      vertices.add( planet->toCartesian(g) );
      
      indexes.add(j*resolution);
      indexes.add(posS++);
    }
    
    // south side
    for (int i = 0; i < resolutionMinus1; i++) {
      const Geodetic3D g(sector.getInnerPoint((double)i/resolutionMinus1, 1),
                         -skirtHeight);
      vertices.add( planet->toCartesian(g) );
      
      indexes.add(resolutionMinus1*resolution + i);
      indexes.add(posS++);
    }
    
    // east side
    for (int j = resolutionMinus1; j > 0; j--) {
      const Geodetic3D g(sector.getInnerPoint(1, (double)j/resolutionMinus1),
                         -skirtHeight);
      vertices.add( planet->toCartesian(g) );
      
      indexes.add(j*resolution + resolutionMinus1);
      indexes.add(posS++);
    }
    
    // north side
    for (int i = resolutionMinus1; i > 0; i--) {
      const Geodetic3D g(sector.getInnerPoint((double)i/resolutionMinus1, 0),
                         -skirtHeight);
      vertices.add( planet->toCartesian(g) );
      
      indexes.add(i);
      indexes.add(posS++);
    }
    
    // last triangle
    indexes.add(0);
    indexes.add(resolution*resolution);
  }
  
  const Color *color = new Color(Color::fromRGBA((float) 0.1, (float) 0.1, (float) 0.1, (float) 1.0));
  const Vector3D center = planet->toCartesian( sector.getCenter() );

#ifdef C_CODE
  return new IndexedMesh(TriangleStrip,
                         true,
                         GivenCenter,
                         center,
                         vertices.create(),
                         indexes.create(),
                         color);
#endif
#ifdef JAVA_CODE
  return new IndexedMesh(GLPrimitive.TriangleStrip,
                         true,
                         CenterStrategy.GivenCenter,
                         center,
                         vertices.create(),
                         indexes.create(),
                         color);
#endif
}


std::vector<MutableVector2D>* EllipsoidalTileTessellator::createUnitTextCoords() const {
  
  std::vector<MutableVector2D>* textCoords = new std::vector<MutableVector2D>();
  
  const int resolution       = _resolution;
  const int resolutionMinus1 = resolution - 1;
  
  float* u = new float[resolution * resolution];
  float* v = new float[resolution * resolution];
  
  for (int j = 0; j < resolution; j++) {
    for (int i = 0; i < resolution; i++) {
      const int pos = j*resolution + i;
      u[pos] = (float) i / resolutionMinus1;
      v[pos] = (float) j / resolutionMinus1;
    }
  }
  
  for (int j = 0; j < resolution; j++) {
    for (int i = 0; i < resolution; i++) {
      const int pos = j*resolution + i;
      textCoords->push_back(MutableVector2D(u[pos], v[pos]));
    }
  }
  
  // create skirts
  if (_skirted) {
    // west side
    for (int j = 0; j < resolutionMinus1; j++) {
      const int pos = j*resolution;
      textCoords->push_back(MutableVector2D(u[pos], v[pos]));
    }
    
    // south side
    for (int i = 0; i < resolutionMinus1; i++) {
      const int pos = resolutionMinus1 * resolution + i;
      textCoords->push_back(MutableVector2D(u[pos], v[pos]));
    }
    
    // east side
    for (int j = resolutionMinus1; j > 0; j--) {
      const int pos = j*resolution + resolutionMinus1;
      textCoords->push_back(MutableVector2D(u[pos], v[pos]));
    }
    
    // north side
    for (int i = resolutionMinus1; i > 0; i--) {
      const int pos = i;
      textCoords->push_back(MutableVector2D(u[pos], v[pos]));
    }
  }
  
  // free temp memory
  delete[] u;
  delete[] v;
  
  return textCoords;
}


Mesh* EllipsoidalTileTessellator::createDebugMesh(const RenderContext* rc,
                                                  const Tile* tile) const
{
  const Sector sector = tile->getSector();
  const Planet* planet = rc->getPlanet();
  
  // create vectors
  std::vector<MutableVector3D> vertices;
  std::vector<MutableVector2D> texCoords;
  std::vector<int> indexes;
  const int resolutionMinus1 = _resolution - 1;
  int posS = 0;
  
  // compute offset for vertices
  const Vector3D sw = planet->toCartesian(sector.getSW());
  const Vector3D nw = planet->toCartesian(sector.getNW());
  const double offset = nw.sub(sw).length() * 1e-3;
  
  // west side
  for (int j = 0; j < resolutionMinus1; j++) {
    const Geodetic3D g(sector.getInnerPoint(0, (double)j/resolutionMinus1), offset);
    addVertex(planet, &vertices, g);
    indexes.push_back(posS++);
  }
  
  // south side
  for (int i = 0; i < resolutionMinus1; i++) {
    const Geodetic3D g(sector.getInnerPoint((double)i/resolutionMinus1, 1), offset);
    addVertex(planet, &vertices, g);
    indexes.push_back(posS++);
  }
  
  // east side
  for (int j = resolutionMinus1; j > 0; j--) {
    const Geodetic3D g(sector.getInnerPoint(1, (double)j/resolutionMinus1), offset);
    addVertex(planet, &vertices, g);
    indexes.push_back(posS++);
  }
  
  // north side
  for (int i = resolutionMinus1; i > 0; i--) {
    const Geodetic3D g(sector.getInnerPoint((double)i/resolutionMinus1, 0), offset);
    addVertex(planet, &vertices, g);
    indexes.push_back(posS++);
  }
  
  const Color *color = new Color(Color::fromRGBA((float) 1.0, (float) 0, (float) 0, (float) 1.0));
  const Vector3D center = planet->toCartesian(sector.getCenter());
  
#ifdef C_CODE
  return IndexedMesh::createFromVector3D(vertices, LineLoop, GivenCenter, center, indexes, color);
#else
  return IndexedMesh::createFromVector3D(vertices, GLPrimitive.LineLoop, GivenCenter, center, indexes, color);
#endif
}

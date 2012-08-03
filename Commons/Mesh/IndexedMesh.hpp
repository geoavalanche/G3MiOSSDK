//
//  IndexedMesh.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 22/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef G3MiOSSDK_IndexedMesh_h
#define G3MiOSSDK_IndexedMesh_h

#include "Mesh.hpp"
#include "Color.hpp"
#include "MutableVector2D.hpp"
#include "MutableVector3D.hpp"

#include <vector>


enum GLPrimitive {
  TriangleStrip,
  Lines,
  LineLoop
};

enum CenterStrategy {
  NoCenter,
  AveragedVertex,
  FirstVertex,
  GivenCenter
};


class IndexedMesh : public Mesh {
private:
  IndexedMesh(std::vector<MutableVector3D>& vertices,
              const GLPrimitive primitive,
              CenterStrategy strategy,
              Vector3D center,
              std::vector<unsigned int>& indexes,
              const Color* flatColor = NULL,
              std::vector<Color>* colors = NULL,
              const float colorsIntensity = 0.0,
              std::vector<MutableVector3D>* normals = NULL);
  
  IndexedMesh(bool owner,
              const GLPrimitive primitive,
              CenterStrategy strategy,
              Vector3D center,
              const unsigned int numVertices,
              const float* vertices,
              const unsigned int* indexes,
              const int numIndex, 
              const Color* flatColor = NULL,
              const float * colors = NULL,
              const float colorsIntensity = 0.0,
              const float* normals = NULL);


  const bool           _owner;
  
  const GLPrimitive    _primitive; 
  
  const float*         _vertices;
  const unsigned int   _numVertices;
  
  const unsigned int*  _indexes;
  const int            _numIndex;
  
  const float*         _normals;
  
  const Color *        _flatColor;
  const float *        _colors;
  const float          _colorsIntensity;
  
  mutable Extent *     _extent;
  
  Extent* computeExtent() const;
  
  CenterStrategy       _centerStrategy;
  Vector3D             _center;
  
public:
  
  ~IndexedMesh();

    
  static IndexedMesh* CreateFromVector3D(bool owner,
                                         const GLPrimitive primitive,
                                         CenterStrategy strategy,
                                         Vector3D center,
                                         const unsigned int numVertices,
                                         const float* vertices,
                                         const unsigned int* indexes,
                                         const int numIndex, 
                                         const Color* flatColor = NULL,
                                         const float * colors = NULL,
                                         const float colorsIntensity = 0.0,
                                         const float* normals = NULL)
  {
    return new IndexedMesh(owner, primitive, strategy, center, numVertices, vertices,
                           indexes, numIndex, flatColor, colors, colorsIntensity, normals);
  }

    
  static IndexedMesh* CreateFromVector3D(std::vector<MutableVector3D>& vertices,
                                         const GLPrimitive primitive,
                                         CenterStrategy strategy,
                                         Vector3D center,
                                         std::vector<unsigned int>& indexes,
                                         const Color* flatColor = NULL,
                                         std::vector<Color>* colors = NULL,
                                         const float colorsIntensity = 0.0,
                                         std::vector<MutableVector3D>* normals = NULL)
  {
    return new IndexedMesh(vertices, primitive, strategy, center, indexes,
                           flatColor, colors, colorsIntensity, normals);
  }

                                         
    
  virtual void render(const RenderContext* rc) const;
  
  Extent *getExtent() const;
  
  int getVertexCount() const {
    return _numVertices;
  }
  
  const Vector3D getVertex(int i) const {
    const int p = i * 3;
    return Vector3D(_vertices[p  ] + _center.x(),
                    _vertices[p+1] + _center.y(),
                    _vertices[p+2] + _center.z());
  }

  
};

#endif

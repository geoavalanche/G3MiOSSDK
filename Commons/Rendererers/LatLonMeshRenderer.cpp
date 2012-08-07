//
//  LatLonMeshRenderer.cpp
//  G3MiOSSDK
//
//  Created by Agustín Trujillo Pino on 02/08/12.
//  Copyright (c) 2012 Universidad de Las Palmas. All rights reserved.
//

#include <iostream>

#include "LatLonMeshRenderer.h"


LatLonMeshRenderer::~LatLonMeshRenderer()
{
  delete mesh;
}


void LatLonMeshRenderer::initialize(const InitializationContext* ic)
{
  unsigned int numVertices = 4;
  int numIndices = 4;

  float v[] = {
    (float) 28.753213, (float) -17.898788, 500,
    (float) 28.680347, (float) -17.898788, 500,
    (float) 28.753213, (float) -17.83287,  500,
    (float) 28.680347, (float) -17.83287,  500
  };
  
  int i[] = { 0, 1, 2, 3};
  
  // create vertices and indices in dinamic memory
  float *vertices = new float [numVertices*3];
  memcpy(vertices, v, numVertices*3*sizeof(float));
  int *indices = new int [numIndices];
  memcpy(indices, i, numIndices*sizeof(unsigned int));

  // create mesh
  Color *flatColor = new Color(Color::fromRGBA(1.0, 1.0, 0.0, 1.0));
  mesh = IndexedMesh::CreateFromGeodetic3D(ic->getPlanet(), true, TriangleStrip, NoCenter, Vector3D(0,0,0), 
                                           4, vertices, indices, 4, flatColor);
}  


int LatLonMeshRenderer::render(const RenderContext* rc)
{  
  mesh->render(rc);

  return Renderer::maxTimeToRender;
}
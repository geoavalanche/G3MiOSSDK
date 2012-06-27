//
//  SimplePlanetRenderer.cpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 12/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "SimplePlanetRenderer.hpp"

SimplePlanetRenderer::SimplePlanetRenderer(const std::string textureFilename):
_latRes(16),//FOR NOW THEY MUST BE EQUAL
_lonRes(16),
_textureFilename(textureFilename),
_mesh(NULL)
{
}

SimplePlanetRenderer::~SimplePlanetRenderer()
{
  delete _mesh;
}

void SimplePlanetRenderer::initialize(const InitializationContext* ic)
{

}

float * SimplePlanetRenderer::createVertices(const Planet& planet)
{
  //VERTICES
  float* vertices = new float[_latRes *_lonRes * 3];
  
  const double lonRes1 = (double) (_lonRes-1);
  const double latRes1 = (double) (_latRes-1);
  int verticesIndex = 0;
  for(double i = 0.0; i < _lonRes; i++){
    const Angle lon = Angle::fromDegrees( (i * 360 / lonRes1) -180);
    for (double j = 0.0; j < _latRes; j++) {
      const Angle lat = Angle::fromDegrees( (j * 180.0 / latRes1)  -90.0 );
      const Geodetic2D g(lat, lon);
      
      const Vector3D v = planet.toVector3D(g);
      vertices[verticesIndex++] = (float) v.x();//Vertices
      vertices[verticesIndex++] = (float) v.y();
      vertices[verticesIndex++] = (float) v.z();
    }
  }
  
  return vertices;
}



unsigned char* SimplePlanetRenderer::createMeshIndex()
{
  const int res = _lonRes;
  
  const int numIndexes = 2 * (res - 1) * (res + 1);
  unsigned char *indexes = new unsigned char[numIndexes];
  
  int n = 0;
  for (int j = 0; j < res - 1; j++) {
    if (j > 0) indexes[n++] = (char) (j * res);
    for (int i = 0; i < res; i++) {
      indexes[n++] = (char) (j * res + i);
      indexes[n++] = (char) (j * res + i + res);
    }
    indexes[n++] = (char) (j * res + 2 * res - 1);
  }
  
  return indexes;
}

float* SimplePlanetRenderer::createTextureCoordinates()
{
  float* texCoords = new float[_latRes *_lonRes * 2];
  
  const double lonRes1 = (double) (_lonRes-1), latRes1 = (double) (_latRes-1);
  int p = 0;
  for(double i = 0.0; i < _lonRes; i++){
    double u = (i / lonRes1);
    for (double j = 0.0; j < _latRes; j++) {
      const double v = 1.0 - (j / latRes1);
      texCoords[p++] = (float) u;
      texCoords[p++] = (float) v;
    }
  }
  
  return texCoords;
}

bool SimplePlanetRenderer::initializeMesh(const RenderContext* rc) {
  const int texID = rc->getTexturesHandler()->getTextureIdFromFileName(rc, _textureFilename, 2048, 1024);
  
  if (texID < 1) {
    rc->getLogger()->logError("Can't load file %s", _textureFilename.c_str());
    return false;
  }
  
  const Planet* planet = rc->getPlanet();
  
  float* ver = createVertices(*planet);
  
  const int res = _lonRes;
  const int numIndexes = 2 * (res - 1) * (res + 1);
  unsigned char * ind = createMeshIndex();
  
  float * texC = createTextureCoordinates();
  
  _mesh = new IndexedTriangleStripMesh(true, ver, ind, numIndexes, texID, texC);
  
  return true;
}

int SimplePlanetRenderer::render(const RenderContext* rc){
  if (_mesh == NULL){
    if (!initializeMesh(rc)) {
      return MAX_TIME_TO_RENDER;
    }
  }
  
  _mesh->render(rc);
  
  return MAX_TIME_TO_RENDER;
}

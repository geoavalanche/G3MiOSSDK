//
//  GLFeature.cpp
//  G3MiOSSDK
//
//  Created by Jose Miguel SN on 18/03/13.
//
//

#include "GLFeature.hpp"
#include "Camera.hpp"

ViewportExtentGLFeature::ViewportExtentGLFeature(int viewportWidth,
                                                 int viewportHeight) :
GLFeature(NO_GROUP, GLF_VIEWPORT_EXTENT)
{
  _values.addUniformValue(VIEWPORT_EXTENT,
                          new GPUUniformValueVec2Float(viewportWidth, viewportHeight),
                          false);
}

BillboardGLFeature::BillboardGLFeature(const Vector3D& position,
                                       int textureWidth,
                                       int textureHeight) :
GLFeature(NO_GROUP, GLF_BILLBOARD)
{
  _values.addUniformValue(TEXTURE_EXTENT,
                          new GPUUniformValueVec2Float(textureWidth, textureHeight),
                          false);

  _values.addUniformValue(BILLBOARD_POSITION,
                          new GPUUniformValueVec4Float((float) position._x,
                                                       (float) position._y,
                                                       (float) position._z,
                                                       1),
                          false);
}

void BillboardGLFeature::applyOnGlobalGLState(GLGlobalState* state)  const {
  state->disableDepthTest();
  state->disableCullFace();
  state->disPolygonOffsetFill();
}

GeometryGLFeature::GeometryGLFeature(IFloatBuffer* buffer,
                                     int arrayElementSize,
                                     int index,
                                     bool normalized,
                                     int stride,
                                     bool depthTestEnabled,
                                     bool cullFace, int culledFace,
                                     bool polygonOffsetFill,
                                     float polygonOffsetFactor,
                                     float polygonOffsetUnits,
                                     float lineWidth,
                                     bool needsPointSize,
                                     float pointSize) :
GLFeature(NO_GROUP, GLF_GEOMETRY),
_depthTestEnabled(depthTestEnabled),
_cullFace(cullFace),
_culledFace(culledFace),
_polygonOffsetFill(polygonOffsetFill),
_polygonOffsetFactor(polygonOffsetFactor),
_polygonOffsetUnits(polygonOffsetUnits),
_lineWidth(lineWidth)
{

  _position = new GPUAttributeValueVec4Float(buffer, arrayElementSize, index, stride, normalized);
  _values.addAttributeValue(POSITION, _position, false);

  if (needsPointSize) {
    _values.addUniformValue(POINT_SIZE, new GPUUniformValueFloat(pointSize), false);
  }
}

void GeometryGLFeature::applyOnGlobalGLState(GLGlobalState* state) const{
  if (_depthTestEnabled) {
    state->enableDepthTest();
  }
  else {
    state->disableDepthTest();
  }

  if (_cullFace) {
    state->enableCullFace(_culledFace);
  }
  else {
    state->disableCullFace();
  }

  if (_polygonOffsetFill) {
    state->enablePolygonOffsetFill(_polygonOffsetFactor, _polygonOffsetUnits);
  }
  else {
    state->disPolygonOffsetFill();
  }

  state->setLineWidth(_lineWidth);
}


GeometryGLFeature::~GeometryGLFeature() {
  //  _position->_release();

#ifdef JAVA_CODE
  super.dispose();
#endif
}

void TextureGLFeature::createBasicValues(IFloatBuffer* texCoords,
                                         int arrayElementSize,
                                         int index,
                                         bool normalized,
                                         int stride){
  GPUAttributeValueVec2Float* value = new GPUAttributeValueVec2Float(texCoords,
                                                                     arrayElementSize,
                                                                     index,
                                                                     stride,
                                                                     normalized);

  GPUUniformValueInt* texUnit = new GPUUniformValueInt(_target);

  switch (_target) {
    case 0:
    _values.addUniformValue(SAMPLER, texUnit, false);
    _values.addAttributeValue(TEXTURE_COORDS, value, false);
    break;

    case 1:
    _values.addUniformValue(SAMPLER2, texUnit, false);
    _values.addAttributeValue(TEXTURE_COORDS_2, value, false);
    break;

    default:
    ILogger::instance()->logError("Wrong texture target.");

    break;
  }
}


TextureGLFeature::TextureGLFeature(const IGLTextureId* texID,
                                   IFloatBuffer* texCoords,
                                   int arrayElementSize,
                                   int index,
                                   bool normalized,
                                   int stride,
                                   bool blend,
                                   int sFactor,
                                   int dFactor,
                                   float translateU,
                                   float translateV,
                                   float scaleU,
                                   float scaleV,
                                   float rotationAngleInRadians,
                                   float rotationCenterU,
                                   float rotationCenterV,
                                   int target) :
GLColorGroupFeature(GLF_TEXTURE, 4, blend, sFactor, dFactor),
_texID(texID),
_target(target)
{

  createBasicValues(texCoords, arrayElementSize, index, normalized, stride);

#warning TRANFORMATIONS IF TARGET != 0
  _values.addUniformValue(TRANSLATION_TEXTURE_COORDS,
                          new GPUUniformValueVec2Float(translateU, translateV),
                          false);

  _values.addUniformValue(SCALE_TEXTURE_COORDS,
                          new GPUUniformValueVec2Float(scaleU, scaleV),
                          false);

  if (rotationAngleInRadians != 0.0) {
    _values.addUniformValue(ROTATION_CENTER_TEXTURE_COORDS,
                            new GPUUniformValueVec2Float(rotationCenterU, rotationCenterV),
                            false);

    _values.addUniformValue(ROTATION_ANGLE_TEXTURE_COORDS,
                            new GPUUniformValueFloat(rotationAngleInRadians),
                            false);
  }
}

TextureGLFeature::TextureGLFeature(const IGLTextureId* texID,
                                   IFloatBuffer* texCoords,
                                   int arrayElementSize,
                                   int index,
                                   bool normalized,
                                   int stride,
                                   bool blend,
                                   int sFactor,
                                   int dFactor,
                                   int target) :
GLColorGroupFeature(GLF_TEXTURE, 4, blend, sFactor, dFactor),
_texID(texID),
_target(target)
{

  createBasicValues(texCoords, arrayElementSize, index, normalized, stride);

}

void TextureGLFeature::applyOnGlobalGLState(GLGlobalState* state) const{
  blendingOnGlobalGLState(state);
  state->bindTexture(_target, _texID);
}

ColorGLFeature::ColorGLFeature(IFloatBuffer* colors, int arrayElementSize, int index, bool normalized, int stride,
                               bool blend, int sFactor, int dFactor):
GLColorGroupFeature(GLF_COLOR, 3, blend, sFactor, dFactor)
{
  GPUAttributeValueVec4Float* value = new GPUAttributeValueVec4Float(colors, arrayElementSize, index, stride, normalized);
  _values.addAttributeValue(COLOR, value, false);
}

FlatColorGLFeature::FlatColorGLFeature(const Color& color,
                                       bool blend, int sFactor, int dFactor):
GLColorGroupFeature(GLF_FLATCOLOR, 2, blend, sFactor, dFactor)
{
  _values.addUniformValue(FLAT_COLOR, new GPUUniformValueVec4Float(color._red,
                                                                   color._green,
                                                                   color._blue,
                                                                   color._alpha), false);
}



//////////////////////////////////////////

TextureIDGLFeature::TextureIDGLFeature(const IGLTextureId* texID):
PriorityGLFeature(COLOR_GROUP, GLF_TEXTURE_ID, 4),
_texID(texID) {
}

void TextureIDGLFeature::applyOnGlobalGLState(GLGlobalState* state) const{
  state->bindTexture(_texID);
}

BlendingModeGLFeature::BlendingModeGLFeature(bool blend, int sFactor, int dFactor):
GLColorGroupFeature(GLF_BLENDING_MODE, 4, blend, sFactor, dFactor) {
}

void BlendingModeGLFeature::applyOnGlobalGLState(GLGlobalState* state) const{
  blendingOnGlobalGLState(state);
}

TextureCoordsGLFeature::TextureCoordsGLFeature(IFloatBuffer* texCoords, int arrayElementSize, int index, bool normalized,
                                               int stride,
                                               bool coordsTransformed, const Vector2D& translate, const Vector2D& scale):
PriorityGLFeature(COLOR_GROUP, GLF_TEXTURE_COORDS, 4)
{

  GPUAttributeValueVec2Float* value = new GPUAttributeValueVec2Float(texCoords, arrayElementSize, index, stride, normalized);
  _values.addAttributeValue(TEXTURE_COORDS, value, false);

#warning ONLY TARGET 0 FOR SGNODES
  GPUUniformValueInt* texUnit = new GPUUniformValueInt(0);
  _values.addUniformValue(SAMPLER, texUnit, false);

  if (coordsTransformed) {
    _values.addUniformValue(TRANSLATION_TEXTURE_COORDS,
                            new GPUUniformValueVec2Float((float)translate._x, (float)translate._y), false);
    _values.addUniformValue(SCALE_TEXTURE_COORDS,
                            new GPUUniformValueVec2Float((float)scale._x, (float)scale._y), false);
  }

}
void TextureCoordsGLFeature::applyOnGlobalGLState(GLGlobalState* state) const{

}

ProjectionGLFeature::ProjectionGLFeature(const Camera* cam):
GLCameraGroupFeature(cam->getProjectionMatrix44D(), GLF_PROJECTION) {}

ModelGLFeature::ModelGLFeature(const Camera* cam):
GLCameraGroupFeature(cam->getModelMatrix44D(), GLF_MODEL) {}

ModelViewGLFeature::ModelViewGLFeature(const Camera* cam):
GLCameraGroupFeature(cam->getModelViewMatrix44D(), GLF_MODEL_VIEW) {}

DirectionLightGLFeature::DirectionLightGLFeature(const Vector3D& diffuseLightDirection,
                                                 const Color& diffuseLightColor,
                                                 const Color& ambientLightColor):
GLFeature(LIGHTING_GROUP, GLF_DIRECTION_LIGTH) {
  _values.addUniformValue(AMBIENT_LIGHT_COLOR,
                          new GPUUniformValueVec3Float(ambientLightColor), false);

  Vector3D dirN = diffuseLightDirection.normalized();

  _lightDirectionUniformValue = new GPUUniformValueVec3FloatMutable((float) dirN._x,
                                                                    (float) dirN._y,
                                                                    (float) dirN._z);

  _values.addUniformValue(DIFFUSE_LIGHT_DIRECTION,
                          _lightDirectionUniformValue,
                          false);
  _values.addUniformValue(DIFFUSE_LIGHT_COLOR,
                          new GPUUniformValueVec3Float(diffuseLightColor),
                          false);

}

void DirectionLightGLFeature::setLightDirection(const Vector3D& lightDir) {
  Vector3D dirN = lightDir.normalized();
  _lightDirectionUniformValue->changeValue((float)dirN._x,
                                           (float)dirN._y,
                                           (float)dirN._z);
}

VertexNormalGLFeature::VertexNormalGLFeature(IFloatBuffer* buffer, int arrayElementSize, int index, bool normalized, int stride):
GLFeature(LIGHTING_GROUP, GLF_VERTEX_NORMAL)
{
  _values.addAttributeValue(NORMAL,
                            new GPUAttributeValueVec3Float(buffer, arrayElementSize, index, stride, normalized),
                            false);

}

//
//  SceneJSShapesParser.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/7/12.
//
//

#include "SceneJSShapesParser.hpp"

#include "IJSONParser.hpp"
#include "IByteBuffer.hpp"
#include "JSONObject.hpp"
#include "JSONString.hpp"
#include "JSONArray.hpp"
#include "JSONNumber.hpp"

#include "SGNode.hpp"
#include "SGShape.hpp"
#include "SGRotateNode.hpp"
#include "SGTranslateNode.hpp"
#include "SGMaterialNode.hpp"
#include "SGTextureNode.hpp"
#include "SGGeometryNode.hpp"
#include "SGLayerNode.hpp"
#include "JSONBoolean.hpp"
#include "GLConstants.hpp"
#include "IFactory.hpp"
#include "IFloatBuffer.hpp"
#include "IIntBuffer.hpp"


Shape* SceneJSShapesParser::parse(const std::string& json,
                                  const std::string& uriPrefix) {
  return SceneJSShapesParser(json, uriPrefix).getRootShape();
}

Shape* SceneJSShapesParser::parse(const IByteBuffer* json,
                                  const std::string& uriPrefix) {
  return SceneJSShapesParser(json, uriPrefix).getRootShape();
}


SceneJSShapesParser::SceneJSShapesParser(const IByteBuffer* json,
                                         const std::string& uriPrefix) :
_uriPrefix(uriPrefix),
_rootShape(NULL)
{
  pvtParse(json->getAsString());
}

SceneJSShapesParser::SceneJSShapesParser(const std::string& json,
                                         const std::string& uriPrefix):
_uriPrefix(uriPrefix),
_rootShape(NULL)
{
  pvtParse(json);
}


void SceneJSShapesParser::pvtParse(const std::string& json) {
  JSONBaseObject* jsonRootObject = IJSONParser::instance()->parse(json);

  //  _rootShape = toShape(jsonRootObject);

  SGNode* node = toNode(jsonRootObject);

  _rootShape = new SGShape(node, _uriPrefix);

  delete jsonRootObject;
}

int SceneJSShapesParser::parseChildren(JSONObject* jsonObject,
                                       SGNode* node) const {
  int processedKeys = 0;

  JSONArray* jsNodes = jsonObject->getAsArray("nodes");
  if (jsNodes != NULL) {
    const int nodesCount = jsNodes->size();
    for (int i = 0; i < nodesCount; i++) {
      JSONObject* child = jsNodes->getAsObject(i);
      if (child != NULL) {
        SGNode* childNode = toNode(child);
        if (childNode != NULL) {
          node->addNode(childNode);
        }
      }
    }
    processedKeys++;
  }

  return processedKeys;
}

void SceneJSShapesParser::checkProcessedKeys(JSONObject* jsonObject,
                                             int processedKeys) const {
  std::vector<std::string> keys = jsonObject->keys();
  if (processedKeys != keys.size()) {
    for (int i = 0; i < keys.size(); i++) {
      printf("%s\n", keys.at(i).c_str());
    }

    ILogger::instance()->logWarning("Not all keys processed in node, processed %i of %i",
                                    processedKeys,
                                    keys.size());
  }
}

SGNode* SceneJSShapesParser::createNode(JSONObject* jsonObject) const {

  int processedKeys = 1; // "type" is already processed

  const std::string id = jsonObject->getAsString("id", "");
  if (id.compare("") != 0) {
    processedKeys++;
  }

  const std::string sId = jsonObject->getAsString("sid", "");
  if (sId.compare("") != 0) {
    processedKeys++;
  }

  SGNode* node = new SGNode(id, sId);

  processedKeys += parseChildren(jsonObject, node);

  //  std::vector<std::string> keys = jsonObject->keys();
  //  if (processedKeys != keys.size()) {
  //    for (int i = 0; i < keys.size(); i++) {
  //      printf("%s\n", keys.at(i).c_str());
  //    }
  //
  ////    ILogger::instance()->logWarning("Not all keys processed in node of type \"%s\"", type.c_str());
  //    ILogger::instance()->logWarning("Not all keys processed in node");
  //  }
  //

  checkProcessedKeys(jsonObject, processedKeys);

  return node;
}

SGRotateNode* SceneJSShapesParser::createRotateNode(JSONObject* jsonObject) const {
  int processedKeys = 1; // "type" is already processed

  const std::string id = jsonObject->getAsString("id", "");
  if (id.compare("") != 0) {
    processedKeys++;
  }

  const std::string sId = jsonObject->getAsString("sid", "");
  if (sId.compare("") != 0) {
    processedKeys++;
  }

  JSONNumber* jsX = jsonObject->getAsNumber("x");
  double x = 0.0;
  if (jsX != NULL) {
    x = jsX->value();
    processedKeys++;
  }

  JSONNumber* jsY = jsonObject->getAsNumber("y");
  double y = 0.0;
  if (jsY != NULL) {
    y = jsY->value();
    processedKeys++;
  }

  JSONNumber* jsZ = jsonObject->getAsNumber("z");
  double z = 0.0;
  if (jsZ != NULL) {
    z = jsZ->value();
    processedKeys++;
  }

  JSONNumber* jsAngle = jsonObject->getAsNumber("angle");
  double angle = 0;
  if (jsAngle != NULL) {
    angle = jsAngle->value();
    processedKeys++;
  }

  SGRotateNode* node = new SGRotateNode(id, sId,
                                        x, y, z,
                                        angle);

  processedKeys += parseChildren(jsonObject, node);

  checkProcessedKeys(jsonObject, processedKeys);

  return node;
}

SGTranslateNode* SceneJSShapesParser::createTranslateNode(JSONObject* jsonObject) const {
  int processedKeys = 1; // "type" is already processed

  const std::string id = jsonObject->getAsString("id", "");
  if (id.compare("") != 0) {
    processedKeys++;
  }

  const std::string sId = jsonObject->getAsString("sid", "");
  if (sId.compare("") != 0) {
    processedKeys++;
  }

  JSONNumber* jsX = jsonObject->getAsNumber("x");
  double x = 0.0;
  if (jsX != NULL) {
    x = jsX->value();
    processedKeys++;
  }

  JSONNumber* jsY = jsonObject->getAsNumber("y");
  double y = 0.0;
  if (jsY != NULL) {
    y = jsY->value();
    processedKeys++;
  }

  JSONNumber* jsZ = jsonObject->getAsNumber("z");
  double z = 0.0;
  if (jsZ != NULL) {
    z = jsZ->value();
    processedKeys++;
  }

  SGTranslateNode* node = new SGTranslateNode(id, sId,
                                              x, y, z);

  processedKeys += parseChildren(jsonObject, node);

  checkProcessedKeys(jsonObject, processedKeys);

  return node;
}

Color* SceneJSShapesParser::parseColor(JSONObject* jsColor) const {
  const float r = (float) jsColor->getAsNumber("r", 0.0);
  const float g = (float) jsColor->getAsNumber("g", 0.0);
  const float b = (float) jsColor->getAsNumber("b", 0.0);
  const float a = (float) jsColor->getAsNumber("a", 1.0);

  return Color::newFromRGBA(r, g, b, a);
}

SGMaterialNode* SceneJSShapesParser::createMaterialNode(JSONObject* jsonObject) const {
  int processedKeys = 1; // "type" is already processed

  const std::string id = jsonObject->getAsString("id", "");
  if (id.compare("") != 0) {
    processedKeys++;
  }

  const std::string sId = jsonObject->getAsString("sid", "");
  if (sId.compare("") != 0) {
    processedKeys++;
  }

  JSONObject* jsBaseColor = jsonObject->getAsObject("baseColor");
  Color* baseColor;
  if (jsBaseColor == NULL) {
    baseColor = Color::newFromRGBA(0, 0, 0, 1);
  }
  else {
    baseColor = parseColor(jsBaseColor);
    processedKeys++;
  }

  JSONObject* jsSpecularColor = jsonObject->getAsObject("specularColor");
  Color* specularColor;
  if (jsSpecularColor == NULL) {
    specularColor = Color::newFromRGBA(0, 0, 0, 1);
  }
  else {
    specularColor = parseColor(jsSpecularColor);
    processedKeys++;
  }

  JSONNumber* jsShine = jsonObject->getAsNumber("shine");
  double shine = 10;
  if (jsShine != NULL) {
    shine = jsShine->value();
    processedKeys++;
  }

  JSONNumber* jsSpecular = jsonObject->getAsNumber("specular");
  double specular = 1.0;
  if (jsSpecular != NULL) {
    specular = jsSpecular->value();
    processedKeys++;
  }

  JSONNumber* jsAlpha = jsonObject->getAsNumber("alpha");
  double alpha = 1.0;
  if (jsAlpha != NULL) {
    alpha = jsAlpha->value();
    processedKeys++;
  }

  JSONNumber* jsEmit = jsonObject->getAsNumber("emit");
  double emit = 0.0;
  if (jsEmit != NULL) {
    emit = jsEmit->value();
    processedKeys++;
  }

  SGMaterialNode* node = new SGMaterialNode(id, sId,
                                            baseColor,
                                            specularColor,
                                            specular,
                                            shine,
                                            alpha,
                                            emit);

  processedKeys += parseChildren(jsonObject, node);

  checkProcessedKeys(jsonObject, processedKeys);

  return node;
}

SGLayerNode* SceneJSShapesParser::createLayerNode(JSONObject* jsonObject) const {
  int processedKeys = 0; //  Layer has not "type"


  const std::string id = jsonObject->getAsString("id", "");
  if (id.compare("") != 0) {
    processedKeys++;
  }

  const std::string sId = jsonObject->getAsString("sid", "");
  if (sId.compare("") != 0) {
    processedKeys++;
  }

  const std::string uri = jsonObject->getAsString("uri", "");
  if (uri.compare("") != 0) {
    processedKeys++;
  }

  const std::string applyTo = jsonObject->getAsString("applyTo", "");
  if (applyTo.compare("") != 0) {
    processedKeys++;
  }

  const std::string blendMode = jsonObject->getAsString("blendMode", "");
  if (blendMode.compare("") != 0) {
    processedKeys++;
  }

  JSONBoolean* jsFlipY = jsonObject->getAsBoolean("flipY");
  bool flipY = true;
  if (jsFlipY != NULL) {
    flipY = jsFlipY->value();
    processedKeys++;
  }

  const std::string magFilter = jsonObject->getAsString("magFilter", "");
  if (magFilter.compare("") != 0) {
    processedKeys++;
  }

  const std::string minFilter = jsonObject->getAsString("minFilter", "");
  if (minFilter.compare("") != 0) {
    processedKeys++;
  }

  const std::string wrapS = jsonObject->getAsString("wrapS", "");
  if (wrapS.compare("") != 0) {
    processedKeys++;
  }

  const std::string wrapT = jsonObject->getAsString("wrapT", "");
  if (wrapT.compare("") != 0) {
    processedKeys++;
  }

  SGLayerNode* node = new SGLayerNode(id, sId,
                                      uri,
                                      applyTo,
                                      blendMode,
                                      flipY,
                                      magFilter,
                                      minFilter,
                                      wrapS,
                                      wrapT);

  processedKeys += parseChildren(jsonObject, node);

  checkProcessedKeys(jsonObject, processedKeys);

  return node;
}


SGTextureNode* SceneJSShapesParser::createTextureNode(JSONObject* jsonObject) const {
  int processedKeys = 1; // "type" is already processed

  const std::string id = jsonObject->getAsString("id", "");
  if (id.compare("") != 0) {
    processedKeys++;
  }

  const std::string sId = jsonObject->getAsString("sid", "");
  if (sId.compare("") != 0) {
    processedKeys++;
  }

  SGTextureNode* node = new SGTextureNode(id, sId);

  processedKeys += parseChildren(jsonObject, node);

  JSONArray* jsLayers = jsonObject->getAsArray("layers");
  if (jsLayers != NULL) {
    int layersCount = jsLayers->size();
    for (int i = 0; i < layersCount; i++) {
      JSONObject* jsLayer = jsLayers->getAsObject(i);
      if (jsLayer != NULL) {
        node->addLayer( createLayerNode(jsLayer) );
      }
    }

    processedKeys++;
  }

  checkProcessedKeys(jsonObject, processedKeys);

  return node;
}

SGGeometryNode* SceneJSShapesParser::createGeometryNode(JSONObject* jsonObject) const {
  int processedKeys = 1; // "type" is already processed

  const std::string id = jsonObject->getAsString("id", "");
  if (id.compare("") != 0) {
    processedKeys++;
  }

  const std::string sId = jsonObject->getAsString("sid", "");
  if (sId.compare("") != 0) {
    processedKeys++;
  }
  

  JSONString* jsPrimitive = jsonObject->getAsString("primitive");
  int primitive = GLPrimitive::triangles(); // triangles is the default
  if (jsPrimitive != NULL) {
    const std::string strPrimitive = jsPrimitive->value();

    if (strPrimitive.compare("points") == 0) {
      primitive = GLPrimitive::points();
    }
    else if (strPrimitive.compare("lines") == 0) {
      primitive = GLPrimitive::lines();
    }
    else if (strPrimitive.compare("line-loop") == 0) {
      primitive = GLPrimitive::lineLoop();
    }
    else if (strPrimitive.compare("line-strip") == 0) {
      primitive = GLPrimitive::lineStrip();
    }
    else if (strPrimitive.compare("triangles") == 0) {
      primitive = GLPrimitive::triangles();
    }
    else if (strPrimitive.compare("triangle-strip") == 0) {
      primitive = GLPrimitive::triangleStrip();
    }
    else if (strPrimitive.compare("triangle-fan") == 0) {
      primitive = GLPrimitive::triangleFan();
    }
  }

  JSONArray* jsPositions = jsonObject->getAsArray("positions");
  if (jsPositions == NULL) {
    ILogger::instance()->logError("Mandatory positions are not present");
    return NULL;
  }
  processedKeys++;
  int verticesCount = jsPositions->size();
  IFloatBuffer* vertices = IFactory::instance()->createFloatBuffer(verticesCount);
  for (int i = 0; i < verticesCount; i++) {
    vertices->put(i, (float) jsPositions->getAsNumber(i)->value());
  }

  JSONArray* jsColors = jsonObject->getAsArray("colors");
  IFloatBuffer* colors = NULL;
  if (jsColors != NULL) {
    const int colorsCount = jsColors->size();
    colors = IFactory::instance()->createFloatBuffer(colorsCount);
    for (int i = 0; i < colorsCount; i++) {
      colors->put(i, (float) jsColors->getAsNumber(i)->value());
    }
    processedKeys++;
  }

  JSONArray* jsUV = jsonObject->getAsArray("uv");
  IFloatBuffer* uv = NULL;
  if (jsUV != NULL) {
    const int uvCount = jsUV->size();
    uv = IFactory::instance()->createFloatBuffer(uvCount);
    bool isY = false;
    for (int i = 0; i < uvCount; i++) {
      float value = (float) jsUV->getAsNumber(i)->value();
      if (isY) {
        value = 1 - value;
      }
      isY = !isY;
      uv->put(i, value);

      int __TEXTURES_AT_WORK;
    }
    processedKeys++;
  }

  JSONArray* jsNormals = jsonObject->getAsArray("normals");
  IFloatBuffer* normals = NULL;
  if (jsNormals != NULL) {
    processedKeys++;
  }

  JSONArray* jsIndices = jsonObject->getAsArray("indices");
  if (jsIndices == NULL) {
    ILogger::instance()->logError("Non indexed geometries not supported");
    return NULL;
  }
  int indicesCount = jsIndices->size();
  IIntBuffer* indices = IFactory::instance()->createIntBuffer(indicesCount);
  for (int i = 0; i < indicesCount; i++) {
    indices->put(i, (int) jsIndices->getAsNumber(i)->value());
  }
  processedKeys++;

  SGGeometryNode* node = new SGGeometryNode(id, sId,
                                            primitive,
                                            vertices,
                                            colors,
                                            uv,
                                            normals,
                                            indices);

  processedKeys += parseChildren(jsonObject, node);

  checkProcessedKeys(jsonObject, processedKeys);

  return node;
}



SGNode* SceneJSShapesParser::toNode(JSONBaseObject* jsonBaseObject) const {

  if (jsonBaseObject == NULL) {
    return NULL;
  }

  int ____DIEGO_AT_WORK;
  JSONObject* jsonObject = jsonBaseObject->asObject();

  SGNode* result = NULL;

  if (jsonObject != NULL) {
    JSONString* jsType = jsonObject->getAsString("type");
    if (jsType != NULL) {
      const std::string type = jsType->value();
      if (type.compare("node") == 0) {
        result = createNode(jsonObject);
      }
      else if (type.compare("rotate") == 0) {
        result = createRotateNode(jsonObject);
      }
      else if (type.compare("translate") == 0) {
        result = createTranslateNode(jsonObject);
      }
      else if (type.compare("material") == 0) {
        result = createMaterialNode(jsonObject);
      }
      else if (type.compare("texture") == 0) {
        result = createTextureNode(jsonObject);
      }
      else if (type.compare("geometry") == 0) {
        result = createGeometryNode(jsonObject);
      }
      else {
        ILogger::instance()->logWarning("Unknown type \"%s\"", type.c_str());
      }
    }
  }
  
  return result;
}

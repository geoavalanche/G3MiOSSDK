//
//  GEOJSONParser.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/29/12.
//
//

#ifndef __G3MiOSSDK__GEOJSONParser__
#define __G3MiOSSDK__GEOJSONParser__

#include <string>

class IByteBuffer;

class JSONObject;
class JSONArray;

class GEOObject;
class GEOFeatureCollection;
class GEOFeature;
class GEOGeometry;
class GEOLineStringGeometry;
class GEOMultiLineStringGeometry;

#include <vector>
class Geodetic2D;

class GEOJSONParser {
private:
  const std::string _json;

  // statistics
  mutable int _coordinates2DCount;
  mutable int _lineStrings2DCount;
  mutable int _multiLineStrings2DCount;
  mutable int _featuresCount;
  mutable int _featuresCollectionCount;


  GEOJSONParser(const std::string& json) :
  _json(json),
  _coordinates2DCount(0),
  _lineStrings2DCount(0),
  _multiLineStrings2DCount(0),
  _featuresCount(0),
  _featuresCollectionCount(0)
  {

  }

  GEOObject* pvtParse() const;

  GEOObject* toGEO(const JSONObject* jsonObject) const;

  GEOFeatureCollection* createFeaturesCollection(const JSONObject* jsonObject) const;
  GEOFeature*           createFeature(const JSONObject* jsonObject) const;

  GEOGeometry*                createGeometry(const JSONObject* jsonObject) const;
  GEOLineStringGeometry*      createLineStringGeometry(const JSONObject* jsonObject) const;
  GEOMultiLineStringGeometry* createMultiLineStringGeometry(const JSONObject* jsonObject) const;


  std::vector<Geodetic2D*>* create2DCoordinates(const JSONArray* jsCoordinates) const;

  void showStatistics() const;

public:

  static GEOObject* parse(const std::string& json);

  static GEOObject* parse(const IByteBuffer* json);
  
};

#endif

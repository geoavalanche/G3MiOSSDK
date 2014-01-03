//
//  HUDQuadWidget.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 12/17/13.
//
//

#ifndef __G3MiOSSDK__HUDQuadWidget__
#define __G3MiOSSDK__HUDQuadWidget__

#include "HUDWidget.hpp"

#include <vector>
//#include "URL.hpp"
#include "Vector2D.hpp"
#include "Angle.hpp"
class HUDPosition;
class HUDSize;
class IImage;
class Mesh;
class SimpleTextureMapping;
class IImageBuilder;

#include "ChangedListener.hpp"

class HUDQuadWidget : public HUDWidget, public ChangedListener {
private:
  IImageBuilder* _imageBuilder;

  const HUDPosition* _xPosition;
  const HUDPosition* _yPosition;
  const HUDSize*     _widthSize;
  const HUDSize*     _heightSize;

  float _texCoordsTranslationU;
  float _texCoordsTranslationV;
  float _texCoordsScaleU;
  float _texCoordsScaleV;
  float _texCoordsRotationInRadians;
  float _texCoordsRotationCenterU;
  float _texCoordsRotationCenterV;

#ifdef C_CODE
  const IImage* _image;
#endif
#ifdef JAVA_CODE
  private IImage _image;
#endif
  std::string   _imageName;
  int _imageWidth;
  int _imageHeight;

  bool _buildingImage;
  std::vector<std::string> _errors;

  Mesh* _mesh;
  SimpleTextureMapping* _simpleTextureMapping;
  Mesh* createMesh(const G3MRenderContext* rc);
  Mesh* getMesh(const G3MRenderContext* rc);

  void cleanMesh();

  const G3MContext* _context;

protected:
  void rawRender(const G3MRenderContext* rc,
                 GLState* glState);

public:
  HUDQuadWidget(IImageBuilder* imageBuilder,
                HUDPosition* xPosition,
                HUDPosition* yPosition,
                HUDSize* widthSize,
                HUDSize* heightSize) :
  _imageBuilder(imageBuilder),
  _xPosition(xPosition),
  _yPosition(yPosition),
  _widthSize(widthSize),
  _heightSize(heightSize),
  _mesh(NULL),
  _simpleTextureMapping(NULL),
  _image(NULL),
  _imageWidth(0),
  _imageHeight(0),
  _buildingImage(false),
  _texCoordsTranslationU(0),
  _texCoordsTranslationV(0),
  _texCoordsScaleU(1),
  _texCoordsScaleV(1),
  _texCoordsRotationInRadians(0),
  _texCoordsRotationCenterU(0),
  _texCoordsRotationCenterV(0),
  _context(NULL)
  {
  }

  void setTexCoordsTranslation(float u, float v);

  void setTexCoordsScale(float u, float v);

  void setTexCoordsRotation(float angleInRadians,
                            float centerU,
                            float centerV);

  void setTexCoordsRotation(const Angle& angle,
                            float centerU,
                            float centerV) {
    setTexCoordsRotation((float) angle._radians,
                         centerU, centerV);
  }

  ~HUDQuadWidget();

  void initialize(const G3MContext* context);

  void onResizeViewportEvent(const G3MEventContext* ec,
                             int width,
                             int height);

  RenderState getRenderState(const G3MRenderContext* rc);

  /** private, do not call */
  void imageCreated(const IImage*      image,
                    const std::string& imageName);

  /** private, do not call */
  void onImageBuildError(const std::string& error);

  void changed();

};

#endif

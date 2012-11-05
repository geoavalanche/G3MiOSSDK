//
//  Shape.h
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 10/28/12.
//
//

#ifndef __G3MiOSSDK__Shape__
#define __G3MiOSSDK__Shape__

#include "Geodetic3D.hpp"
#include "Context.hpp"
class MutableMatrix44D;

class Shape {
private:
  Geodetic3D* _position;
  Angle*      _heading;
  Angle*      _pitch;

  MutableMatrix44D* _transformMatrix;
  MutableMatrix44D* createTransformMatrix(const Planet* planet);
  MutableMatrix44D* getTransformMatrix(const Planet* planet);

protected:
  virtual void cleanTransformMatrix();

public:
  Shape(const Geodetic3D& position) :
  _position( new Geodetic3D(position) ),
  _heading( new Angle(Angle::zero()) ),
  _pitch( new Angle(Angle::zero()) ),
  _transformMatrix(NULL) {

  }

  virtual ~Shape();
  
  Geodetic3D getPosition() const {
    return *_position;
  }

  Angle getHeading() const {
    return *_heading;
  }

  Angle getPitch() const {
    return *_pitch;
  }

  void setPosition(const Geodetic3D& position) {
    delete _position;
    _position = new Geodetic3D(position);
    cleanTransformMatrix();
  }

  void setHeading(const Angle& heading) {
    delete _heading;
    _heading = new Angle(heading);
    cleanTransformMatrix();
  }

  void setPitch(const Angle& pitch) {
    delete _pitch;
    _pitch = new Angle(pitch);
    cleanTransformMatrix();
  }

  void render(const RenderContext* rc);

  virtual bool isReadyToRender(const RenderContext* rc) = 0;
  
  virtual void rawRender(const RenderContext* rc) = 0;

};

#endif

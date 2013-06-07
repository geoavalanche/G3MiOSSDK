//
//  Shape.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 10/28/12.
//
//

#ifndef __G3MiOSSDK__Shape__
#define __G3MiOSSDK__Shape__

#include "Geodetic3D.hpp"
#include "Context.hpp"
#include "Vector3D.hpp"
#include "GLClient.hpp"

class MutableMatrix44D;

#include "Effects.hpp"
#include <vector>

#include "GLClient.hpp"

class ShapePendingEffect;
class GPUProgramState;

class Shape : public GLClient, public EffectTarget{
private:
  Geodetic3D* _position;
  
  Angle*      _heading;
  Angle*      _pitch;
  
  double      _scaleX;
  double      _scaleY;
  double      _scaleZ;
  
  const Planet* _planet;
  
  mutable MutableMatrix44D* _transformMatrix;
  MutableMatrix44D* createTransformMatrix(const Planet* planet) const;
  MutableMatrix44D* getTransformMatrix(const Planet* planet) const;
  
  std::vector<ShapePendingEffect*> _pendingEffects;
  
protected:
  virtual void cleanTransformMatrix();
  
public:
  Shape(Geodetic3D* position) :
  _position( position ),
  _heading( new Angle(Angle::zero()) ),
  _pitch( new Angle(Angle::zero()) ),
  _scaleX(1),
  _scaleY(1),
  _scaleZ(1),
  _transformMatrix(NULL),
  _planet(NULL)
  {
    
  }
  
  virtual ~Shape();
  
  const Geodetic3D getPosition() const {
    return *_position;
  }
  
  const Angle getHeading() const {
    return *_heading;
  }
  
  const Angle getPitch() const {
    return *_pitch;
  }
  
  void setPosition(Geodetic3D* position) {
    delete _position;
    _position = position;
    cleanTransformMatrix();
  }
  
  void setAnimatedPosition(const TimeInterval& duration,
                           const Geodetic3D& position,
                           bool linearInterpolation=false);
  
  void setAnimatedPosition(const TimeInterval& duration,
                           const Geodetic3D& position,
                           const Angle& pitch,
                           const Angle& heading,
                           bool linearInterpolation=false);
  
  void setAnimatedPosition(const Geodetic3D& position,
                           bool linearInterpolation=false) {
    setAnimatedPosition(TimeInterval::fromSeconds(3),
                        position,
                        linearInterpolation);
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
  
  void setScale(double scale) {
    setScale(scale, scale, scale);
  }
  
  void setScale(double scaleX,
                double scaleY,
                double scaleZ) {
    _scaleX = scaleX;
    _scaleY = scaleY;
    _scaleZ = scaleZ;
    cleanTransformMatrix();
  }
  
  void setScale(const Vector3D& scale) {
    setScale(scale._x,
             scale._y,
             scale._z);
  }
  
  Vector3D getScale() const {
    return Vector3D(_scaleX,
                    _scaleY,
                    _scaleZ);
  }
  
  void setAnimatedScale(const TimeInterval& duration,
                        double scaleX,
                        double scaleY,
                        double scaleZ);
  
  void setAnimatedScale(double scaleX,
                        double scaleY,
                        double scaleZ) {
    setAnimatedScale(TimeInterval::fromSeconds(1),
                     scaleX, scaleY, scaleZ);
  }
  
  void setAnimatedScale(const Vector3D& scale) {
    setAnimatedScale(scale._x,
                     scale._y,
                     scale._z);
  }
  
  void setAnimatedScale(const TimeInterval& duration,
                        const Vector3D& scale) {
    setAnimatedScale(duration,
                     scale._x,
                     scale._y,
                     scale._z);
  }
  
  void orbitCamera(const TimeInterval& duration,
                   double fromDistance,       double toDistance,
                   const Angle& fromAzimuth,  const Angle& toAzimuth,
                   const Angle& fromAltitude, const Angle& toAltitude);
  
  void render(const G3MRenderContext* rc);

  virtual void initialize(const G3MContext* context) {
    _planet = context->getPlanet();
  }
  
  virtual bool isReadyToRender(const G3MRenderContext* rc) = 0;
  
  virtual void rawRender(const G3MRenderContext* rc) = 0;

  virtual bool isTransparent(const G3MRenderContext* rc) = 0;
  
  virtual void modifyGPUProgramState(GPUProgramState& progState) const; //Reimplemented on AbstractMeshShape
  
};

#endif

//
//  TrailsRenderer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 10/23/12.
//
//

#ifndef __G3MiOSSDK__TrailsRenderer__
#define __G3MiOSSDK__TrailsRenderer__

#include "LeafRenderer.hpp"
#include "Geodetic3D.hpp"
#include <vector>
#include "Color.hpp"

class Mesh;
class Planet;
class Frustum;

class TrailSegment {
private:
  Color _color;
  const float _ribbonWidth;

  bool _positionsDirty;
  std::vector<Geodetic3D*> _positions;
  Geodetic3D* _nextSegmentFirstPosition;
  Geodetic3D* _previousSegmentLastPosition;

  Mesh* createMesh(const Planet* planet);

  Mesh* _mesh;
  Mesh* getMesh(const Planet* planet);
  
public:
  TrailSegment(Color color,
               float ribbonWidth) :
  _color(color),
  _ribbonWidth(ribbonWidth),
  _positionsDirty(true),
  _mesh(NULL),
  _nextSegmentFirstPosition(NULL),
  _previousSegmentLastPosition(NULL)
  {

  }

  ~TrailSegment();

  int getSize() const {
    return _positions.size();
  }

  void addPosition(const Geodetic3D& position) {
    _positionsDirty = true;
    _positions.push_back(new Geodetic3D(position));
  }

  void setNextSegmentFirstPosition(const Geodetic3D& position) {
    _positionsDirty = true;
    delete _nextSegmentFirstPosition;
    _nextSegmentFirstPosition = new Geodetic3D(position);
  }

  void setPreviousSegmentLastPosition(const Geodetic3D& position) {
    _positionsDirty = true;
    delete _previousSegmentLastPosition;
    _previousSegmentLastPosition = new Geodetic3D(position);
  }

  Geodetic3D getLastPosition() const {
    return *(_positions[ _positions.size() - 1]);
  }

  Geodetic3D getPreLastPosition() const {
    return *(_positions[ _positions.size() - 2]);
  }

  void render(const G3MRenderContext* rc,
              const GLState& parentState,
              const Frustum* frustum);

};

class Trail {
private:
  bool _visible;

  Color _color;
  const float _ribbonWidth;

  std::vector<TrailSegment*> _segments;


public:
  Trail(Color color,
        float ribbonWidth):
  _visible(true),
  _color(color),
  _ribbonWidth(ribbonWidth)
  {
  }

  ~Trail();

  void render(const G3MRenderContext* rc,
              const GLState& parentState,
              const Frustum* frustum);

  void setVisible(bool visible) {
    _visible = visible;
  }

  bool isVisible() const {
    return _visible;
  }

  void addPosition(const Geodetic3D& position);
  
};


class TrailsRenderer : public LeafRenderer {
private:
  std::vector<Trail*> _trails;

public:
  TrailsRenderer() {
  }

  void addTrail(Trail* trail);

  virtual ~TrailsRenderer();

  void onResume(const G3MContext* context) {

  }

  void onPause(const G3MContext* context) {

  }

  void onDestroy(const G3MContext* context) {

  }

  void initialize(const G3MContext* context) {

  }

  bool isReadyToRender(const G3MRenderContext* rc) {
    return true;
  }

  bool onTouchEvent(const G3MEventContext* ec,
                    const TouchEvent* touchEvent) {
    return false;
  }

  void onResizeViewportEvent(const G3MEventContext* ec,
                             int width, int height) {

  }

  void start(const G3MRenderContext* rc) {

  }

  void stop(const G3MRenderContext* rc) {

  }

  void render(const G3MRenderContext* rc,
              const GLState& parentState);
  
};

#endif

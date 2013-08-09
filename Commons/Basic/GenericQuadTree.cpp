//
//  GenericQuadTree.cpp
//  G3MiOSSDK
//
//  Created by Jose Miguel SN on 07/08/13.
//
//

#include "GenericQuadTree.hpp"

#include "GEORasterLineSymbol.hpp"
#include "ICanvas.hpp"

#pragma mark NODE

GenericQuadTree_Node::~GenericQuadTree_Node() {
  const int elementsSize = _elements.size();
  for (int i = 0; i < elementsSize; i++) {
    delete _elements[i];
  }

  if (_children != NULL) {
    for (int i = 0; i < 4; i++) {
      delete _children[i];
    }

    delete [] _children;
  }

  delete _sector;
}

//void GenericQuadTree_Node::computeElementsSector(){
//
//  delete _elementsSector;
//  _elementsSector = new Sector(_sector);
//  if (_children == NULL){
//    const int size = _elements.size();
//    for (int i = 0; i < size; i++) {
//      Sector newElementSector = _elementsSector->mergedWith(_elements[i]->getSector());
//      delete _elementsSector;
//      _elementsSector = new Sector(newElementSector);
//    }
//
//  } else{
//
//    for (int i = 0; i < 4; i++) {
//      Sector newElementSector =_elementsSector->mergedWith(_children[i]->getElementsSector());
//      delete _elementsSector;
//      _elementsSector = new Sector(newElementSector);
//    }
//
//  }
//
//}

void GenericQuadTree_Node::splitNode(int maxElementsPerNode,
                                     int maxDepth){
  _children = new GenericQuadTree_Node*[4];

  const Geodetic2D lower = _sector->_lower;
  const Geodetic2D upper = _sector->_upper;

  const Angle splitLongitude = Angle::midAngle(lower._longitude, upper._longitude);
  const Angle splitLatitude  = Angle::midAngle(lower._latitude,  upper._latitude);

  const Sector sector0(lower,
                       Geodetic2D(splitLatitude, splitLongitude));

  const Sector sector1(Geodetic2D(lower._latitude, splitLongitude),
                       Geodetic2D(splitLatitude, upper._longitude));

  const Sector sector2(Geodetic2D(splitLatitude, lower._longitude),
                       Geodetic2D(upper._latitude, splitLongitude));

  const Sector sector3(Geodetic2D(splitLatitude, splitLongitude),
                       upper);

  _children[0] = new GenericQuadTree_Node(sector0, this);
  _children[1] = new GenericQuadTree_Node(sector1, this);
  _children[2] = new GenericQuadTree_Node(sector2, this);
  _children[3] = new GenericQuadTree_Node(sector3, this);

  //Split elements by sector
  std::vector<GenericQuadTree_Element*> elementsToBeInserted = _elements;
  _elements.clear();
  const int size = elementsToBeInserted.size();
  for (int i = 0; i < size; i++) {
    this->add(elementsToBeInserted[i], maxElementsPerNode, maxDepth);
  }


  //  const int size = _elements.size();
  //  for (int i = 0; i < size; i++) {
  //    GenericQuadTree_Element* e = _elements[i];
  //
  //    for (int j = 0; j < 4; j++){
  //      GenericQuadTree_Node* child = _children[j];
  //      if (child->add(e, maxElementsPerNode, maxDepth)){
  //        break;
  //      }
  //    }
  //  }
  //
  //  _elements.clear();
}

GenericQuadTree_Node* GenericQuadTree_Node::getBestNodeForInsertion(GenericQuadTree_Element* element){


  Sector sector = element->getSector();
//  double myArea = _sector->getAngularAreaInSquaredDegrees();
  double myArea = getAreaInSquaredDegreesAfterInsertion(sector);

  if (sector.getAngularAreaInSquaredDegrees() > (myArea * 0.3)){
    return this;
  }

  double minChildInsertionCost = _children[0]->getInsertionCostInSquaredDegrees(sector);
  GenericQuadTree_Node* bestChildForInsertion = _children[0];
  for (int i = 1; i < 4; i++) {
    GenericQuadTree_Node* child = _children[i];
    double cost = child->getInsertionCostInSquaredDegrees(sector);
    if (cost < minChildInsertionCost){
      minChildInsertionCost = cost;
      bestChildForInsertion = child;
    } else{

      if (cost == minChildInsertionCost){
//        printf("BOTH CHILDREN WITH SAME COST");

        int n1 = bestChildForInsertion->getSubtreeNElements();
        int n2 = child->getSubtreeNElements();

        if (n2 < n1){ //SAME COST BUT LESS CONFLICTS
          minChildInsertionCost = cost;
          bestChildForInsertion = child;
        }
      }

    }
  }

  if (minChildInsertionCost == 0){
    return bestChildForInsertion;
  }

  if (minChildInsertionCost > (myArea / 2)){ //We store in parent
    return this;
  }
  
  return bestChildForInsertion;

}

bool GenericQuadTree_Node::add(GenericQuadTree_Element* element,
                               int maxElementsPerNode,
                               int maxDepth) {

  if (_children == NULL){ //LEAF NODE

    if (_elements.size() < maxElementsPerNode || _depth == maxDepth){ //Tipical insertion
      _elements.push_back(element);
      increaseNodeSector(element);
      return true;
    }

    //Node must create children
    splitNode(maxElementsPerNode, maxDepth);//We must split
    return this->add(element, maxElementsPerNode, maxDepth); //We try it again, this time as inner node

  } else{ //INNER NODE

    //Calculate best node for insertion
    GenericQuadTree_Node* bestInsertionNode = getBestNodeForInsertion(element);

    if (bestInsertionNode == this){
      _elements.push_back(element);
      increaseNodeSector(element);
      return true;

    } else{ //Inserting into child
      increaseNodeSector(element);
      return bestInsertionNode->add(element, maxElementsPerNode, maxDepth);
    }

  }

  //  Geodetic2D position = element->getCenter();
  //
  //  if (!_sector->contains(position)) {
  //    return false;
  //  }
  //
  //  if (_children == NULL){ //LEAF NODE
  //
  //    _elements.push_back(element);
  //    if (_elements.size() > maxElementsPerNode && _depth < maxDepth){
  //      //WE SPLIT THE NODE
  //      splitNode(maxElementsPerNode, maxDepth);
  //    }
  //  } else{ //INNER NODE
  //
  //    for (int j = 0; j < 4; j++){
  //      GenericQuadTree_Node* child = _children[j];
  //      if (child->add(element, maxElementsPerNode, maxDepth)){
  //        break;
  //      }
  //    }
  //  }
  //
  ////  computeElementsSector();
  //
  //  return true;

}

bool GenericQuadTree_Node::acceptVisitor(const Sector& sector,
                                         const GenericQuadTreeVisitor& visitor) const {



  visitor.addComparisonsDoneWhileVisiting(1);

  if (!_sector->touchesWith(sector)) {
    return false;
  }

  const int elementsSize = _elements.size();
  visitor.addComparisonsDoneWhileVisiting(elementsSize);

  for (int i = 0; i < elementsSize; i++) {
    GenericQuadTree_Element* element = _elements[i];

    if (element->isSectorElement()){ //Element is associated to sector
      GenericQuadTree_SectorElement* e = (GenericQuadTree_SectorElement*) element;
      if (e->_sector.touchesWith(sector)) {
        const bool abort = visitor.visitElement(e->_sector, element->_element);
        if (abort) {
          return true;
        }
      }
    } else{ //Element is associated to geodetic
      GenericQuadTree_Geodetic2DElement* e = (GenericQuadTree_Geodetic2DElement*) element;
      if (sector.contains(e->_geodetic)) {
        const bool abort = visitor.visitElement(e->_geodetic, element->_element);
        if (abort) {
          return true;
        }
      }

    }
  }

  if (_children != NULL) {
    for (int i = 0; i < 4; i++) {
      GenericQuadTree_Node* child = _children[i];
      const bool abort = child->acceptVisitor(sector, visitor);
      if (abort) {
        return true;
      }
    }
  }

  return false;
}

bool GenericQuadTree_Node::acceptVisitor(const Geodetic2D& geo,
                                         const GenericQuadTreeVisitor& visitor) const {

  
  visitor.addComparisonsDoneWhileVisiting(1);
  
  if (!_sector->contains(geo)) {
    return false;
  }

  const int elementsSize = _elements.size();
  visitor.addComparisonsDoneWhileVisiting(elementsSize);
  
  for (int i = 0; i < elementsSize; i++) {
    GenericQuadTree_Element* element = _elements[i];

    if (element->isSectorElement()){ //Element is associated to sector
      GenericQuadTree_SectorElement* e = (GenericQuadTree_SectorElement*) element;
      if (e->_sector.contains(geo)) {
        const bool abort = visitor.visitElement(e->_sector, element->_element);
        if (abort) {
          return true;
        }
      }
    } else{ //Element is associated to geodetic
      GenericQuadTree_Geodetic2DElement* e = (GenericQuadTree_Geodetic2DElement*) element;
      if (geo.isEqualsTo(e->_geodetic)) {
        const bool abort = visitor.visitElement(e->_geodetic, element->_element);
        if (abort) {
          return true;
        }
      }

    }
  }

  if (_children != NULL) {
    for (int i = 0; i < 4; i++) {
      GenericQuadTree_Node* child = _children[i];
      const bool abort = child->acceptVisitor(geo, visitor);
      if (abort) {
        return true;
      }
    }
  }

  return false;
}

bool GenericQuadTree_Node::acceptNodeVisitor(GenericQuadTreeNodeVisitor& visitor) const{
  if (visitor.visitNode(this)){
    return true;
  }
  if (_children != NULL){
    for (int i = 0; i < 4; i++) {
      GenericQuadTree_Node* child = _children[i];
      if (child->acceptNodeVisitor(visitor)){
        return true;
      }
    }
  }
  return false;
}

double GenericQuadTree_Node::getInsertionCostInSquaredDegrees(const Sector& sector) const{

  Sector newSector = _sector->mergedWith(sector);
  double areaAfter = newSector.getAngularAreaInSquaredDegrees();

  double areaNow = _sector->getAngularAreaInSquaredDegrees();
  return areaAfter - areaNow;
}

double GenericQuadTree_Node::getAreaInSquaredDegreesAfterInsertion(const Sector& sector) const{
  Sector newSector = _sector->mergedWith(sector);
  return newSector.getAngularAreaInSquaredDegrees();
}

void GenericQuadTree_Node::increaseNodeSector(GenericQuadTree_Element* element){
  Sector s = *_sector;
  delete _sector;
  _sector = new Sector(s.mergedWith(element->getSector()));
}

void GenericQuadTree_Node::symbolize(GEOTileRasterizer* geoTileRasterizer) const{

  if (_elements.size() > 0){
    std::vector<Geodetic2D*> line;

    line.push_back( new Geodetic2D( _sector->getSW() ) );
    line.push_back( new Geodetic2D( _sector->getNW() ) );
    line.push_back( new Geodetic2D( _sector->getNE() ) );
    line.push_back( new Geodetic2D( _sector->getSE() ) );
    line.push_back( new Geodetic2D( _sector->getSW() ) );

//    printf("RESTERIZING: %s\n", _sector->description().c_str());

    float dashLengths[] = {};
    int dashCount = 0;

    Color c = Color::red().wheelStep(12, _depth);

    GEO2DLineRasterStyle ls(c, //const Color&     color,
                            (float)1.0, //const float      width,
                            CAP_ROUND, // const StrokeCap  cap,
                            JOIN_ROUND, //const StrokeJoin join,
                            1,//const float      miterLimit,
                            dashLengths,//float            dashLengths[],
                            dashCount,//const int        dashCount,
                            0);//const int        dashPhase) :


    GEORasterLineSymbol * symbol = new GEORasterLineSymbol(&line, ls);

    geoTileRasterizer->addSymbol(symbol);
  }

  if (_children != NULL){
    for (int i = 0; i < 4; i++) {
      _children[i]->symbolize(geoTileRasterizer);
    }
  }


}

///////////////////////////////
#pragma mark TREE

GenericQuadTree::~GenericQuadTree() {
  delete _root;
}

bool GenericQuadTree::add(GenericQuadTree_Element* element){

  if (_root == NULL){
    _root = new GenericQuadTree_Node(element->getSector());
  }

  return _root->add(element, _maxElementsPerNode, _maxDepth);
}

bool GenericQuadTree::add(const Sector& sector,
                          const void* element) {
  GenericQuadTree_SectorElement* sectorElement = new GenericQuadTree_SectorElement(sector, element);
  return add(sectorElement);
}

bool GenericQuadTree::add(const Geodetic2D& geodetic,
                          const void* element) {
  GenericQuadTree_Geodetic2DElement* geodeticElement = new GenericQuadTree_Geodetic2DElement(geodetic, element);
  return add(geodeticElement);
}

bool GenericQuadTree::acceptVisitor(const Sector& sector,
                                    const GenericQuadTreeVisitor& visitor) const {
  const bool aborted = _root->acceptVisitor(sector, visitor);
  visitor.endVisit(aborted);
  return aborted;
}

bool GenericQuadTree::acceptVisitor(const Geodetic2D& geo,
                                    const GenericQuadTreeVisitor& visitor) const{

  const bool aborted = _root->acceptVisitor(geo, visitor);
  visitor.endVisit(aborted);
  return aborted;

}

bool GenericQuadTree::acceptNodeVisitor(GenericQuadTreeNodeVisitor& visitor) const{
  const bool aborted = _root->acceptNodeVisitor(visitor);
  visitor.endVisit(aborted);
  return aborted;
}

void GenericQuadTree::symbolize(GEOTileRasterizer* geoTileRasterizer) const{
  _root->symbolize(geoTileRasterizer);
}

///////////////////////////////////////////////////////////////////////

#pragma mark TESTER


int GenericQuadTree_TESTER::_nComparisons = 0;
int GenericQuadTree_TESTER::_nElements = 0;

void GenericQuadTree_TESTER::run(int nElements,  GEOTileRasterizer* rasterizer){
  
  _nElements = 0;
  _nComparisons = 0;


  GenericQuadTree tree;
  std::vector<Sector*> sectors;
  std::vector<Geodetic2D*> geos;

  for (int i = 0; i < nElements; i++) {


    double minLat = random(90);// rand()%180 -90;
    double minLon = random(180);//rand()%360 -180;

    int type = rand();
    if (type%2 == 0){

      double maxLat = minLat + random(90 - (int)minLat); //rand()%(90 - (int)minLat);
      double maxLon = minLon + random(90 - (int)minLat);//rand()%(180 - (int)minLon);

      Sector s = Sector::fromDegrees(minLat, minLon, maxLat, maxLon);
      sectors.push_back(new Sector(s));
      std::string desc = "SECTOR ELEMENT " + s.description();
      std::string* element = new std::string();
      *element = desc;

      if (!tree.add(s, element)){
        ILogger::instance()->logInfo("ERROR");
      }

    } else{

      Geodetic2D geo = Geodetic2D::fromDegrees(minLat, minLon);
      geos.push_back(new Geodetic2D(geo));
      std::string desc = "GEODETIC ELEMENT " + geo.description();
      std::string* element = new std::string();
      *element = desc;

      if (!tree.add(geo, element)){
        ILogger::instance()->logInfo("ERROR");
      }
    }
  }

  for (int i = 0; i < sectors.size(); i++){
    Sector s = *sectors[i];
    GenericQuadTreeVisitorSector_TESTER vis(s);
    tree.acceptVisitor(s, vis);
  }

  for (int i = 0; i < geos.size(); i++){
    Geodetic2D g = *geos[i];
    GenericQuadTreeVisitorGeodetic_TESTER vis(g);
    tree.acceptVisitor(g, vis);
  }
  
  NodeVisitor_TESTER nodeVis;
  tree.acceptNodeVisitor(nodeVis);
  
  if (rasterizer != NULL){
    tree.symbolize(rasterizer);
  }

  double c_e = (float)_nComparisons / _nElements;
  ILogger::instance()->logInfo("NElements Found = %d, Mean NComparisons = %f -> COEF: %f\n", _nElements, c_e, c_e / _nElements);
  
}
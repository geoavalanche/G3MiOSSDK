//
//  CompositeTileImageProvider.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 4/23/14.
//
//

#include "CompositeTileImageProvider.hpp"
#include "TileImageListener.hpp"
#include "Tile.hpp"
#include "CompositeTileImageContribution.hpp"
#include "IFactory.hpp"
#include "ICanvas.hpp"
#include "IImage.hpp"

CompositeTileImageProvider::~CompositeTileImageProvider() {
  for (int i = 0; i < _childrenSize; i++) {
    TileImageProvider* child = _children[i];
    child->_release();
  }
#ifdef JAVA_CODE
  super.dispose();
#endif
}

const TileImageContribution* CompositeTileImageProvider::contribution(const Tile* tile) {
  std::vector<const CompositeTileImageContribution::ChildContribution*> childrenContributions;

  for (int i = 0; i < _childrenSize; i++) {
    TileImageProvider* child = _children[i];
    const TileImageContribution* childContribution = child->contribution(tile);
    if (childContribution != NULL) {
      // ignore previous contributions, they are covered by the current fullCoverage & Opaque contribution.
      const int childrenContributionsSize = childrenContributions.size();
      if ((childrenContributionsSize > 0) &&
          childContribution->isFullCoverageAndOpaque()) {
        for (int j = 0; j < childrenContributionsSize; j++) {
          const CompositeTileImageContribution::ChildContribution* previousContribution = childrenContributions[j];
#ifdef C_CODE
          delete previousContribution;
#endif
#ifdef JAVA_CODE
          previousContribution.dispose();
#endif
        }
        childrenContributions.clear();
      }

      childrenContributions.push_back( new CompositeTileImageContribution::ChildContribution(i, childContribution) );
    }
  }

  return CompositeTileImageContribution::create(childrenContributions);
}

CompositeTileImageProvider::ChildResult::~ChildResult() {
  delete _image;
}

const CompositeTileImageProvider::ChildResult* CompositeTileImageProvider::ChildResult::image(const IImage*                image,
                                                                                              const std::string&           imageId,
                                                                                              const TileImageContribution* contribution) {
  return new CompositeTileImageProvider::ChildResult(false ,        // isError
                                                     false,         // isCanceled
                                                     image,
                                                     imageId,
                                                     contribution,
                                                     ""             // error
                                                     );
}

const CompositeTileImageProvider::ChildResult* CompositeTileImageProvider::ChildResult::error(const std::string& error) {
  return new CompositeTileImageProvider::ChildResult(true,  // isError
                                                     false, // isCanceled
                                                     NULL,  // image
                                                     "",    // imageId
                                                     NULL,  // contribution
                                                     error);
}

const CompositeTileImageProvider::ChildResult* CompositeTileImageProvider::ChildResult::cancelation() {
  return new CompositeTileImageProvider::ChildResult(false, // isError
                                                     true,  // isCanceled
                                                     NULL,  // image
                                                     "",    // imageId
                                                     NULL,  // contribution
                                                     ""     // error
                                                     );
}

CompositeTileImageProvider::Composer::Composer(int width,
                                               int height,
                                               CompositeTileImageProvider* compositeTileImageProvider,
                                               const std::string& tileId,
                                               TileImageListener* listener,
                                               bool deleteListener,
                                               const CompositeTileImageContribution* compositeContribution) :
_width(width),
_height(height),
_compositeTileImageProvider(compositeTileImageProvider),
_tileId(tileId),
_listener(listener),
_deleteListener(deleteListener),
_compositeContribution(compositeContribution),
_contributionsSize( compositeContribution->size() ),
_stepsDone(0),
_anyError(false),
_anyCancelation(false),
_canceled(false)
{
  for (int i = 0; i < _contributionsSize; i++) {
    _results.push_back( NULL );
  }
}

CompositeTileImageProvider::Composer::~Composer() {
#warning remove debug printf
//  printf("**** deleted CompositeTileImageProvider::Composer %p (_stepsDone=%d, _anyError=%s, _anyCancelation=%s, _canceled=%s, _compositeContribution=%p)\n",
//         this,
//         _stepsDone,
//         _anyError       ? "true" : "false",
//         _anyCancelation ? "true" : "false",
//         _canceled       ? "true" : "false",
//         _compositeContribution
//         );
  for (int i = 0; i < _contributionsSize; i++) {
    const ChildResult* result = _results[i];
    delete result;
  }
  delete _compositeContribution;
}

void CompositeTileImageProvider::Composer::cleanUp() {
  if (_deleteListener) {
    delete _listener;
    _listener = NULL;
  }

  _compositeTileImageProvider->composerDone(this);
}

void CompositeTileImageProvider::Composer::done() {
  if (_canceled) {
    cleanUp();
    return;
  }

  if (_contributionsSize == 1) {
    const ChildResult* singleResult = _results[0];

    if (singleResult->_isError) {
      _listener->imageCreationError(_tileId,
                                    singleResult->_error);
    }
    else if (singleResult->_isCanceled) {
      _listener->imageCreationCanceled(_tileId);
    }
    else {
      _listener->imageCreated(singleResult->_imageId,
                              singleResult->_image->shallowCopy(),
                              singleResult->_imageId,
                              singleResult->_contribution);
    }

    cleanUp();
  }
  else {
    if (_anyError) {
      std::string composedError = "";
      for (int i = 0; i < _contributionsSize; i++) {
        const ChildResult* childResult = _results[i];
        if (childResult->_isError) {
          composedError += childResult->_error + " ";
        }
      }

      _listener->imageCreationError(_tileId,
                                    composedError);

      cleanUp();
    }
    else if (_anyCancelation) {
      _listener->imageCreationCanceled(_tileId);
      cleanUp();
    }
    else {
      ICanvas* canvas = IFactory::instance()->createCanvas();

      canvas->initialize(_width, _height);

      std::string imageId = "";

      for (int i = 0; i < _contributionsSize; i++) {
       const ChildResult* result = _results[i];

        imageId += result->_imageId + "|";
        canvas->drawImage(result->_image, 0, 0);
      }
      _imageId = imageId;

      canvas->createImage(this, false);

      delete canvas;
    }
  }
}

void CompositeTileImageProvider::Composer::imageCreated(const IImage* image) {
  _listener->imageCreated(_tileId,
                          image,
                          _imageId,
                          _compositeContribution);
  _compositeContribution = NULL; // the _compositeContribution ownership moved to the listener
  cleanUp();
}

void CompositeTileImageProvider::Composer::stepDone() {
  _stepsDone++;
  if (_stepsDone == _contributionsSize) {
    done();
  }
}

void CompositeTileImageProvider::Composer::imageCreated(const std::string&           tileId,
                                                        const IImage*                image,
                                                        const std::string&           imageId,
                                                        const TileImageContribution* contribution,
                                                        const int                    index) {
//  if (_results[index] != NULL) {
//    printf("Logic error 1\n");
//  }
  _results[index] = ChildResult::image(image, imageId, contribution);
  stepDone();
}

void CompositeTileImageProvider::Composer::imageCreationError(const std::string& error,
                                                              const int          index) {
//  if (_results[index] != NULL) {
//    printf("Logic error 2\n");
//  }
  _results[index] = ChildResult::error(error);
  _anyError = true;
  stepDone();
}

void CompositeTileImageProvider::Composer::imageCreationCanceled(const int index) {
//  if (_results[index] != NULL) {
//    printf("Logic error 3\n");
//  }
  _results[index] = ChildResult::cancelation();
  _anyCancelation = true;
  stepDone();
}

void CompositeTileImageProvider::Composer::cancel(const Tile* tile) {
  _canceled = true;
  _compositeTileImageProvider->cancelChildren(tile, _compositeContribution);
}

void CompositeTileImageProvider::ChildTileImageListener::imageCreated(const std::string&           tileId,
                                                                      const IImage*                image,
                                                                      const std::string&           imageId,
                                                                      const TileImageContribution* contribution) {
  _composer->imageCreated(tileId, image, imageId, contribution, _index);
}

void CompositeTileImageProvider::ChildTileImageListener::imageCreationError(const std::string& tileId,
                                                                            const std::string& error) {
  _composer->imageCreationError(error, _index);
}

void CompositeTileImageProvider::ChildTileImageListener::imageCreationCanceled(const std::string& tileId) {
  _composer->imageCreationCanceled(_index);
}

void CompositeTileImageProvider::create(const Tile* tile,
                                        const TileImageContribution* contribution,
                                        const Vector2I& resolution,
                                        long long tileDownloadPriority,
                                        bool logDownloadActivity,
                                        TileImageListener* listener,
                                        bool deleteListener) {

  const CompositeTileImageContribution* compositeContribution = (const CompositeTileImageContribution*) contribution;

  const std::string tileId = tile->_id;

  Composer* composer = new Composer(resolution._x,
                                    resolution._y,
                                    this,
                                    tileId,
                                    listener,
                                    deleteListener,
                                    compositeContribution);

  _composers[ tileId ] = composer;

  const int contributionsSize = compositeContribution->size();
  for (int i = 0; i < contributionsSize; i++) {
    const CompositeTileImageContribution::ChildContribution* childContribution = compositeContribution->get(i);

    TileImageProvider* child = _children[ childContribution->_childIndex ];

    child->create(tile,
                  childContribution->_contribution,
                  resolution,
                  tileDownloadPriority,
                  logDownloadActivity,
                  new ChildTileImageListener(composer, i),
                  true);
  }
}

void CompositeTileImageProvider::cancel(const Tile* tile) {
  const std::string tileId = tile->_id;
#ifdef C_CODE
  if (_composers.find(tileId) != _composers.end()) {
    Composer* composer = _composers[tileId];

    composer->cancel(tile);

    _composers.erase(tileId);
  }
#endif
#ifdef JAVA_CODE
  final Composer composer = _composers.remove(tileId);
  if (composer != null) {
    composer.cancel(tile);
  }
#endif
}

void CompositeTileImageProvider::composerDone(Composer* composer) {
  _composers.erase( composer->_tileId );
  delete composer;
}

void CompositeTileImageProvider::cancelChildren(const Tile* tile,
                                                const CompositeTileImageContribution* compositeContribution) {
  const int contributionsSize = compositeContribution->size();

  // store all the indexes before calling child->cancel().
  // child->cancel() can force the deletion of the builder (and in order the deletion of compositeContribution)
  int* indexes = new int[contributionsSize];
  for (int i = 0; i < contributionsSize; i++) {
    indexes[i] = compositeContribution->get(i)->_childIndex;
  }

  for (int i = 0; i < contributionsSize; i++) {
    TileImageProvider* child = _children[ indexes[i] ];
    child->cancel(tile);
  }

  delete [] indexes;
}

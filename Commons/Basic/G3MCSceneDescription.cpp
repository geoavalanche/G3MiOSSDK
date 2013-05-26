//
//  G3MCSceneDescription.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 5/26/13.
//
//

#include "G3MCSceneDescription.hpp"
#include "IStringBuilder.hpp"

const std::string G3MCSceneDescription::getId() const {
  return _id;
}

const std::string G3MCSceneDescription::getUser() const {
  return _user;
}

const std::string G3MCSceneDescription::getName() const {
  return _name;
}

const std::string G3MCSceneDescription::getDescription() const {
  return _description;
}

const std::string G3MCSceneDescription::getIconURL() const {
  return _iconURL;
}

std::vector<std::string> G3MCSceneDescription::getTags() const {
  std::vector<std::string> result;
  const int size = _tags.size();
  for (int i = 0; i < size; i++) {
    result.push_back(_tags[i]);
  }
  return result;
}

const std::string G3MCSceneDescription::description() const {
  //return "TODO: G3MCSceneDescription::description()";
  IStringBuilder *isb = IStringBuilder::newStringBuilder();
  //isb->addDouble(_degrees);
  isb->addString("[Scene id=");
  isb->addString(_id);
  isb->addString(", user=");
  isb->addString(_user);
  isb->addString(", name=");
  isb->addString(_name);
  isb->addString(", description=");
  isb->addString(_description);
  isb->addString(", iconURL=");
  isb->addString(_iconURL);

  isb->addString(", tags=[");
  const int tagsSize = _tags.size();
  for (int i = 0; i < tagsSize; i++) {
    if (i > 0) {
      isb->addString(", ");
    }
    isb->addString(_tags[i]);
  }
  isb->addString("]");

  isb->addString("]");
  const std::string s = isb->getString();
  delete isb;
  return s;
}

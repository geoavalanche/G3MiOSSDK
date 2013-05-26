//
//  G3MCSceneDescription.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 5/26/13.
//
//

#ifndef __G3MiOSSDK__G3MCSceneDescription__
#define __G3MiOSSDK__G3MCSceneDescription__

#include <string>
#include <vector>

class G3MCSceneDescription {
private:
  const std::string              _id;
  const std::string              _user;
  const std::string              _name;
  const std::string              _description;
  const std::string              _iconURL;
#ifdef C_CODE
  const std::vector<std::string> _tags;
#endif
#ifdef JAVA_CODE
  private final java.util.ArrayList<String> _tags;
#endif

public:

  G3MCSceneDescription(const std::string&        id,
                       const std::string&        user,
                       const std::string&        name,
                       const std::string&        description,
                       const std::string&        iconURL,
                       std::vector<std::string>& tags) :
  _id(id),
  _user(user),
  _name(name),
  _description(description),
  _iconURL(iconURL),
  _tags(tags)
  {

  }

  const std::string getId() const;

  const std::string getUser() const;

  const std::string getName() const;

  const std::string getDescription() const;

  const std::string getIconURL() const;

  std::vector<std::string> getTags() const;

  virtual ~G3MCSceneDescription() {

  }

  const std::string description() const;
  
};

#endif

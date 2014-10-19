#include "BaseSource.h"

namespace ofx {
  namespace piMapper {
    BaseSource::BaseSource() {
      //cout << "BaseSource" << endl;
      init();
    }
    
    BaseSource::BaseSource(ofTexture* newTexture) {
      init();
      texture = newTexture;
    }
    
    BaseSource::~BaseSource() {}
    
    ofTexture* BaseSource::getTexture() {
      return texture;
    }
    
    std::string& BaseSource::getName() {
      return name;
    }
    
    bool BaseSource::isLoadable() {
      return loadable;
    }
    
    bool BaseSource::isLoaded() {
      return loaded;
    }
    
    int BaseSource::getType() {
      return type;
    }
    
    std::string& BaseSource::getPath() {
      return path;
    }
    
    void BaseSource::init() {
      texture = NULL;
      name = "";
      path = "";
      loadable = false;
      loaded = false;
      type = SourceType::SOURCE_TYPE_NONE;
      referenceCount = 1; // We have one instance on init
    }
    
    void BaseSource::setNameFromPath(std::string& fullPath) {
      std::vector<string> pathParts;
      //cout << "fullPath: " << fullPath << endl;
      pathParts = ofSplitString(fullPath, "/"); // Maybe on win "/" is "\", have to test
      //cout << "lastPathPart: " << pathParts[pathParts.size() - 1] << endl;
      name = pathParts[pathParts.size() - 1];
    }
  }
}
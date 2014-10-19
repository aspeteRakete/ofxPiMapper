#include "SurfaceManager.h"

namespace ofx {
namespace piMapper {
  SurfaceManager::SurfaceManager() {
    // Init variables
    mediaServer = NULL;
  }

SurfaceManager::~SurfaceManager() { clear(); }

void SurfaceManager::draw() {
  for (int i = 0; i < surfaces.size(); i++) {
    surfaces[i]->draw();
  }
}

void SurfaceManager::addSurface(int surfaceType) {
  if (surfaceType == SurfaceType::TRIANGLE_SURFACE) {
    surfaces.push_back(new TriangleSurface());
  } else if (surfaceType == SurfaceType::QUAD_SURFACE) {
    surfaces.push_back(new QuadSurface());
  } else {
    ofLogFatalError("SurfaceManager") << "Attempt to add non-existing surface type";
    std::exit(EXIT_FAILURE);
  }
}

void SurfaceManager::addSurface(int surfaceType, BaseSource* newSource) {
  if (surfaceType == SurfaceType::TRIANGLE_SURFACE) {
    surfaces.push_back(new TriangleSurface());
    surfaces.back()->setSource(newSource);
  } else if (surfaceType == SurfaceType::QUAD_SURFACE) {
    surfaces.push_back(new QuadSurface());
    surfaces.back()->setSource(newSource);
  } else {
    ofLogFatalError("SurfaceManager") << "Attempt to add non-existing surface type";
    std::exit(EXIT_FAILURE);
  }
}

void SurfaceManager::addSurface(int surfaceType, vector<ofVec2f> vertices,
                                vector<ofVec2f> texCoords) {
  if (surfaceType == SurfaceType::TRIANGLE_SURFACE) {
    if (vertices.size() < 3) {
      throw std::runtime_error(
          "There must be 3 vertices for a triangle surface.");
    } else if (texCoords.size() < 3) {
      throw std::runtime_error(
          "There must be 3 texture coordinates for a triangle surface.");
    }

    surfaces.push_back(new TriangleSurface());

    for (int i = 0; i < 3; i++) {
      surfaces.back()->setVertex(i, vertices[i]);
      surfaces.back()->setTexCoord(i, texCoords[i]);
    }

  } else if (surfaceType == SurfaceType::QUAD_SURFACE) {
    if (vertices.size() < 4) {
      throw std::runtime_error("There must be 4 vertices for a quad surface.");
    } else if (texCoords.size() < 4) {
      throw std::runtime_error(
          "There must be 4 texture coordinates for a quad surface.");
    }

    surfaces.push_back(new QuadSurface());

    for (int i = 0; i < 4; i++) {
      surfaces.back()->setVertex(i, vertices[i]);
      surfaces.back()->setTexCoord(i, texCoords[i]);
    }
  } else {
    ofLogFatalError("SurfaceManager") << "Attempt to add non-existing surface type";
    std::exit(EXIT_FAILURE);
  }
}

void SurfaceManager::addSurface(int surfaceType, BaseSource* newSource,
                                vector<ofVec2f> vertices,
                                vector<ofVec2f> texCoords) {
  if (surfaceType == SurfaceType::TRIANGLE_SURFACE) {
    if (vertices.size() < 3) {
      throw std::runtime_error(
          "There must be 3 vertices for a triangle surface.");
    } else if (texCoords.size() < 3) {
      throw std::runtime_error(
          "Thre must be 3 texture coordinates for a triangle surface.");
    }

    surfaces.push_back(new TriangleSurface());
    surfaces.back()->setSource(newSource);

    for (int i = 0; i < 3; i++) {
      surfaces.back()->setVertex(i, vertices[i]);
      surfaces.back()->setTexCoord(i, texCoords[i]);
    }

  } else if (surfaceType == SurfaceType::QUAD_SURFACE) {
    if (vertices.size() < 4) {
      throw std::runtime_error("There must be 4 vertices for a quad surface.");
    } else if (texCoords.size() < 4) {
      throw std::runtime_error(
          "Thre must be 4 texture coordinates for a quad surface.");
    }

    surfaces.push_back(new QuadSurface());
    surfaces.back()->setSource(newSource);

    for (int i = 0; i < 4; i++) {
      surfaces.back()->setVertex(i, vertices[i]);
      surfaces.back()->setTexCoord(i, texCoords[i]);
    }
  } else {
    ofLogFatalError("SurfaceManager") << "Attempt to add non-existing surface type";
    std::exit(EXIT_FAILURE);
  }
}

void SurfaceManager::removeSelectedSurface() {
  if (selectedSurface == NULL) {
    return;
  }
  for (int i = 0; i < surfaces.size(); i++) {
    if (surfaces[i] == selectedSurface) {
      delete surfaces[i];
      surfaces.erase(surfaces.begin() + i);
      selectedSurface = NULL;
      break;
    }
  }
}

void SurfaceManager::clear() {
  // delete all extra allocations from the heap
  while (surfaces.size()) {
    delete surfaces.back();
    surfaces.pop_back();
  }
}

void SurfaceManager::saveXmlSettings(string fileName) {
  // Exit if mediaServer not set
  if (mediaServer == NULL) {
    ofLogFatalError("SurfaceManager") << "Media server not set";
    std::exit(EXIT_FAILURE);
  }
  // We need a fresh copy of the xml settings object
  xmlSettings.clear();
  // Save surfaces
  xmlSettings.addTag("surfaces");
  xmlSettings.pushTag("surfaces");
  for (int i = 0; i < surfaces.size(); i++) {
    xmlSettings.addTag("surface");
    xmlSettings.pushTag("surface", i);
    BaseSurface* surface = surfaces[i];

    xmlSettings.addTag("vertices");
    xmlSettings.pushTag("vertices");
    vector<ofVec3f>* vertices = &surface->getVertices();
    for (int j = 0; j < vertices->size(); j++) {
      xmlSettings.addTag("vertex");
      xmlSettings.pushTag("vertex", j);
      ofVec3f* vertex = &(*vertices)[j];
      xmlSettings.addValue("x", vertex->x);
      xmlSettings.addValue("y", vertex->y);

      // we don't need z as it will be 0 anyways

      xmlSettings.popTag();  // vertex
    }
    xmlSettings.popTag();  // vertices

    xmlSettings.addTag("texCoords");
    xmlSettings.pushTag("texCoords");
    vector<ofVec2f>* texCoords = &surface->getTexCoords();
    for (int j = 0; j < texCoords->size(); j++) {
      xmlSettings.addTag("texCoord");
      xmlSettings.pushTag("texCoord", j);
      ofVec2f* texCoord = &(*texCoords)[j];
      xmlSettings.addValue("x", texCoord->x);
      xmlSettings.addValue("y", texCoord->y);
      xmlSettings.popTag();  // texCoord
    }
    xmlSettings.popTag();  // texCoords
    xmlSettings.addTag("source");
    xmlSettings.pushTag("source");
    string sourceTypeName = SourceType::GetSourceTypeName(surface->getSource()->getType());
    cout << "sourceTypeName: " << sourceTypeName << endl;
    xmlSettings.addValue("source-type", sourceTypeName);
    xmlSettings.addValue("source-name", surface->getSource()->getName());
    xmlSettings.popTag();  // source
    xmlSettings.popTag();  // surface
  }
  xmlSettings.popTag();  // surfaces
  xmlSettings.save(fileName);
}

void SurfaceManager::loadXmlSettings(string fileName) {
  // Exit if there is no media server
  if (mediaServer == NULL) {
    ofLogFatalError("SurfaceManager") << "Media server not set";
    std::exit(EXIT_FAILURE);
  }
  if (!xmlSettings.loadFile(fileName)) {
    ofLogWarning("SurfaceManager") << "Could not load XML settings";
    return;
  }
  if (!xmlSettings.tagExists("surfaces")) {
    ofLogWarning("SurfaceManager") << "XML settings is empty or has wrong markup";
    return;
  }

  xmlSettings.pushTag("surfaces");

  int numSurfaces = xmlSettings.getNumTags("surface");
  for (int i = 0; i < numSurfaces; i++) {
    xmlSettings.pushTag("surface", i);
    // attempt to load surface source
    xmlSettings.pushTag("source");
    string sourceType = xmlSettings.getValue("source-type", "");
    string sourceName = xmlSettings.getValue("source-name", "");
    BaseSource* source = NULL;
    if (sourceName != "" && sourceName != "none" && sourceType != "") {
      // Load source depending on type
      int typeEnum = SourceType::GetSourceTypeEnum(sourceType);
      // Construct full path
      string dir = mediaServer->getDefaultMediaDir(typeEnum);
      std::stringstream pathss;
      pathss << ofToDataPath(dir, true) << sourceName;
      string sourcePath = pathss.str();
      // Load media by using full path
      source = mediaServer->loadMedia(sourcePath, typeEnum);
    }
    xmlSettings.popTag();  // source
    xmlSettings.pushTag("vertices");
    vector<ofVec2f> vertices;
    int vertexCount = xmlSettings.getNumTags("vertex");
    // it's a triangle ?
    if (vertexCount == 3) {
      //ofLog(OF_LOG_NOTICE, "create Triangle");
      xmlSettings.pushTag("vertex", 0);
      vertices.push_back(ofVec2f(xmlSettings.getValue("x", 0.0f),
                                 xmlSettings.getValue("y", 0.0f)));
      xmlSettings.popTag();

      xmlSettings.pushTag("vertex", 1);
      vertices.push_back(ofVec2f(xmlSettings.getValue("x", 100.0f),
                                 xmlSettings.getValue("y", 0.0f)));
      xmlSettings.popTag();

      xmlSettings.pushTag("vertex", 2);
      vertices.push_back(ofVec2f(xmlSettings.getValue("x", 0.0f),
                                 xmlSettings.getValue("y", 100.0f)));
      xmlSettings.popTag();

      xmlSettings.popTag();  // vertices

      xmlSettings.pushTag("texCoords");

      vector<ofVec2f> texCoords;

      xmlSettings.pushTag("texCoord", 0);
      texCoords.push_back(ofVec2f(xmlSettings.getValue("x", 0.0f),
                                  xmlSettings.getValue("y", 0.0f)));
      xmlSettings.popTag();

      xmlSettings.pushTag("texCoord", 1);
      texCoords.push_back(ofVec2f(xmlSettings.getValue("x", 1.0f),
                                  xmlSettings.getValue("y", 0.0f)));
      xmlSettings.popTag();

      xmlSettings.pushTag("texCoord", 2);
      texCoords.push_back(ofVec2f(xmlSettings.getValue("x", 0.0f),
                                  xmlSettings.getValue("y", 1.0f)));
      xmlSettings.popTag();

      xmlSettings.popTag();  // texCoords

      // now we have variables sourceName and sourceTexture
      // by checking those we can use one or another addSurface method
      if (sourceName != "none" && source != NULL) {
        addSurface(SurfaceType::TRIANGLE_SURFACE, source, vertices,
                   texCoords);
      } else {
        addSurface(SurfaceType::TRIANGLE_SURFACE, vertices, texCoords);
      }
    }
    // it's a quad ?
    else if (vertexCount == 4)
    // if (surface-type == QUAD_SURFACE)
    {
      xmlSettings.pushTag("vertex", 0);
      vertices.push_back(ofVec2f(xmlSettings.getValue("x", 0.0f),
                                 xmlSettings.getValue("y", 0.0f)));
      xmlSettings.popTag();

      xmlSettings.pushTag("vertex", 1);
      vertices.push_back(ofVec2f(xmlSettings.getValue("x", 100.0f),
                                 xmlSettings.getValue("y", 0.0f)));
      xmlSettings.popTag();

      xmlSettings.pushTag("vertex", 2);
      vertices.push_back(ofVec2f(xmlSettings.getValue("x", 100.0f),
                                 xmlSettings.getValue("y", 100.0f)));
      xmlSettings.popTag();

      xmlSettings.pushTag("vertex", 3);
      vertices.push_back(ofVec2f(xmlSettings.getValue("x", 0.0f),
                                 xmlSettings.getValue("y", 100.0f)));
      xmlSettings.popTag();

      xmlSettings.popTag();  // vertices

      xmlSettings.pushTag("texCoords");

      vector<ofVec2f> texCoords;

      xmlSettings.pushTag("texCoord", 0);
      texCoords.push_back(ofVec2f(xmlSettings.getValue("x", 0.0f),
                                  xmlSettings.getValue("y", 0.0f)));
      xmlSettings.popTag();

      xmlSettings.pushTag("texCoord", 1);
      texCoords.push_back(ofVec2f(xmlSettings.getValue("x", 1.0f),
                                  xmlSettings.getValue("y", 0.0f)));
      xmlSettings.popTag();

      xmlSettings.pushTag("texCoord", 2);
      texCoords.push_back(ofVec2f(xmlSettings.getValue("x", 1.0f),
                                  xmlSettings.getValue("y", 1.0f)));
      xmlSettings.popTag();

      xmlSettings.pushTag("texCoord", 3);
      texCoords.push_back(ofVec2f(xmlSettings.getValue("x", 0.0f),
                                  xmlSettings.getValue("y", 1.0f)));
      xmlSettings.popTag();

      xmlSettings.popTag();  // texCoords

      // now we have variables sourceName and sourceTexture
      // by checking those we can use one or another addSurface method
      if (sourceName != "none" && source != NULL) {
        addSurface(SurfaceType::QUAD_SURFACE, source, vertices,
                   texCoords);
      } else {
        addSurface(SurfaceType::QUAD_SURFACE, vertices, texCoords);
      }
    }

    xmlSettings.popTag();  // surface
  }

  xmlSettings.popTag();  // surfaces
}
  
  void SurfaceManager::setMediaServer(MediaServer* newMediaServer) {
    mediaServer = newMediaServer;
  }

BaseSurface* SurfaceManager::selectSurface(int index) {
  if (index >= surfaces.size()) {
    throw std::runtime_error("Surface index out of bounds.");
  }

  selectedSurface = surfaces[index];

  // notify that a new surface has been selected
  ofSendMessage("surfaceSelected");
}

BaseSurface* SurfaceManager::getSelectedSurface() {
  return selectedSurface;
}

void SurfaceManager::deselectSurface() {
  selectedSurface = NULL;
}

BaseSurface* SurfaceManager::getSurface(int index) {
  if (index >= surfaces.size()) {
    throw std::runtime_error("Surface index out of bounds.");
    return NULL;
  }

  return surfaces[index];
}

int SurfaceManager::size() { return surfaces.size(); }
}
}

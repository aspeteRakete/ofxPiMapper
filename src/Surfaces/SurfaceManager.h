#pragma once

#include "BaseSurface.h"
#include "TriangleSurface.h"
#include "QuadSurface.h"
#include "SurfaceType.h"
#include "MediaServer.h"
#include "BaseSource.h"
#include "SourceType.h"

#include "ofEvents.h"
#include "ofxXmlSettings.h"

using namespace std;

namespace ofx {
namespace piMapper {
class SurfaceManager {
 public:
  SurfaceManager();
  ~SurfaceManager();

  void draw();
  void addSurface(int surfaceType);
  void addSurface(int surfaceType, BaseSource* newSource);
  void addSurface(int surfaceType, vector<ofVec2f> vertices,
                  vector<ofVec2f> texCoords);
  void addSurface(int surfaceType, BaseSource* newSource,
                  vector<ofVec2f> vertices, vector<ofVec2f> texCoords);
  void removeSelectedSurface();
  void clear();
  void saveXmlSettings(string fileName);
  void loadXmlSettings(string fileName);
  void setMediaServer(MediaServer* newMediaServer);

  BaseSurface* getSurface(int index);
  int size();
  BaseSurface* selectSurface(int index);
  BaseSurface* getSelectedSurface();
  void deselectSurface();

 private:
  std::vector<BaseSurface*> surfaces;
  BaseSurface* selectedSurface;
  ofxXmlSettings xmlSettings;
  MediaServer* mediaServer;
};
}
}
#ifndef H_OF_APP
#define H_OF_APP

#include "ofMain.h"
#include "ofxPiMapper.h"
#ifdef TARGET_OPENGLES
#include "ofxOMXPlayer.h"
#endif

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);

    void addRandomSurface();
    void addQuadSurface();
    void addSurface();
    void setFboAsTexture();

    ofImage image;
    ofxSurfaceManager surfaceManager;
    ofxSurfaceManagerGui gui;
    bool bShowInfo;
    ofFbo* fbo;
    vector<ofRectangle> rects;
    vector<float> rectSpeeds;
#ifdef TARGET_OPENGLES
    ofxOMXPlayer video;
#else
    ofVideoPlayer video;
#endif
};

#endif
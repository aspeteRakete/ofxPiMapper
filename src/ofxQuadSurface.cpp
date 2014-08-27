#include "ofxQuadSurface.h"

ofxQuadSurface::ofxQuadSurface() {
  cout << "ofxQuadSurface constructor." << endl;
  setup();
}

ofxQuadSurface::~ofxQuadSurface() {
  cout << "ofxQuadSurface destructor." << endl;
}

void ofxQuadSurface::setup() {
  // Create 4 points for the 2 triangles
  ofVec2f p1 = ofVec2f(0, 0);
  ofVec2f p2 = ofVec2f(0, ofGetHeight());
  ofVec2f p3 = ofVec2f(ofGetWidth(), ofGetHeight());
  ofVec2f p4 = ofVec2f(ofGetWidth(), 0);

  // Create 4 point for the texture coordinates
  ofVec2f t1 = ofVec2f(ofVec2f(0.0f, 0.0f));
  ofVec2f t2 = ofVec2f(ofVec2f(1.0f, 0.0f));
  ofVec2f t3 = ofVec2f(ofVec2f(1.0f, 1.0f));
  ofVec2f t4 = ofVec2f(ofVec2f(0.0f, 1.0f));

  setup(p1, p2, p3, p4, t1, t2, t3, t4, texture);
}

void ofxQuadSurface::setup(ofVec2f p1, ofVec2f p2, ofVec2f p3, ofVec2f p4,
                           ofVec2f t1, ofVec2f t2, ofVec2f t3, ofVec2f t4,
                           ofTexture *texturePtr) {
  // Assign texture
  texture = texturePtr;

  // Clear mesh
  mesh.clear();

  // Create a surface with the points
  mesh.addVertex(p1);
  mesh.addVertex(p2);
  mesh.addVertex(p3);
  mesh.addVertex(p4);

  // Add 2 triangles
  mesh.addTriangle(0, 2, 3);
  mesh.addTriangle(0, 1, 2);

  // Add texture coordinates
  mesh.addTexCoord(t1);
  mesh.addTexCoord(t2);
  mesh.addTexCoord(t3);
  mesh.addTexCoord(t4);
}

void ofxQuadSurface::draw() {
  // Perspective Warping with OpenGL Fixed Pipeline and q coordinates
  // see:
  // http://www.reedbeta.com/blog/2012/05/26/quadrilateral-interpolation-part-1/
  // for information on the technique
  // Pue OpenGL is used because the ofMesh sadly doesn't support ofVec4f as
  // texture coordinates.
  ofPushMatrix();

  // draw intersection point
  ofVec3f p0 = mesh.getVertex(0);
  ofVec3f p1 = mesh.getVertex(1);
  ofVec3f p2 = mesh.getVertex(2);
  ofVec3f p3 = mesh.getVertex(3);

  ofVec3f t0 = mesh.getTexCoord(0);
  ofVec3f t1 = mesh.getTexCoord(1);
  ofVec3f t2 = mesh.getTexCoord(2);
  ofVec3f t3 = mesh.getTexCoord(3);

  for (int i = 0; i < 4; i++) {
    ofDrawBitmapString(ofToString(i), mesh.getVertex(i));
  }

  // calculate intersection point
  ofPoint interSect;
  ofLineSegmentIntersection(ofPoint(p0.x, p0.y), ofPoint(p2.x, p2.y),
                            ofPoint(p1.x, p1.y), ofPoint(p3.x, p3.y),
                            interSect);
  ofVec3f interSecVec = ofVec3f(interSect.x, interSect.y, 0);

  // calculate distances to intersection point
  float d0 = interSecVec.distance(p0);
  float d1 = interSecVec.distance(p1);
  float d2 = interSecVec.distance(p2);
  float d3 = interSecVec.distance(p3);

  // define texture and vertex coords
  texture->bind();
  /*glBegin(GL_QUADS);
  {
    float q = (d0 + d2) / (d2);
    glTexCoord4f(t0.x, t0.y, 0, q);
    glVertex3f(p0.x, p0.y, p0.z);

    q = (d1 + d3) / (d3);
    glTexCoord4f(t1.x * q, t1.y, 0, q);
    glVertex3f(p1.x, p1.y, p1.z);

    q = (d2 + d0) / (d0);
    glTexCoord4f(t2.x * q, t2.y * q, 0, q);
    glVertex3f(p2.x, p2.y, p2.z);

    q = (d3 + d1) / (d1);
    glTexCoord4f(t3.x, t3.y * q, 0, q);
    glVertex3f(p3.x, p3.y, p3.z);
  }
  glEnd();*/
    GLfloat glverts[8];
    glVertexPointer(3, GL_FLOAT, 0, glverts);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    //fill in vertex positions with your data
    for (int i = 0; i < 3; i++) {
        glverts[i*3]   = mesh.getVertex(i).x;
        glverts[i*3+1] = mesh.getVertex(i).y;
        glverts[i*3+2] = 0;
    }
    
    glDrawArrays(GL_TRIANGLES, 0, 3);

  texture->unbind();
  ofPopMatrix();
}

void ofxQuadSurface::setVertex(int index, ofVec2f p) {
  if (index > 3) {
    ofLog() << "Vertex with this index does not exist: " << index << endl;
    return;
  }

  mesh.setVertex(index, p);
}

void ofxQuadSurface::setTexCoord(int index, ofVec2f t) {
  if (index > 3) {
    ofLog() << "Texture coordinate with this index does not exist: " << index
            << endl;
    return;
  }

  mesh.setTexCoord(index, t);
}

int ofxQuadSurface::getType() { return ofxSurfaceType::QUAD_SURFACE; }

bool ofxQuadSurface::hitTest(ofVec2f p) {
  // Construct ofPolyline from vertices
  ofPolyline line = getHitArea();

  if (line.inside(p.x, p.y)) {
    return true;
  } else {
    return false;
  }
}

ofVec2f ofxQuadSurface::getVertex(int index) {
  if (index > 3) {
    ofLog() << "Vertex with this index does not exist: " << index << endl;
    throw std::runtime_error("Vertex index out of bounds.");
  }

  ofVec3f vert = mesh.getVertex(index);
  return ofVec2f(vert.x, vert.y);
}

ofVec2f ofxQuadSurface::getTexCoord(int index) {
  if (index > 3) {
    throw std::runtime_error("Texture coordinate index out of bounds.");
  }

  return mesh.getTexCoord(index);
}

ofPolyline ofxQuadSurface::getHitArea() {
  ofPolyline line;
  line.addVertex(ofPoint(mesh.getVertex(0).x, mesh.getVertex(0).y));
  line.addVertex(ofPoint(mesh.getVertex(1).x, mesh.getVertex(1).y));
  line.addVertex(ofPoint(mesh.getVertex(2).x, mesh.getVertex(2).y));
  line.addVertex(ofPoint(mesh.getVertex(3).x, mesh.getVertex(3).y));
  line.close();

  return line;
}

ofPolyline ofxQuadSurface::getTextureHitArea() {
  ofPolyline line;
  vector<ofVec2f> &texCoords = mesh.getTexCoords();
  ofVec2f textureSize = ofVec2f(texture->getWidth(), texture->getHeight());
  for (int i = 0; i < texCoords.size(); i++) {
    line.addVertex(ofPoint(texCoords[i] * textureSize));
  }
  line.close();

  return line;
}

vector<ofVec3f> &ofxQuadSurface::getVertices() {
  // return only joint vertices
  return mesh.getVertices();
}

vector<ofVec2f> &ofxQuadSurface::getTexCoords() { return mesh.getTexCoords(); }
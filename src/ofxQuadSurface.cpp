#include "ofxQuadSurface.h"

ofxQuadSurface::ofxQuadSurface()
{
    cout << "ofxQuadSurface constructor." << endl;
    setup();
}

ofxQuadSurface::~ofxQuadSurface()
{
    cout << "ofxQuadSurface destructor." << endl;
}

void ofxQuadSurface::setup()
{
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
                           ofTexture* texturePtr)
{
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

void ofxQuadSurface::draw()
{
    // Perspective Warping with OpenGL Fixed Pipeline and q coordinates
    // see:
    // http://www.reedbeta.com/blog/2012/05/26/quadrilateral-interpolation-part-1/
    // for information on the technique
    // Pue OpenGL is used because the ofMesh sadly doesn't support ofVec4f as
    // texture coordinates.
    ofPushMatrix();
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

    GLfloat vertices[] = { mesh.getVertex(0).x, mesh.getVertex(0).y, 0, // top left corner
                           mesh.getVertex(1).x, mesh.getVertex(1).y, 0, // top right corner
                           mesh.getVertex(2).x, mesh.getVertex(2).y, 0, // bottom right corner
                           mesh.getVertex(3).x, mesh.getVertex(3).y, 0 }; // bottom left corner

    GLubyte indices[] = { 0, 1, 2, // first triangle (bottom left - top left - top right)
                          0, 2, 3 }; // second triangle (bottom left - top right - bottom right)

    float q0 = (d0 + d2) / (d2);
    float q1 = (d1 + d3) / (d3);
    float q2 = (d2 + d0) / (d0);
    float q3 = (d3 + d1) / (d1);

    GLfloat texCoordinates[] = {
        t0.x, t0.y, 0, q0,
        t1.x * q1, t1.y, 0, q1,
        t2.x * q2, t2.y * q2, 0, q2,
        t3.x, t3.y * q3, 0, q3
    };
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(4, GL_FLOAT, 0, texCoordinates);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    texture->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
    texture->unbind();
    ofPopMatrix();
}

void ofxQuadSurface::setVertex(int index, ofVec2f p)
{
    if (index > 3) {
        ofLog() << "Vertex with this index does not exist: " << index << endl;
        return;
    }

    mesh.setVertex(index, p);
}

void ofxQuadSurface::setTexCoord(int index, ofVec2f t)
{
    if (index > 3) {
        ofLog() << "Texture coordinate with this index does not exist: " << index
                << endl;
        return;
    }

    mesh.setTexCoord(index, t);
}

int ofxQuadSurface::getType() { return ofxSurfaceType::QUAD_SURFACE; }

bool ofxQuadSurface::hitTest(ofVec2f p)
{
    // Construct ofPolyline from vertices
    ofPolyline line = getHitArea();

    if (line.inside(p.x, p.y)) {
        return true;
    } else {
        return false;
    }
}

ofVec2f ofxQuadSurface::getVertex(int index)
{
    if (index > 3) {
        ofLog() << "Vertex with this index does not exist: " << index << endl;
        throw std::runtime_error("Vertex index out of bounds.");
    }

    ofVec3f vert = mesh.getVertex(index);
    return ofVec2f(vert.x, vert.y);
}

ofVec2f ofxQuadSurface::getTexCoord(int index)
{
    if (index > 3) {
        throw std::runtime_error("Texture coordinate index out of bounds.");
    }

    return mesh.getTexCoord(index);
}

ofPolyline ofxQuadSurface::getHitArea()
{
    ofPolyline line;
    line.addVertex(ofPoint(mesh.getVertex(0).x, mesh.getVertex(0).y));
    line.addVertex(ofPoint(mesh.getVertex(1).x, mesh.getVertex(1).y));
    line.addVertex(ofPoint(mesh.getVertex(2).x, mesh.getVertex(2).y));
    line.addVertex(ofPoint(mesh.getVertex(3).x, mesh.getVertex(3).y));
    line.close();

    return line;
}

ofPolyline ofxQuadSurface::getTextureHitArea()
{
    ofPolyline line;
    vector<ofVec2f>& texCoords = mesh.getTexCoords();
    ofVec2f textureSize = ofVec2f(texture->getWidth(), texture->getHeight());
    for (int i = 0; i < texCoords.size(); i++) {
        line.addVertex(ofPoint(texCoords[i] * textureSize));
    }
    line.close();

    return line;
}

vector<ofVec3f>& ofxQuadSurface::getVertices()
{
    // return only joint vertices
    return mesh.getVertices();
}

vector<ofVec2f>& ofxQuadSurface::getTexCoords() { return mesh.getTexCoords(); }
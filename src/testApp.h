#pragma once

#include "ofMain.h"
#include "FaceTracking.h"
#include "MyBox2D.h"
#include "ofxXmlSettings.h"
//#include "FaceMapper.h"
class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void exit();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);		
    
    
    MyBox2D box2d;
    FaceTracking faceTracking;
    int counter,numDigi;
    string path_to_save;
    vector <string>lastFileNames;
    ofxXmlSettings xml;
    
    ofImage loadedImage;
    
    bool bCapture;
    
    
    
};

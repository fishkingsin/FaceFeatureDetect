#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableAlphaBlending();
    
    // enable depth->video image calibration
    faceTracking.setup();
    
    
}

//--------------------------------------------------------------
void testApp::update(){
	faceTracking.update();
}


//--------------------------------------------------------------
void testApp::draw(){
    faceTracking.draw();
    gui.draw();
}

void testApp::exit() {
	faceTracking.exit();
    
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch(key)
    {
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    
}
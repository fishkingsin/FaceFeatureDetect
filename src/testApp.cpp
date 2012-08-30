#include "testApp.h"
#include "PlayState.h"
#include "EditState.h"
const static string settingFileName = "config.xml";
//--------------------------------------------------------------
void testApp::setup(){
    ofxXmlSettings xml  = stateMachine.getSharedData().xml;
    if(!xml.loadFile(settingFileName))
    {
        xml.addTag("DATA");
        if(xml.pushTag("DATA"))
        {
            xml.addValue("COUNTER", stateMachine.getSharedData().counter);
            xml.addValue("PATH", "./captures");
            xml.addValue("DIGI", 5);
            xml.popTag();
        }
        xml.saveFile(settingFileName);
    }
    else
    {
        if(xml.pushTag("DATA"))
        {
           stateMachine.getSharedData().counter = xml.getValue("COUNTER", 0);
            stateMachine.getSharedData().path_to_save = xml.getValue("PATH", "./captures");
            stateMachine.getSharedData().numDigi = xml.getValue("DIGI", 5);
            ofDirectory dir;
            if(dir.listDir(stateMachine.getSharedData().path_to_save)<1)
            {
                dir.createDirectory(stateMachine.getSharedData().path_to_save);
            }
            
            xml.popTag();
        }
    }
//    // setup shared data
//	stateMachine.getSharedData().counter = 0;
//	stateMachine.getSharedData().lastUpdate = ofGetElapsedTimeMillis();
//	stateMachine.getSharedData().font.loadFont("vag.ttf", 50);
	
	// initialise state machine
	stateMachine.addState(new PlayState());
	stateMachine.addState(new EditState());
	stateMachine.changeState("PlayState");
}

//--------------------------------------------------------------
void testApp::update(){
    
}

//--------------------------------------------------------------
void testApp::draw(){
    gui.draw();

}
void testApp::exit()
{
    if (stateMachine.getSharedData().xml.pushTag("DATA")) {
        stateMachine.getSharedData().xml.setValue("COUNTER", stateMachine.getSharedData().counter);
        stateMachine.getSharedData().xml.popTag();
        
        
    }
    stateMachine.getSharedData().xml.saveFile();
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    
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
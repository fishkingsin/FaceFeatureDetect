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
    ofxControlPanel::setBackgroundColor(simpleColor(30, 30, 60, 100));
	ofxControlPanel::setTextColor(simpleColor(240, 50, 50, 255));
    stateMachine.getSharedData().panel.setup(ofGetWidth(),ofGetHeight());
	stateMachine.getSharedData().panel.loadFont("MONACO.TTF", 8);		
	stateMachine.getSharedData().panel.addPanel("General", 4,false);
    
	ofxControlPanel::setBackgroundColor(simpleColor(60, 30, 30, 100));	
	stateMachine.getSharedData().panel.addPanel("FaceTracking", 5, false);
    
	ofxControlPanel::setBackgroundColor(simpleColor(70, 70, 30, 100));	
	stateMachine.getSharedData().panel.addPanel("FaceMapEdit", 4, false);
    
	ofxControlPanel::setBackgroundColor(simpleColor(30, 30, 30, 100));	
    

	
	// initialise state machine
	stateMachine.addState(new PlayState());
	stateMachine.addState(new EditState());
	stateMachine.changeState("PlayState");
    stateMachine.getSharedData().panel.loadSettings("settings.xml");
    stateMachine.getSharedData().panel.hide();
    
}

//--------------------------------------------------------------
void testApp::update(){
    
}

//--------------------------------------------------------------
void testApp::draw(){
    

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
    switch(key)
    {
        case OF_KEY_F1:
            stateMachine.changeState("PlayState");
            break;
        case OF_KEY_F2:
            stateMachine.changeState("EditState");
            break;
            case 's':
            stateMachine.getSharedData().panel.saveSettings();
            break;
    }
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
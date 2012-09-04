#include "testApp.h"
#include "IndexState.h"
#include "SelectPlayerState.h"
#include "PlayState.h"
#include "EditState.h"
const static string settingFileName = "config.xml";
//--------------------------------------------------------------
void testApp::setup(){
	ofEnableAlphaBlending();
    ofxXmlSettings xml  = stateMachine.getSharedData().xml;
    if(xml.loadFile(settingFileName))
	{
		if(xml.pushTag("DATA"))
		{
			stateMachine.getSharedData().counter = xml.getValue("COUNTER", 0);
			stateMachine.getSharedData().path_to_save = xml.getValue("CAPTURE_PATH", "./captures");
			stateMachine.getSharedData().numDigi = xml.getValue("DIGI", 5);
			ofDirectory dir;
			if(dir.listDir(stateMachine.getSharedData().path_to_save)<1)
			{
				dir.createDirectory(stateMachine.getSharedData().path_to_save);
			}
			
			xml.popTag();
		}
	}
	else
	{
		ofLog(OF_LOG_ERROR,"Faile to load "+ settingFileName);
	}
    
	//    // setup shared data
    ofxControlPanel::setBackgroundColor(simpleColor(30, 30, 60, 100));
	ofxControlPanel::setTextColor(simpleColor(240, 50, 50, 255));
    stateMachine.getSharedData().panel.setup(ofGetWidth(),ofGetHeight());
	stateMachine.getSharedData().panel.loadFont("MONACO.TTF", 8);		
	stateMachine.getSharedData().panel.addPanel("General", 4,false);
    
	ofxControlPanel::setBackgroundColor(simpleColor(60, 30, 30, 100));	
	stateMachine.getSharedData().panel.addPanel("FaceTracking", 5, false);
    ofxControlPanel::setBackgroundColor(simpleColor(60, 30, 30, 100));	
	stateMachine.getSharedData().panel.addPanel("FaceTracking0", 4, false);
    ofxControlPanel::setBackgroundColor(simpleColor(60, 30, 30, 100));	
	stateMachine.getSharedData().panel.addPanel("FaceTracking1", 4, false);
    
	ofxControlPanel::setBackgroundColor(simpleColor(70, 70, 30, 100));	
	stateMachine.getSharedData().panel.addPanel("FaceMapEdit", 4, false);
    
	ofxControlPanel::setBackgroundColor(simpleColor(30, 30, 30, 100));	
    //some dummy vars we will update to show the variable lister object
	appFrameRate	= ofGetFrameRate();
    stateMachine.getSharedData().panel.setWhichPanel("General");
    stateMachine.getSharedData().panel.setWhichColumn(0);
	stateMachine.getSharedData().panel.addChartPlotter("some chart", guiStatVarPointer("app fps", &appFrameRate, GUI_VAR_FLOAT, true, 2), 200, 50, 200, 5, 80);
    vector<string> loglevel;
    loglevel.push_back("OF_LOG_VERBOSE");
    loglevel.push_back("OF_LOG_NOTICE");
    loglevel.push_back("OF_LOG_WARNING");
    loglevel.push_back("OF_LOG_ERROR");
    loglevel.push_back("OF_LOG_FATAL_ERROR");
    loglevel.push_back("OF_LOG_SILENT");
    stateMachine.getSharedData().panel.addTextDropDown("LogLevel","LogLevel", 0, loglevel);
	
	// initialise state machine
	stateMachine.addState(new IndexState());
	stateMachine.addState(new SelectPlayerState());
	stateMachine.addState(new PlayState());
	stateMachine.addState(new EditState());
	stateMachine.changeState("PlayState");
    stateMachine.getSharedData().panel.loadSettings("settings.xml");
    stateMachine.getSharedData().panel.hide();
    stateMachine.getSharedData().numPlayer = 2;
    
}

//--------------------------------------------------------------
void testApp::update(){
	appFrameRate	= ofGetFrameRate();
    int loglevel = stateMachine.getSharedData().panel.getValueI("LogLevel");
    if(ofGetLogLevel()!=  loglevel  )
    {
        ofSetLogLevel((ofLogLevel)loglevel);
    }
	
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackgroundGradient(ofColor::black, ofColor(50,50,50));
}
void testApp::exit()
{
	//    if (stateMachine.getSharedData().xml.pushTag("DATA")) {
	//        stateMachine.getSharedData().xml.setValue("COUNTER", stateMachine.getSharedData().counter);
	//        stateMachine.getSharedData().xml.popTag();
	//        
	//        
	//    }
	//    stateMachine.getSharedData().xml.saveFile();
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
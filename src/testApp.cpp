#include "testApp.h"

const static string settingFileName = "config.xml";
//--------------------------------------------------------------
void testApp::setup(){
    if(!xml.loadFile(settingFileName))
    {
        xml.addTag("DATA");
        if(xml.pushTag("DATA"))
        {
            xml.addValue("COUNTER", counter);
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
            counter = xml.getValue("COUNTER", 0);
            path_to_save = xml.getValue("PATH", "./captures");
            numDigi = xml.getValue("DIGI", 5);
            ofDirectory dir;
            if(dir.listDir(path_to_save)<1)
            {
                dir.createDirectory(path_to_save);
            }
            
            xml.popTag();
        }
    }
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableAlphaBlending();
    
    // enable depth->video image calibration
    faceTracking.setup();
    box2d.setup();
    bCapture = false;
}

//--------------------------------------------------------------
void testApp::update(){
	faceTracking.update();
    box2d.update();
}


//--------------------------------------------------------------
void testApp::draw(){
    faceTracking.draw();
    
    faceTracking.drawFeatures();
    
    box2d.draw();
    if(bCapture)
    {
        bCapture = false;
                
        char imagename[1024];
        stringstream format;
                
        format <<"%0"<<numDigi<<"d."<<"jpg";
                
        sprintf(imagename, format.str().c_str(), counter);
        cout << imagename;
        faceTracking.savingFace(path_to_save+"/"+imagename);
        lastFileNames.push_back(path_to_save+"/"+imagename);
        counter++;  
        if (xml.pushTag("DATA")) {
            xml.setValue("COUNTER", counter);
            xml.popTag();
            
            
        }
        xml.saveFile(settingFileName);
        while(!lastFileNames.empty())
        {
            string file_name = lastFileNames.back();
//            FaceData faceData;
//            faceData.setup(file_name,"settings01.xml");
            //TO_DO load image and map the face on shoes;
            lastFileNames.pop_back();
        }
    }
    gui.draw();
}

void testApp::exit() {
    if (xml.pushTag("DATA")) {
        xml.setValue("COUNTER", counter);
        xml.popTag();
        

    }
    xml.saveFile(settingFileName);
	faceTracking.exit();
    
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch(key)
    {
            case 'c':
            bCapture = true;
            break;
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
//    CustomParticle p;
//	p.setPhysics(1.0, 0.5, 0.3);
//	p.setup(box2d.box2d.getWorld(), x, y, ofRandom(20, 60));
//	p.setupTheCustomData(&faceTracking.leftEye,&faceTracking.alphaMaskShader);
//    box2d.addParticle( p);
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
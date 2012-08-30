//
//  PlayState.cpp
//  FaceFeatureDetect
//
//  Created by james KONG on 30/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "PlayState.h"

string PlayState::getName()
{
	return "PlayState";
}
//--------------------------------------------------------------
void PlayState::setup(){
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableAlphaBlending();
    
    // enable depth->video image calibration
    faceTracking.setup();
    box2d.setup();
    bCapture = false;
}

//--------------------------------------------------------------
void PlayState::update(){
	faceTracking.update();
    box2d.update();
}


//--------------------------------------------------------------
void PlayState::draw(){
    faceTracking.draw();
    
    faceTracking.drawFeatures();
    
    box2d.draw();
    if(bCapture)
    {
        bCapture = false;
        
        char imagename[1024];
        stringstream format;
        
        format <<"%0"<<getSharedData().numDigi<<"d."<<"jpg";
        
        sprintf(imagename, format.str().c_str(), getSharedData().counter);
        cout << imagename;
        faceTracking.savingFace(getSharedData().path_to_save+"/"+imagename);
        getSharedData().lastFileNames.push_back(getSharedData().path_to_save+"/"+imagename);
        getSharedData().counter++;  
        if (getSharedData().xml.pushTag("DATA")) {
            getSharedData().xml.setValue("COUNTER", getSharedData().counter);
            getSharedData().xml.popTag();
            
            
        }
        getSharedData().xml.saveFile();
        while(!getSharedData().lastFileNames.empty())
        {
            string file_name = getSharedData().lastFileNames.back();
            FaceData faceData;
            faceData.setup(file_name,"face_profile/settings_a.xml");
            faceData.setup(file_name,"face_profile/settings_b.xml");
            faceData.setup(file_name,"face_profile/settings_c.xml");
            faceData.setup(file_name,"face_profile/settings_d.xml");
            faceData.setup(file_name,"face_profile/settings_e.xml");
            //TO_DO load image and map the face on shoes;
            getSharedData().lastFileNames.pop_back();
        }
    }
}

void PlayState::stateExit() {
    
	faceTracking.exit();
    
}
//--------------------------------------------------------------
void PlayState::keyPressed(int key){
    switch(key)
    {
        case 'c':
            bCapture = true;
            break;
    }
}

//--------------------------------------------------------------
void PlayState::keyReleased(int key){
    
}

//--------------------------------------------------------------
void PlayState::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void PlayState::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void PlayState::mousePressed(int x, int y, int button){
    //    CustomParticle p;
    //	p.setPhysics(1.0, 0.5, 0.3);
    //	p.setup(box2d.box2d.getWorld(), x, y, ofRandom(20, 60));
    //	p.setupTheCustomData(&faceTracking.leftEye,&faceTracking.alphaMaskShader);
    //    box2d.addParticle( p);
}

//--------------------------------------------------------------
void PlayState::mouseReleased(int x, int y, int button){
    
}


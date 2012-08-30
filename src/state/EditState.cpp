//
//  EditState.cpp
//  FaceFeatureDetect
//
//  Created by james KONG on 30/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "EditState.h"
//
//  EditState.cpp
//  FaceFeatureDetect
//
//  Created by james KONG on 30/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "EditState.h"
string EditState::getName()
{
	return "EditState";
}
//--------------------------------------------------------------
void EditState::setup(){
    ofDirectory dir;
    dir.allowExt("xml");
    int numTag = dir.listDir("face_profile");
    file = new string[numTag];
    for(int i = 0 ;i < numTag ; i++)
    {
        file[i] = dir.getPath(i);
    }
    prevSelection = -1;
    currSelection = 0;
    //gui.addComboBox("FaceProfile" , currSelection, numTag,file);
}

//--------------------------------------------------------------
void EditState::update(){
    if(currSelection!=prevSelection)
    {
        prevSelection  = currSelection;
        faceData.setup("face.jpg",file[currSelection]);
    }
}


//--------------------------------------------------------------
void EditState::draw(){
    faceData.draw();
}

void EditState::stateExit() {

    
}
//--------------------------------------------------------------
void EditState::keyPressed(int key){
    switch(key)
    {
        case ' ':
            if(!faceData.warpper.isActive())faceData.warpper.activate();
            else faceData.warpper.deactivate();
            break;
        case 's':
            faceData.saveSetting();
            break;
    }
}

//--------------------------------------------------------------
void EditState::keyReleased(int key){
    
}

//--------------------------------------------------------------
void EditState::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void EditState::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void EditState::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void EditState::mouseReleased(int x, int y, int button){
    
}



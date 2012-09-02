/*
 *  EditState.cpp
 *
 *  Copyright (c) 2012, James Kong, http://www.fishkingsin.com
 *  All rights reserved. 
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met: 
 *  
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 *  * Neither the name of 16b.it nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 *  POSSIBILITY OF SUCH DAMAGE. 
 *
 */

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
    //file = new string[numTag];
    for(int i = 0 ;i < numTag ; i++)
    {
        file.push_back(dir.getPath(i));
    }
    prevSelection = -1;
    currSelection = 0;
    getSharedData().panel.setWhichPanel("FaceMapEdit");
    getSharedData().panel.setWhichColumn(0);
    getSharedData().panel.addTextDropDown("FaceProfile","FaceProfile", currSelection, file);
    getSharedData().panel.addToggle("DrawWapper","DrawWapper",false);
    getSharedData().panel.addToggle("SaveWarpper","SaveWapper",false);
}

//--------------------------------------------------------------
void EditState::update(){
    currSelection =  getSharedData().panel.getValueI("FaceProfile");
    if(currSelection!=prevSelection)
    {
        prevSelection  = currSelection;
        faceData.setup("face.jpg","face.jpg",file[currSelection]);
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
        case '1':
            if(!faceData.warpper.isActive())faceData.warpper.activate();
            else faceData.warpper.deactivate();
            break;
        case '2':
            if(!faceData.warpper2.isActive())faceData.warpper2.activate();
            else faceData.warpper2.deactivate();
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



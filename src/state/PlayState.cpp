/*
 *  PlayState.cpp
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

#include "PlayState.h"
FaceFeature *feature[4];
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
        getSharedData().panel.setWhichPanel("FaceTracking");
    getSharedData().panel.setWhichColumn(0);
    getSharedData().panel.addSlider("minFaceAreaW",faceTracking.minFaceAreaW,1,512);
	getSharedData().panel.addSlider("minFaceAreaH",faceTracking.minFaceAreaH,1,512);
    getSharedData().panel.addSlider("faceOffsetX",faceTracking.faceOffset.x,1,512);
    getSharedData().panel.addSlider("faceOffsetY",faceTracking.faceOffset.y,1,512);
    getSharedData().panel.addSlider("faceOffsetW",faceTracking.faceOffset.width,1,512);
    getSharedData().panel.addSlider("faceOffsetH",faceTracking.faceOffset.height,1,512);
    
    feature[0] = &faceTracking.leftEye;
    feature[1] = &faceTracking.rightEye;
    feature[2] = &faceTracking.nose;
    feature[3] = &faceTracking.mouth;
    for(int i = 0 ; i < 4 ; i++)
    {
        getSharedData().panel.setWhichColumn(i+1) ;
        getSharedData().panel.addSlider(feature[i]->name+"minFeatureArea",feature[i]->minArea,1,BUFFER_SIZE);
    
        getSharedData().panel.addSlider(feature[i]->name+"ROIL_X",feature[i]->ROI.x,1,BUFFER_SIZE);
        getSharedData().panel.addSlider(feature[i]->name+"ROIL_W",feature[i]->ROI.width,1,BUFFER_SIZE);
        getSharedData().panel.addSlider(feature[i]->name+"ROIL_Y",feature[i]->ROI.y,1,BUFFER_SIZE);
        getSharedData().panel.addSlider(feature[i]->name+"ROIL_H",feature[i]->ROI.height,1,BUFFER_SIZE);
    
    //gui.addPage("OffSet");
        getSharedData().panel.addSlider( feature[i]->name+"offset.x",feature[i]->offset.x,-BUFFER_SIZE,BUFFER_SIZE);
        getSharedData().panel.addSlider( feature[i]->name+"offset.y",feature[i]->offset.y,-BUFFER_SIZE,BUFFER_SIZE);
        getSharedData().panel.addSlider(feature[i]->name+ "offset.width",feature[i]->offset.width,-BUFFER_SIZE,BUFFER_SIZE);
        getSharedData().panel.addSlider(feature[i]->name+ "offset.height",feature[i]->offset.height,-BUFFER_SIZE,BUFFER_SIZE);
    }

    box2d.setup();
    bCapture = false;
}

//--------------------------------------------------------------
void PlayState::update(){
    
    faceTracking.minFaceAreaW = getSharedData().panel.getValueF("minFaceAreaW");
    faceTracking.minFaceAreaH = getSharedData().panel.getValueF("minFaceAreaH");
    faceTracking.faceOffset.x = getSharedData().panel.getValueF("faceOffsetX");
    faceTracking.faceOffset.y = getSharedData().panel.getValueF("faceOffsetY");
    faceTracking.faceOffset.width = getSharedData().panel.getValueF("faceOffsetW");
    faceTracking.faceOffset.height = getSharedData().panel.getValueF("faceOffsetH");
    for(int i = 0 ; i < 4 ; i++)
    {
        feature[i]->minArea = getSharedData().panel.getValueF(feature[i]->name+"minFeatureArea");
        feature[i]->ROI.x = getSharedData().panel.getValueF(feature[i]->name+"ROIL_X");
        feature[i]->ROI.width = getSharedData().panel.getValueF(feature[i]->name+"ROIL_W");
        feature[i]->ROI.y = getSharedData().panel.getValueF(feature[i]->name+"ROIL_Y");
        feature[i]->ROI.height = getSharedData().panel.getValueF(feature[i]->name+"ROIL_H");
        feature[i]->offset.x = getSharedData().panel.getValueF(feature[i]->name+"offset.x");
        feature[i]->offset.y = getSharedData().panel.getValueF(feature[i]->name+"offset.y");
        feature[i]->offset.width = getSharedData().panel.getValueF(feature[i]->name+"offset.width");
        feature[i]->offset.height = getSharedData().panel.getValueF(feature[i]->name+"offset.height");
    
    }
    faceTracking.numPlayer = getSharedData().numPlayer;
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
            faceData.save();
            faceData.setup(file_name,"face_profile/settings_b.xml");
            faceData.save();
            faceData.setup(file_name,"face_profile/settings_c.xml");
            faceData.save();
            faceData.setup(file_name,"face_profile/settings_d.xml");
            faceData.save();
            faceData.setup(file_name,"face_profile/settings_e.xml");
            faceData.save();
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


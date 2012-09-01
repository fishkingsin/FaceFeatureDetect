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
    
//    ofSetLogLevel(OF_LOG_VERBOSE);

    
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
    for(int j = 0 ;j < 2 ; j++)
    {
        getSharedData().panel.setWhichPanel("FaceTracking"+ofToString(j));
        feature[0] = &faceTracking.leftEye[j];
        feature[1] = &faceTracking.rightEye[j];
        feature[2] = &faceTracking.nose[j];
        feature[3] = &faceTracking.mouth[j];
        for(int i = 0 ; i < 4 ; i++)
        {
            getSharedData().panel.setWhichColumn(i) ;
            getSharedData().panel.addSlider(feature[i]->name+"minFeatureArea"+ofToString(j),BUFFER_SIZE*0.5,1,BUFFER_SIZE);
            
            getSharedData().panel.addSlider(feature[i]->name+"ROIL_X"+ofToString(j),0,1,BUFFER_SIZE);
            getSharedData().panel.addSlider(feature[i]->name+"ROIL_W"+ofToString(j),BUFFER_SIZE,1,BUFFER_SIZE);
            getSharedData().panel.addSlider(feature[i]->name+"ROIL_Y"+ofToString(j),0,1,BUFFER_SIZE);
            getSharedData().panel.addSlider(feature[i]->name+"ROIL_H"+ofToString(j),BUFFER_SIZE,1,BUFFER_SIZE);
            
            //gui.addPage("OffSet");
            getSharedData().panel.addSlider( feature[i]->name+"offset.x"+ofToString(j),0,-BUFFER_SIZE,BUFFER_SIZE);
            getSharedData().panel.addSlider( feature[i]->name+"offset.y"+ofToString(j),0,-BUFFER_SIZE,BUFFER_SIZE);
            getSharedData().panel.addSlider(feature[i]->name+ "offset.width"+ofToString(j),0,-BUFFER_SIZE,BUFFER_SIZE);
            getSharedData().panel.addSlider(feature[i]->name+ "offset.height"+ofToString(j),0,-BUFFER_SIZE,BUFFER_SIZE);
        }
    }
    
    box2d.setup();
    bCapture = false;
    bBox2D = false;
}

//--------------------------------------------------------------
void PlayState::update(){
    if(!bBox2D)
    {
        faceTracking.minFaceAreaW = getSharedData().panel.getValueF("minFaceAreaW");
        faceTracking.minFaceAreaH = getSharedData().panel.getValueF("minFaceAreaH");
        faceTracking.faceOffset.x = getSharedData().panel.getValueF("faceOffsetX");
        faceTracking.faceOffset.y = getSharedData().panel.getValueF("faceOffsetY");
        faceTracking.faceOffset.width = getSharedData().panel.getValueF("faceOffsetW");
        faceTracking.faceOffset.height = getSharedData().panel.getValueF("faceOffsetH");
        for(int j = 0 ;j < 2 ; j++)
        {

            feature[0] = &faceTracking.leftEye[j];
            feature[1] = &faceTracking.rightEye[j];
            feature[2] = &faceTracking.nose[j];
            feature[3] = &faceTracking.mouth[j];
            for(int i = 0 ; i < 4 ; i++)
            {
                feature[i]->minArea = getSharedData().panel.getValueF(feature[i]->name+"minFeatureArea"+ofToString(j));
                feature[i]->ROI.x = getSharedData().panel.getValueF(feature[i]->name+"ROIL_X"+ofToString(j));
                feature[i]->ROI.width = getSharedData().panel.getValueF(feature[i]->name+"ROIL_W"+ofToString(j));
                feature[i]->ROI.y = getSharedData().panel.getValueF(feature[i]->name+"ROIL_Y"+ofToString(j));
                feature[i]->ROI.height = getSharedData().panel.getValueF(feature[i]->name+"ROIL_H"+ofToString(j));
                feature[i]->offset.x = getSharedData().panel.getValueF(feature[i]->name+"offset.x"+ofToString(j));
                feature[i]->offset.y = getSharedData().panel.getValueF(feature[i]->name+"offset.y"+ofToString(j));
                feature[i]->offset.width = getSharedData().panel.getValueF(feature[i]->name+"offset.width"+ofToString(j));
                feature[i]->offset.height = getSharedData().panel.getValueF(feature[i]->name+"offset.height"+ofToString(j));
                
            }
        }
        faceTracking.numPlayer = getSharedData().numPlayer;
        faceTracking.update();
    }
    else box2d.update();
}


//--------------------------------------------------------------
void PlayState::draw(){
    
    faceTracking.draw();
    
    //    faceTracking.drawFeatures(0);
    //    faceTracking.drawFeatures(1);
    
    if(bBox2D)box2d.draw();
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
        case 'b':
        {
            
            if(bBox2D)
            {
                if(faceTracking.facefinder.blobs.size()<2)
                {
                    for(int j = 0 ;j < 2 ; j++)
                    {
                        feature[0] = &faceTracking.leftEye[j];
                        feature[1] = &faceTracking.rightEye[j];
                        feature[2] = &faceTracking.nose[j];
                        feature[3] = &faceTracking.mouth[j];
                        for(int i = 0 ; i < faceTracking.facefinder.blobs.size() ;i++)
                        {
                            CustomParticle p;
                            p.setPhysics(1.0, 0.5, 0.3);
                            ofSetRectMode(OF_RECTMODE_CORNER);
                            ofRectangle rect =  faceTracking.faceRect[i];
                            float scaleX = rect.width/BUFFER_SIZE*1.0f;
                            float scaleY = rect.width/BUFFER_SIZE*1.0f;
                            float x = (rect.x+faceTracking.leftEye[i].rect.x)*scaleX;
                            float y = (rect.y+faceTracking.leftEye[i].rect.y)*scaleY;
                            float w = faceTracking.leftEye[i].rect.width*scaleX;
                            float h = faceTracking.leftEye[i].rect.height*scaleY;
                            p.setup(box2d.box2d.getWorld(),x,y,w,h);
                            p.setupTheCustomData(&faceTracking.leftEye[i],&faceTracking.alphaMaskShader,scaleX,scaleY);
                            box2d.addParticle( p);
                        }
                    }
                }
            }
        }
            break;
        case OF_KEY_RETURN:
            bBox2D = !bBox2D;
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
    
    
}

//--------------------------------------------------------------
void PlayState::mouseReleased(int x, int y, int button){
    
}


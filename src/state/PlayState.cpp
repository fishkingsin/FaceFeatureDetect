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
	image.loadImage("images/playstate.png");
    faceTracking.setup();
    getSharedData().panel.setWhichPanel("FaceTracking");
    getSharedData().panel.setWhichColumn(0);
    getSharedData().panel.addSlider("minFaceAreaW",faceTracking.minFaceAreaW,1,512);
	getSharedData().panel.addSlider("minFaceAreaH",faceTracking.minFaceAreaH,1,512);
    getSharedData().panel.addSlider("faceOffsetX",faceTracking.faceOffset.x,1,512);
    getSharedData().panel.addSlider("faceOffsetY",faceTracking.faceOffset.y,1,512);
    getSharedData().panel.addSlider("faceOffsetW",faceTracking.faceOffset.width,1,512);
    getSharedData().panel.addSlider("faceOffsetH",faceTracking.faceOffset.height,1,512);
	ratio = (ofGetHeight()*1.0f)/(camH*1.0f);
	
	capturedScreen.allocate(camW,camH);
	screenWidth = camW*ratio;

	screenHeight = ofGetHeight();

    for(int j = 0 ;j < MAX_PLAYER ; j++)
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
    countDown.setup(getSharedData().xml.getValue("COUNTDOWN",1)*1000);
    ofAddListener(countDown.COUNTER_REACHED,this,&PlayState::Shot);
    ofAddListener(countDown.COMPLETE,this,&PlayState::completeShot);
	
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
        for(int j = 0 ;j < MAX_PLAYER ; j++)
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
        
        faceTracking.update(bCapture);
    }
    else box2d.update();
    
    if(bCapture)
    {
        lastCapture.setFromPixels(faceTracking.getPixels(),camW,camH,OF_IMAGE_COLOR);
        bCapture = false;
        for(int player = 0 ; player<getSharedData().numPlayer ; player++)
		{
			char imagename[1024];
			stringstream format;
			stringstream format2;
			
			if(player==1)format <<"%0"<<getSharedData().numDigi<<"d2_.jpg";
			else format <<"%0"<<getSharedData().numDigi<<"d.jpg";
			ofLog(OF_LOG_VERBOSE, "format ",format.str().data() );
			sprintf(imagename, format.str().c_str(), getSharedData().counter);
			char code[getSharedData().numDigi];
			
			format2 <<"%0"<<getSharedData().numDigi<<"d";
			sprintf(code, format2.str().c_str(), getSharedData().counter);
			
			getSharedData().lastCode = code;
			ofLog(OF_LOG_VERBOSE, "Exported file name = ",imagename );
			ofLog(OF_LOG_VERBOSE, "Exported code = "+getSharedData().lastCode);
			faceTracking.savingFace(player,getSharedData().path_to_save+"/"+imagename);
			getSharedData().lastFileNames.push_back(getSharedData().path_to_save+"/"+imagename);
		}
        getSharedData().counter++;  
        if (getSharedData().xml.pushTag("DATA")) {
			getSharedData().xml.setValue("COUNTER", getSharedData().counter);
			getSharedData().xml.popTag();
		
		
        }
        getSharedData().xml.saveFile();
        if(getSharedData().numPlayer==1)
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
		else
		{
			string file_name2 = getSharedData().lastFileNames.back();
			getSharedData().lastFileNames.pop_back();
			string file_name = getSharedData().lastFileNames.back();
			getSharedData().lastFileNames.pop_back();
            DoubleFaceData faceData;
            faceData.setup(file_name, file_name2, "face_profile/settings_a.xml");
            faceData.save();
            faceData.setup(file_name, file_name2, "face_profile/settings_b.xml");
            faceData.save();
            faceData.setup(file_name, file_name2, "face_profile/settings_c.xml");
            faceData.save();
            faceData.setup(file_name, file_name2, "face_profile/settings_d.xml");
            faceData.save();
            faceData.setup(file_name, file_name2, "face_profile/settings_e.xml");
            faceData.save();
            //TO_DO load image and map the face on shoes;
            //getSharedData().lastFileNames.pop_back();
		}
        
    }
	
}


//--------------------------------------------------------------
void PlayState::draw(){
    capturedScreen.begin();
	ofClearAlpha();
    if(!bBox2D)
    {
        faceTracking.draw();
    }
    if(lastCapture.isAllocated())lastCapture.draw(0,0);
    if(bBox2D)
    {
		
        
        faceTracking.drawFeaturesBlur(0);
        faceTracking.drawFeaturesBlur(1);
        box2d.draw();
    }
    
	capturedScreen.end();
	capturedScreen.draw(0.5*(ofGetWidth()-screenWidth),0,screenWidth,screenHeight);
	image.draw(0,0);
	countDown.draw(ofGetWidth()/2-100,0, 200,200);
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
                //TO-DO add clear box2d body and patricle when add new set of face feature
                //****************************************
				box2d.clear();
                //****************************************
				
				for(int i = 0 ; i < faceTracking.facefinder.blobs.size() ;i++)
				{
					if(i<= MAX_PLAYER)
					{
                        feature[0] = &faceTracking.leftEye[i];
                        feature[1] = &faceTracking.rightEye[i];
                        feature[2] = &faceTracking.nose[i];
                        feature[3] = &faceTracking.mouth[i];
                        ofRectangle rect =  faceTracking.faceRect[i];
                        for(int j = 0 ; j < 4 ; j++)
                        {
                            CustomParticle p;
                            //float density, float bounce, float friction
                            p.setPhysics(0.3, 0.7, 0.7);
                            
                            float scaleX = rect.width/BUFFER_SIZE*1.0f;
                            float scaleY = rect.height/BUFFER_SIZE*1.0f;
                            float x = rect.x+(feature[j]->rect.x*scaleX);
                            float y = rect.y+(feature[j]->rect.y*scaleY);
                            float w = feature[j]->rect.width*scaleX;
                            float h = feature[j]->rect.height*scaleY;
                            p.setup(box2d.box2d[i].getWorld(),x+(w*0.5)*0.5,y+(h*0.5)*0.5,w*0.5*0.5);
                            p.setupTheCustomData(feature[j],&faceTracking.alphaMaskShader,scaleX,scaleY);
                            box2d.addParticle( p);
                        }
						
                    }
                    
                }
                faceTracking.clear();
            }
        }
            break;
        case OF_KEY_RETURN:
            //bBox2D = !bBox2D;
            countDown.start();
			
            break;
        case OF_KEY_BACKSPACE:
			lastCapture.clear();
            bBox2D = false;
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

void PlayState::Shot(ofEventArgs &arg)
{
    ofLog(OF_LOG_VERBOSE,"Shot");
    bCapture = true;
    
}
void PlayState::completeShot(ofEventArgs &arg)
{
    ofLog(OF_LOG_VERBOSE,"completeShot");
    bBox2D = true;
    keyPressed('b');
    
}

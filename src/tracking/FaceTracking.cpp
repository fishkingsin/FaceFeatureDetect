/*
 *  FaceTracking.cpp
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

#include "FaceTracking.h"

void FaceTracking::setup()
{
#ifdef _USE_IMAGE
    faces.loadImage("faces.jpg");
#else
#ifdef _USE_LIVE_VIDEO
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(camW,camH);
#else
    vidPlayer.loadMovie("faces.mov");
    vidPlayer.play();
#endif
#endif
    colorImg.allocate(camW,camH);
    grayImage.allocate(camW,camH);
    
    
    
	facefinder.setup("haarcascade_frontalface_alt2.xml");
    
    
    colorImgFace.allocate(BUFFER_SIZE,BUFFER_SIZE);
	grayImageFace.allocate(BUFFER_SIZE,BUFFER_SIZE);
    
    
    
    //    ofxBlur *blurs[4];
    //    blurs[0] = &blurLEye;
    //    blurs[1] = &blurREye;
    //    blurs[2] = &blurNose;
    //    blurs[3] = &blurMouth;
    for(int i = 0 ; i < 4 ; i++)
    {
        if(i==0 || i==1 || i==3)
        {
            blurs[i].setup(64,48, 4, 0.2, 4);
        }
        else 
        {
            blurs[i].setup(48,64, 4, 0.2, 4);
        }
        blurs[i].setScale(4);
        blurs[i].setRotation(0);
    }
    for(int i = 0 ; i  < MAX_PLAYER ; i++)
    {
        faceBuffer[i].allocate(BUFFER_SIZE,BUFFER_SIZE,GL_RGB);
        facePixels[i].allocate(faceBuffer[i].getWidth(),faceBuffer[i].getHeight(),OF_IMAGE_COLOR);
        faceBuffer[i].begin();
        ofClear(0,0,0,255);
        faceBuffer[i].end();
        normFace[i].addVertex(ofVec2f(0, 0));
        normFace[i].addVertex(ofVec2f(faceBuffer[i].getWidth(), 0));
        normFace[i].addVertex(ofVec2f(faceBuffer[i].getWidth(), faceBuffer[i].getHeight()));
        normFace[i].addVertex(ofVec2f(0, faceBuffer[i].getHeight()));
        normFace[i].addTexCoord(0);
        normFace[i].addTexCoord(0);
        normFace[i].addTexCoord(0);
        normFace[i].addTexCoord(0);
        normFace[i].setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		
		
		
		
		
		
		leftEye[i].setup("lefteye","haarcascade_mcs_lefteye.xml","leftEyeMask.png",64,48);
		rightEye[i].setup("righteye","haarcascade_mcs_righteye.xml","rightEyeMask.png",64,48);
		nose[i].setup("nose","haarcascade_mcs_nose.xml","noseMask.png",48,64);
		mouth[i].setup("mouth","haarcascade_mcs_mouth.xml","mouthMask.png",64,48);
    }
	
    
    // There are 3 of ways of loading a shader:
    //
    //  1 - Using just the name of the shader and ledding ofShader look for .frag and .vert: 
    //      Ex.: shader.load( "myShader");
    //
    //  2 - Giving the right file names for each one: 
    //      Ex.: shader.load( "myShader.vert","myShader.frag");
    //
    //  3 - And the third one it´s passing the shader programa on a single string;
    //
    string shaderProgram = "#version 120\n \
    #extension GL_ARB_texture_rectangle : enable\n \
    \
    uniform sampler2DRect tex0;\
    uniform sampler2DRect maskTex;\
    \
    void main (void){\
    vec2 pos = gl_TexCoord[0].st;\
    \
    vec3 src = texture2DRect(tex0, pos).rgb;\
    float mask = texture2DRect(maskTex, pos).r;\
    \
    gl_FragColor = vec4( src , mask);\
    }";
    alphaMaskShader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
    alphaMaskShader.linkProgram();
    
    
}
void FaceTracking::update(bool bTrack)
{
    //bool bNewFrame = false;
    
    {
#ifdef _USE_IMAGE
		// bNewFrame = true;
#else
#ifdef _USE_LIVE_VIDEO
        vidGrabber.update();
        //bNewFrame = vidGrabber.isFrameNew();
#else
        vidPlayer.idleMovie();
        //bNewFrame = vidPlayer.isFrameNew();
#endif
#endif  
        if (bTrack)
        {
#ifdef _USE_IMAGE
			
            colorImg.setFromPixels(faces.getPixels(), camW,camH);
			
            grayImage = colorImg;
            
            processTracking(0,0,camW,camH,faces.getTextureReference());
#else
#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels(), camW,camH);
#else
            colorImg.setFromPixels(vidPlayer.getPixels(), camW,camH);
#endif
			
            grayImage = colorImg;
            
            processTracking(0,0,camW,camH,vidGrabber.getTextureReference());
#endif
        }
        
    }
}
void FaceTracking::savingFace(int nface , string fn)
{
    
    ofSaveImage(facePixels[nface], fn);
}

void FaceTracking::draw()
{
    ofSetColor(ofColor::white);
    {
#ifdef _USE_IMAGE
        faces.draw(0,0);
#else
        vidGrabber.draw(0,0);
#endif
		
        
		//        ofPushMatrix();
		//        drawFeaturesBlur(0);
		//        drawFeaturesBlur(1);
		//        ofPopMatrix();
        //drawFeatures();
        
        
        
        
        
    }
    if(ofGetLogLevel()==OF_LOG_VERBOSE)
    {
        ofPushMatrix();
        ofPushStyle();
        ofNoFill();
        for(int i = 0 ; i < facefinder.blobs.size() ; i++)
        {
            
            ofRectangle cur = facefinder.blobs[i].boundingRect;
            ofRect(cur.x,cur.y,cur.width,cur.height);
        }
        ofPopStyle();
        ofPopMatrix();
		
        ofPushMatrix();
        ofTranslate(camW,0);
        {
            faceBuffer[0].draw(0,0);
            faceBuffer[1].draw(0,faceBuffer[0].getHeight());
            
            ofPushMatrix();
            ofTranslate(0,camH*0.5f);
            //drawFeatures();
            
            ofPopMatrix();
            
            drawMarkers(0);
            ofPushMatrix();
            ofTranslate(0, BUFFER_SIZE);
            drawMarkers(1);
            
            ofPopMatrix();
            
        }
        ofPopMatrix();
    }
	//    ofPushStyle();
	//    ofSetColor(ofColor::black);
	//    ostringstream os;
	//    os <<"minArea : "+ofToString(minArea) <<endl ;
	//    if(facefinder.blobs.size()>0) os << "face detect OK"<<endl;
	//    else os << "no face detected"<<endl;
	//    os << "spacebar toggle gui" <<endl;
	//    ofDrawBitmapString(os.str(),10,ofGetHeight()-50);
	//    ofPopStyle();
}
void FaceTracking::drawFeatures(int i)
{
    leftEye[i].drawEffect(alphaMaskShader);
    rightEye[i].drawEffect(alphaMaskShader);
    nose[i].drawEffect(alphaMaskShader);
    mouth[i].drawEffect(alphaMaskShader);
    
}
void FaceTracking::drawFeaturesBlur(int i)
{
    ofPushMatrix();
    ofTranslate(faceRect[i].x, faceRect[i].y);
    ofScale(faceRect[i].width/BUFFER_SIZE*1.0f, faceRect[i].height/BUFFER_SIZE*1.0f);
    leftEye[i].drawEffect(alphaMaskShader,&blurs[0]);
    rightEye[i].drawEffect(alphaMaskShader,&blurs[1]);
    nose[i].drawEffect(alphaMaskShader,&blurs[2]);
    mouth[i].drawEffect(alphaMaskShader,&blurs[3]);
    ofPopMatrix();
    //        blurs[0].begin();
    //        {
    //            leftEye.draw(0,0);
    //        }
    //        blurs[0].end();
    //        glPushMatrix();
    //        glTranslatef(leftEye.rect.x,leftEye.rect.y,0);
    //        {
    //            alphaMaskShader.begin();
    //            alphaMaskShader.setUniformTexture("maskTex", leftEye.mask, 1 );
    //            
    //
    //            blurs[0].draw(0,0,leftEye.rect.width,leftEye.rect.height);
    //            
    //            alphaMaskShader.end();
    //        }
    //        glPopMatrix();
}
void FaceTracking::drawMarkers(int i)
{
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::yellow);
    leftEye[i].drawRect();
    
    ofSetColor(ofColor::cyan);
    rightEye[i].drawRect();
    ofSetColor(ofColor::blue);
    nose[i].drawRect();
    ofSetColor(ofColor::gray);
    mouth[i].drawRect();
    ofPopStyle();
}
void FaceTracking::clear()
{
	
}
void FaceTracking::processTracking(int x, int y , int w, int h , ofTexture &tex)
{
    facefinder.findHaarObjects(grayImage,x,y,w,h,minFaceAreaW,minFaceAreaH);
	///    if(facefinder.blobs.size()<=2 && facefinder.blobs.size()>0)
    {
        for(int i = 0 ; i  < facefinder.blobs.size() ; i++)
        {
			if(i<MAX_PLAYER)
			{
				ofRectangle cur = facefinder.blobs[i].boundingRect;
				faceRect[i].x = cur.x+faceOffset.x;
				faceRect[i].y = cur.y+faceOffset.y;
				faceRect[i].width = cur.width+faceOffset.width;
				faceRect[i].height = cur.height+faceOffset.height;
				
				normFace[i].setTexCoord(0,ofVec2f(faceRect[i].x, faceRect[i].y));
				normFace[i].setTexCoord(1,ofVec2f(faceRect[i].x+faceRect[i].width, faceRect[i].y));
				normFace[i].setTexCoord(2,ofVec2f(faceRect[i].x+faceRect[i].width, faceRect[i].y+faceRect[i].height));
				normFace[i].setTexCoord(3,ofVec2f(faceRect[i].x, faceRect[i].y+faceRect[i].height));
				
				faceBuffer[i].begin();	
				tex.bind();
				normFace[i].draw();
				tex.unbind();
				faceBuffer[i].end();
				
				faceBuffer[i].readToPixels(facePixels[i]);
				colorImgFace.setFromPixels(facePixels[i].getPixels(),BUFFER_SIZE,BUFFER_SIZE);
				grayImageFace = colorImgFace;
				
				leftEye[i].update(grayImageFace,faceBuffer[i].getTextureReference());
				rightEye[i].update(grayImageFace,faceBuffer[i].getTextureReference());
				nose[i].update(grayImageFace,faceBuffer[i].getTextureReference());
				mouth[i].update(grayImageFace,faceBuffer[i].getTextureReference());
			}
		}
        
    }
}
void FaceTracking::exit()
{
	
}

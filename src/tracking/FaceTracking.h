/*
 *  FaceTracking.h
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

#ifndef FaceFeatureDetect_FaceTracking_h
#define FaceFeatureDetect_FaceTracking_h
#include "ofxCvHaarFinder.h"
#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
//#define _USE_IMAGE		// uncomment this to use a live camera

//#define USE_QTKIT
#ifdef USE_QTKIT
#include "ofxQTKitVideoGrabber.h"
#endif
// otherwise, we'll use a movie file
//#include "ofxSimpleGuiToo.h"

#include "ofxBlur.h"

#define camW 640
#define camH 480
#define BUFFER_SIZE 256
#define QUARTER_SIZE 0.25
#define MAX_PLAYER 2
class FeatureData
{
public:
    FeatureData()
    {
        
    }
    FeatureData(const FeatureData & mom)
    {
        
    }
    void  operator = ( FeatureData &data )
    {
		
        buffer = data.buffer;
        mask = data.mask;
        rect = data.rect;
    }
    ofFbo buffer;    
    ofFbo mask;
    ofRectangle rect;
    
};
class FaceFeature
{
public:
    void setup(string feature_name,string haar_path,string mask_path, int w, int h)
    {
        finder.setup(haar_path);
        
        ROI.set(ofRectangle(0,QUARTER_SIZE,BUFFER_SIZE,BUFFER_SIZE*QUARTER_SIZE));
        
        
        buffer.allocate(w,h,GL_RGBA);
        
        buffer.begin();
        ofClear(0,0,0,255);
        buffer.end();
        norm.addVertex(ofVec2f(0, 0));
        norm.addVertex(ofVec2f(buffer.getWidth(), 0));
        norm.addVertex(ofVec2f(buffer.getWidth(), buffer.getHeight()));
        norm.addVertex(ofVec2f(0, buffer.getHeight()));
        norm.addTexCoord(0);
        norm.addTexCoord(0);
        norm.addTexCoord(0);
        norm.addTexCoord(0);
        norm.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        
        
        
        name = feature_name;
		
        ofEnableArbTex();
        ofImage maskImage;
        
        maskImage.loadImage(mask_path);
        
        mask.allocate(maskImage.getWidth(),maskImage.getHeight(),GL_RGBA);    
        
        
        mask.begin();
        ofClear(0, 0, 0, 0);
        maskImage.draw(0,0);
        mask.end();
    }
    
    void update(ofxCvGrayscaleImage &grayImage, ofTexture &tex)
    {
        if(finder.findHaarObjects(grayImage,ROI,minArea,minArea)>0)
        {
            ofEnableAlphaBlending();
            ofRectangle curr = finder.blobs[0].boundingRect;
            rect.x = curr.x+offset.x;
            rect.y = curr.y+offset.y;
            rect.width = curr.width+offset.width;
            rect.height = curr.height+offset.height;
            //ofRect(rect.x, rect.y, rect.width, rect.height);
            
            norm.setTexCoord(0,ofVec2f(rect.x, rect.y));
            norm.setTexCoord(1,ofVec2f(rect.x+rect.width, rect.y));
            norm.setTexCoord(2,ofVec2f(rect.x+rect.width, rect.y+rect.height));
            norm.setTexCoord(3,ofVec2f(rect.x, rect.y+rect.height));
            
            buffer.begin();	
            ofClear(0, 0, 0, 0);
            
            tex.bind();
            norm.draw();
            tex.unbind();
            
            buffer.end();
        }
        else
        {
			
            buffer.begin();	
            ofClear(0, 0, 0, 0);
            
            tex.bind();
            norm.draw();
            tex.unbind();
            
            buffer.end();
        }
        
        //*feature.buffer = *buffer; 
        
    }
    //not yet finish
	//    FeatureData & getFeatureData()
	//    {
	//        FeatureData &feature = _feature;
	//        feature.buffer = buffer;
	//        feature.mask = mask;
	//        feature.rect = rect;
	//        return feature;
	//    }
    void draw()
    {
        buffer.draw(rect.x,rect.y,rect.width,rect.height);
    }
    void draw(int x,int y)
    {
        buffer.draw(x,y);
    }
    void draw(int x,int y,int w,int h)
    {
        buffer.draw(x,y,w,h);
    }
    void drawRect()
    {
        ofRect(ROI.x, ROI.y, ROI.width, ROI.height);
        
        ofRect(rect.x, rect.y, rect.width, rect.height);
        ofDrawBitmapString(name,rect.x, rect.y);
    }
    void drawEffect(ofShader &alpha,ofxBlur *blur = NULL , float scale = 1.0f)
    {
        if(blur!=NULL)
        {
            blur->begin();
            {
				ofClearAlpha();
                draw(0,0);
            }
            blur->end();
            glPushMatrix();
            glTranslatef(rect.x,rect.y,0);
            glScalef(scale,scale,1);
            {
                alpha.begin();
                alpha.setUniformTexture("maskTex", mask, 1 );
                
                
                blur->draw(0,0,rect.width,rect.height);
                
                alpha.end();
            }
            glPopMatrix();
        }
        else
        {
            alpha.begin();
            alpha.setUniformTexture("maskTex", mask, 1 );
            
            glPushMatrix();
            //glTranslatef(rect.x,rect.y,0);
            //glTranslatef(rect.x,rect.y,0);
			//            glScalef(scale,scale,1);
            {
                draw(0,0,rect.width,rect.height);
            }
            glPopMatrix();
            alpha.end();
        }
    }
    string name;
    ofxCvHaarFinder finder;
    ofFbo buffer;
    ofMesh norm;    
    ofFbo mask;
    ofRectangle rect;
    ofRectangle offset;
    ofRectangle	ROI;
    
    int minArea;
};

class FaceTracking
{
public:
    void setup();
    void update(bool bTrack = false);
    void draw();
    void exit();
    void processTracking(int x, int y , int w, int h , ofTexture& tex);
    void savingFace(int nface, string fn);
    void drawFeatures(int i);
    void drawFeaturesBlur(int i);
    void drawMarkers(int i);
    void clear();
    unsigned char* getPixels()
    {
#ifdef _USE_IMAGE
        return faces.getPixels();
#else
#ifdef _USE_LIVE_VIDEO
#ifdef USE_QTKIT
        return vidGrabber.getPixels();
#else
        return vidGrabber.getPixels();
#endif
#else
        return vidPlayer.getPixels();
#endif
#endif 
    }
#ifdef _USE_IMAGE
    ofImage faces;
#else
#ifdef _USE_LIVE_VIDEO
#ifdef USE_QTKIT
    ofxQTKitVideoGrabber vidGrabber;
#else
    ofVideoGrabber 		vidGrabber;
#endif
#else
    ofVideoPlayer 		vidPlayer;
#endif
#endif
    ofxCvHaarFinder facefinder;
    ofRectangle       faceOffset;
    
    
    ofxCvColorImage			colorImg;
    ofxCvColorImage			colorImgFace;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayImageFace;
	
    ofFbo faceBuffer[MAX_PLAYER];
    ofMesh normFace[MAX_PLAYER];
    ofPixels facePixels[MAX_PLAYER];
    ofRectangle faceRect[MAX_PLAYER];
    
    FaceFeature leftEye[MAX_PLAYER];
    FaceFeature rightEye[MAX_PLAYER];
    FaceFeature nose[MAX_PLAYER];
    FaceFeature mouth[MAX_PLAYER];
    
    ofShader alphaMaskBlurShader;
    
    ofxBlur blurs[4];
    
    ofShader alphaMaskShader;
    
    int minArea ,
	minFaceAreaW ,
	minFaceAreaH ;
};


#endif

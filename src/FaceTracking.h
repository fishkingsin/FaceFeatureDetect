//
//  FaceTracking.h
//  FaceFeatureDetect
//
//  Created by james KONG on 29/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef FaceFeatureDetect_FaceTracking_h
#define FaceFeatureDetect_FaceTracking_h
#include "ofxCvHaarFinder.h"
#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
// otherwise, we'll use a movie file
#include "ofxSimpleGuiToo.h"
#include "ofxKinect.h"
#include "ofxBlur.h"
#ifdef USE_FACETRACKER
#include "ofxFraceTracker.h"
#endif

#define camW 640
#define camH 480
#define BUFFER_SIZE 256
#define QUARTER_SIZE 0.25
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
        gui.addPage(feature_name+"Extract");
        gui.addSlider("minFeatureArea",minArea,1,BUFFER_SIZE);
        
        gui.addSlider("eyeROIL_X",ROI.x,1,BUFFER_SIZE);
        gui.addSlider("eyeROIL_W",ROI.width,1,BUFFER_SIZE);
        gui.addSlider("eyeROIL_Y",ROI.y,1,BUFFER_SIZE);
        gui.addSlider("eyeROIL_H",ROI.height,1,BUFFER_SIZE);
        
        //gui.addPage("OffSet");
        gui.addSlider( "offset.x",offset.x,-BUFFER_SIZE,BUFFER_SIZE);
        gui.addSlider( "offset.y",offset.y,-BUFFER_SIZE,BUFFER_SIZE);
        gui.addSlider( "offset.width",offset.width,-BUFFER_SIZE,BUFFER_SIZE);
        gui.addSlider( "offset.height",offset.height,-BUFFER_SIZE,BUFFER_SIZE);
        
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
        
    }
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
            glTranslatef(rect.x,rect.y,0);
            {
                draw(0,0);
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
    void update();
    void draw();
    void exit();
    void processTracking(int x, int y , int w, int h , ofTexture& tex);
    
    ofxKinect kinect;
    bool bKinect;
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvGrayscaleImage grayDepthThres; // the far thresholded image
    bool bThreshWithOpenCV;
    int nearThreshold;
	int farThreshold;
	int angle;
    
    int kminArea;
    int kmaxArea;
    int knConsidered;
    bool kbFindHoles;
    bool kbUseApproximation;
    
    ofxCvContourFinder contourFinder;
#ifdef _USE_LIVE_VIDEO
    ofVideoGrabber 		vidGrabber;
#else
    ofVideoPlayer 		vidPlayer;
#endif
    ofxCvHaarFinder facefinder;
    ofRectangle       faceOffset;
    
    ofFbo faceBuffer;
    ofMesh normFace;
    ofPixels facePixels;
    
    ofxCvColorImage			colorImg;
    ofxCvColorImage			colorImgFace;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayImageFace;
    
    ofRectangle faceRect;
    
    FaceFeature leftEye;
    FaceFeature rightEye;
    FaceFeature nose;
    FaceFeature mouth;
    
    ofShader alphaMaskBlurShader;
    
    ofxBlur blurs[4];
    
    ofShader alphaMaskShader;
};


#endif

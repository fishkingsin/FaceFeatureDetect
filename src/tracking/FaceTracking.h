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

#define USE_QTKIT
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
            //glTranslatef(rect.x,rect.y,0);
            //glScalef(scale,scale,1);
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
    void update();
    void draw();
    void exit();
    void processTracking(int x, int y , int w, int h , ofTexture& tex);
    void savingFace(string fn);
    void drawFeatures();
    void drawFeaturesBlur();
    void drawMarkers();
#ifdef _USE_LIVE_VIDEO
#ifdef USE_QTKIT
    ofxQTKitVideoGrabber vidGrabber;
#else
    ofVideoGrabber 		vidGrabber;
#endif
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
    
    int minArea ,
     minFaceAreaW ,
     minFaceAreaH ;
};


#endif

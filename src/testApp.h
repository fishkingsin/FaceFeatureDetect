#pragma once

#include "ofMain.h"
#include "ofxCvHaarFinder.h"
#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
// otherwise, we'll use a movie file
#include "ofxSimpleGuiToo.h"
#include "ofxKinect.h"
#include "ofxBlur.h"
#ifdef USE_FACETRACKER
#include "ofxFraceTracker.h"
#endif
class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    
    void drawLeftEye();
    void drawRightEye(); 
    void drawNose(); 
    void drawMouth();
    
    void exit();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);		
    void processTracking(int x, int y , int w, int h , ofTexture& tex);
    void drawNormal(ofxCvHaarFinder &finer , ofRectangle &rect, ofFbo &fbo, ofTexture &tex , ofMesh &normal, ofTexture &mask , ofRectangle &Offset);
    
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
    ofxCvHaarFinder eyefinder;
    ofxCvHaarFinder leftEyefinder;
    ofxCvHaarFinder rightEyefinder;
    ofxCvHaarFinder nosefinder;
    ofxCvHaarFinder mouthfinder;
    
    ofFbo faceBuffer;
    ofMesh normFace;
    ofPixels facePixels;
    
    
    ofFbo lefteyeBuffer;
    ofFbo righteyeBuffer;
    ofFbo noseBuffer;
    ofFbo mouthBuffer;
    
    
    ofMesh normEyeLeft;
    ofMesh normEyeRight;
    ofMesh normNose;
    ofMesh normMouth;
    
    ofFbo leftEyeMask;
    ofFbo rightEyeMask;
    ofFbo noseMask;
    ofFbo mouthMask;
    
    ofRectangle faceRect;
    ofRectangle leftEyeRect;
    ofRectangle rightEyeRect;
    ofRectangle noseRect;
    ofRectangle mouthRect;
    
        ofRectangle leftEyeRectOffSet;
        ofRectangle rightEyeRectOffSet;
        ofRectangle noseRectOffSet;
        ofRectangle mouthRectOffSet;
    
    ofxCvColorImage			colorImg;
    ofxCvColorImage			colorImgs;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayImages;
    
    ofRectangle		eyeROIL;
    ofRectangle		eyeROIR;
    ofRectangle		noseROI;
    ofRectangle		mouthROI;
    ofShader alphaMaskShader;
    
    ofShader alphaMaskBlurShader;
    ofxBlur blurLEye;
    ofxBlur blurREye;
    ofxBlur blurNose;
    ofxBlur blurMouth;
    
};

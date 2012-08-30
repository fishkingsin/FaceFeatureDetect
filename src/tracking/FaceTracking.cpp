//
//  FaceTracking.cpp
//  FaceFeatureDetect
//
//  Created by james KONG on 29/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "FaceTracking.h"
static int minArea =  50;
static int minFaceAreaW =  50;
static int minFaceAreaH =  50;
void FaceTracking::setup()
{
#ifdef _USE_LIVE_VIDEO
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(camW,camH);
#else
    vidPlayer.loadMovie("fingers.mov");
    vidPlayer.play();
#endif
    colorImg.allocate(camW,camH);
    grayImage.allocate(camW,camH);
    
    
    
	facefinder.setup("haarcascade_frontalface_alt2.xml");
    
    
    colorImgFace.allocate(BUFFER_SIZE,BUFFER_SIZE);
	grayImageFace.allocate(BUFFER_SIZE,BUFFER_SIZE);
    
    faceBuffer.allocate(BUFFER_SIZE,BUFFER_SIZE,GL_RGB);
    facePixels.allocate(faceBuffer.getWidth(),faceBuffer.getHeight(),OF_IMAGE_COLOR);
    faceBuffer.begin();
    ofClear(0,0,0,255);
    faceBuffer.end();
    
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
    
	normFace.addVertex(ofVec2f(0, 0));
	normFace.addVertex(ofVec2f(faceBuffer.getWidth(), 0));
	normFace.addVertex(ofVec2f(faceBuffer.getWidth(), faceBuffer.getHeight()));
	normFace.addVertex(ofVec2f(0, faceBuffer.getHeight()));
	normFace.addTexCoord(0);
	normFace.addTexCoord(0);
	normFace.addTexCoord(0);
	normFace.addTexCoord(0);
	normFace.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    
	gui.setDefaultKeys(true);
	gui.addSlider("minFaceAreaW",minFaceAreaW,1,512);
	gui.addSlider("minFaceAreaH",minFaceAreaH,1,512);
    gui.addSlider("faceOffsetX",faceOffset.x,1,512);
    gui.addSlider("faceOffsetY",faceOffset.y,1,512);
    gui.addSlider("faceOffsetW",faceOffset.width,1,512);
    gui.addSlider("faceOffsetH",faceOffset.height,1,512);
    
    
    
    leftEye.setup("lefteye","haarcascade_mcs_lefteye.xml","leftEyeMask.png",64,48);
    rightEye.setup("righteye","haarcascade_mcs_righteye.xml","rightEyeMask.png",64,48);
    nose.setup("nose","haarcascade_mcs_nose.xml","noseMask.png",48,64);
    mouth.setup("mouth","haarcascade_mcs_mouth.xml","mouthMask.png",64,48);
    
    
    gui.loadFromXML();
    
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
void FaceTracking::update()
{
    bool bNewFrame = false;
    
    {
#ifdef _USE_LIVE_VIDEO
        vidGrabber.update();
        bNewFrame = vidGrabber.isFrameNew();
#else
        vidPlayer.idleMovie();
        bNewFrame = vidPlayer.isFrameNew();
#endif
        
        if (bNewFrame){
            
#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels(), camW,camH);
#else
            colorImg.setFromPixels(vidPlayer.getPixels(), camW,camH);
#endif
            
            grayImage = colorImg;
            
            processTracking(0,0,camW,camH,vidGrabber.getTextureReference());
        }
        
    }
}
void FaceTracking::savingFace(string fn)
{
    
    ofSaveImage(facePixels, fn);
}
void FaceTracking::draw()
{
    ofSetColor(ofColor::white);
    {
        vidGrabber.draw(0,0);
        
        
        ofPushMatrix();
        ofTranslate(faceRect.x, faceRect.y);
        ofScale(faceRect.width/BUFFER_SIZE*1.0f, faceRect.height/BUFFER_SIZE*1.0f);
        drawFeaturesBlur();
        drawFeatures();
        
        
        ofPopMatrix();
        
        
    }
    ofPushMatrix();
    ofTranslate(camW,0);
    {
        faceBuffer.draw(0,0);
        
        ofPushMatrix();
        ofTranslate(0,camH*0.5f);
        drawFeatures();
        
        ofPopMatrix();
        
        drawMarkers();
        
        
        
    }
    ofPopMatrix();
    ofPushStyle();
    ofSetColor(ofColor::black);
    ostringstream os;
    os <<"minArea : "+ofToString(minArea) <<endl ;
    if(facefinder.blobs.size()>0) os << "face detect OK"<<endl;
    else os << "no face detected"<<endl;
    os << "spacebar toggle gui" <<endl;
    ofDrawBitmapString(os.str(),10,ofGetHeight()-50);
    ofPopStyle();
}
void FaceTracking::drawFeatures()
{
    leftEye.drawEffect(alphaMaskShader);
    rightEye.drawEffect(alphaMaskShader);
    nose.drawEffect(alphaMaskShader);
    mouth.drawEffect(alphaMaskShader);
}
void FaceTracking::drawFeaturesBlur()
{
    leftEye.drawEffect(alphaMaskShader,&blurs[0]);
    rightEye.drawEffect(alphaMaskShader,&blurs[1]);
    nose.drawEffect(alphaMaskShader,&blurs[2]);
    mouth.drawEffect(alphaMaskShader,&blurs[3]);
    
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
void FaceTracking::drawMarkers()
{
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::yellow);
    leftEye.drawRect();
    
    ofSetColor(ofColor::cyan);
    rightEye.drawRect();
    ofSetColor(ofColor::blue);
    nose.drawRect();
    ofSetColor(ofColor::gray);
    mouth.drawRect();
    ofPopStyle();
}
void FaceTracking::processTracking(int x, int y , int w, int h , ofTexture &tex)
{
    facefinder.findHaarObjects(grayImage,x,y,w,h,minFaceAreaW,minFaceAreaH);
    if(facefinder.blobs.size()>0)
    {
        ofRectangle cur = facefinder.blobs[0].boundingRect;
        faceRect.x = cur.x+faceOffset.x;
        faceRect.y = cur.y+faceOffset.y;
        faceRect.width = cur.width+faceOffset.width;
        faceRect.height = cur.height+faceOffset.height;
        
        //        ofRect(faceRect.x, faceRect.y, faceRect.width, faceRect.height);
        
        normFace.setTexCoord(0,ofVec2f(faceRect.x, faceRect.y));
        normFace.setTexCoord(1,ofVec2f(faceRect.x+faceRect.width, faceRect.y));
        normFace.setTexCoord(2,ofVec2f(faceRect.x+faceRect.width, faceRect.y+faceRect.height));
        normFace.setTexCoord(3,ofVec2f(faceRect.x, faceRect.y+faceRect.height));
        
        
        
        faceBuffer.begin();	
        tex.bind();
        normFace.draw();
        tex.unbind();
        faceBuffer.end();
        
        faceBuffer.readToPixels(facePixels);
        colorImgFace.setFromPixels(facePixels.getPixels(),BUFFER_SIZE,BUFFER_SIZE);
        grayImageFace = colorImgFace;
        
        leftEye.update(grayImageFace,faceBuffer.getTextureReference());
        rightEye.update(grayImageFace,faceBuffer.getTextureReference());
        nose.update(grayImageFace,faceBuffer.getTextureReference());
        mouth.update(grayImageFace,faceBuffer.getTextureReference());
        
    }
}
void FaceTracking::exit()
{

}
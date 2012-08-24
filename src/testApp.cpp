#include "testApp.h"
#define camW 640
#define camH 480
int minArea =  50;
int minFaceAreaW =  50;
int minFaceAreaH =  50;
#define BUFFER_SIZE 256
#define QUARTER_SIZE 0.25

//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableAlphaBlending();
    
    // enable depth->video image calibration
    
    
    bKinect = false;
    kinect.setRegistration(true);
	kinect.init();
	if(kinect.open())
    {
        bKinect = true;
        
        colorImg.allocate(kinect.width, kinect.height);
        grayImage.allocate(kinect.width, kinect.height);
        
        grayThreshNear.allocate(kinect.width, kinect.height);
        grayThreshFar.allocate(kinect.width, kinect.height);
        grayDepthThres.allocate(kinect.width, kinect.height);
        nearThreshold = 230;
        farThreshold = 70;
        bThreshWithOpenCV = true;
        // zero the tilt on startup
        angle = 0;
        kinect.setCameraTiltAngle(angle);
	}
    else
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
    }
    
    
	facefinder.setup("haarcascade_frontalface_alt2.xml");
	leftEyefinder.setup("haarcascade_mcs_lefteye.xml");
	rightEyefinder.setup("haarcascade_mcs_righteye.xml");
	nosefinder.setup("haarcascade_mcs_nose.xml");
	mouthfinder.setup("haarcascade_mcs_mouth.xml");
    
    colorImgs.allocate(BUFFER_SIZE,BUFFER_SIZE);
	grayImages.allocate(BUFFER_SIZE,BUFFER_SIZE);
    
	eyeROIL.set(ofRectangle(0,QUARTER_SIZE,BUFFER_SIZE,BUFFER_SIZE*QUARTER_SIZE));
	eyeROIR.set(ofRectangle(0,QUARTER_SIZE,BUFFER_SIZE,BUFFER_SIZE*QUARTER_SIZE));
	noseROI.set(ofRectangle(0,BUFFER_SIZE*QUARTER_SIZE*2,BUFFER_SIZE,BUFFER_SIZE*QUARTER_SIZE));
	mouthROI.set(ofRectangle(0,BUFFER_SIZE*QUARTER_SIZE*3,BUFFER_SIZE,BUFFER_SIZE*QUARTER_SIZE));
	
    faceBuffer.allocate(BUFFER_SIZE,BUFFER_SIZE,GL_RGB);
    facePixels.allocate(faceBuffer.getWidth(),faceBuffer.getHeight(),OF_IMAGE_COLOR);
    faceBuffer.begin();
    ofClear(0,0,0,255);
    faceBuffer.end();
    ofMesh *temp[4];
    temp[0] = &normEyeLeft;
    temp[1] = &normEyeRight;
    temp[2] = &normNose;
    temp[3] = &normMouth;
    
    ofFbo *temp2[4];
    
    temp2[0] = &lefteyeBuffer;
    temp2[1] = &righteyeBuffer;
    temp2[2] = &noseBuffer;
    temp2[3] = &mouthBuffer;
    ofxBlur *blurs[4];
    blurs[0] = &blurLEye;
    blurs[1] = &blurREye;
    blurs[2] = &blurNose;
    blurs[3] = &blurMouth;
    for(int i = 0 ; i < 4 ; i++)
    {
        if(i==0 || i==1 || i==3)
        {
            temp2[i]->allocate(64,48,GL_RGBA);
            blurs[i]->setup(64,48, 4, 0.2, 4);
        }
        else 
        {
            temp2[i]->allocate(48,64,GL_RGB);
            blurs[i]->setup(48,64, 4, 0.2, 4);
        }
        blurs[i]->setScale(4);
        blurs[i]->setRotation(0);
        temp2[i]->begin();
        ofClear(0,0,0,255);
        temp2[i]->end();
        temp[i]->addVertex(ofVec2f(0, 0));
        temp[i]->addVertex(ofVec2f(temp2[i]->getWidth(), 0));
        temp[i]->addVertex(ofVec2f(temp2[i]->getWidth(), temp2[i]->getHeight()));
        temp[i]->addVertex(ofVec2f(0, temp2[i]->getHeight()));
        temp[i]->addTexCoord(0);
        temp[i]->addTexCoord(0);
        temp[i]->addTexCoord(0);
        temp[i]->addTexCoord(0);
        temp[i]->setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        
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
	gui.addSlider("minFeatureArea",minArea,1,512);
	
	gui.addSlider("eyeROIL_X",eyeROIL.x,1,BUFFER_SIZE);
	gui.addSlider("eyeROIL_W",eyeROIL.width,1,BUFFER_SIZE);
	gui.addSlider("eyeROIL_Y",eyeROIL.y,1,BUFFER_SIZE);
	gui.addSlider("eyeROIL_H",eyeROIL.height,1,BUFFER_SIZE);
    
	gui.addSlider("eyeROIR_X",eyeROIR.x,1,BUFFER_SIZE);
	gui.addSlider("eyeROIR_W",eyeROIR.width,1,BUFFER_SIZE);
	gui.addSlider("eyeROIR_Y",eyeROIR.y,1,BUFFER_SIZE);
	gui.addSlider("eyeROIR_H",eyeROIR.height,1,BUFFER_SIZE);
    
	gui.addSlider("noseROI_Y",noseROI.y,1,BUFFER_SIZE);
	gui.addSlider("noseROI_H",noseROI.height,1,BUFFER_SIZE);
    
	gui.addSlider("mouthROI_Y",mouthROI.y,1,BUFFER_SIZE);
	gui.addSlider("mouthROI_H",mouthROI.height,1,BUFFER_SIZE);
    gui.addPage("ExtractOffSet");
    gui.addSlider( "leftEyeRectOffSet.x",leftEyeRectOffSet.x,-100,100);
    gui.addSlider( "leftEyeRectOffSet.x",leftEyeRectOffSet.y,-100,100);
    gui.addSlider( "leftEyeRectOffSet.width",leftEyeRectOffSet.width,-100,100);
    gui.addSlider( "leftEyeRectOffSet.height",leftEyeRectOffSet.height,-100,100);
    
    gui.addSlider( "rightEyeRectOffSet.x",rightEyeRectOffSet.x,-100,100);
    gui.addSlider( "rightEyeRectOffSet.y",rightEyeRectOffSet.y,-100,100);
    gui.addSlider( "rightEyeRectOffSet.width",rightEyeRectOffSet.width,-100,100);
    gui.addSlider( "rightEyeRectOffSet.height",rightEyeRectOffSet.height,-100,100);
    
    gui.addSlider( "noseRectOffSet.x",noseRectOffSet.x,-100,100);
    gui.addSlider( "noseRectOffSet.y",noseRectOffSet.y,-100,100);
    gui.addSlider( "noseRectOffSet.width",noseRectOffSet.width,-100,100);
    gui.addSlider( "noseRectOffSet.height",noseRectOffSet.height,-100,100);
    
    gui.addSlider( "mouthRectOffSet.x",mouthRectOffSet.x,-100,100);
    gui.addSlider( "mouthRectOffSet.y",mouthRectOffSet.y,-100,100);
    gui.addSlider( "mouthRectOffSet.width",mouthRectOffSet.width,-100,100);
    gui.addSlider( "mouthRectOffSet.height",mouthRectOffSet.height,-100,100);
    
    gui.addPage("Kinect");
    gui.addSlider("nearThreshold",nearThreshold,0,255);
    gui.addSlider("farThreshold",farThreshold,0,255);
    gui.addToggle("bThreshWithOpenCV",bThreshWithOpenCV);
    // zero the tilt on startup
    gui.addSlider("angle",angle,-90,90);
    
    gui.addSlider("kminArea",kminArea,1,kinect.getWidth()*kinect.getHeight()*0.5f);
    gui.addSlider("kmaxArea",kmaxArea,1,kinect.getWidth()*kinect.getHeight());
    gui.addSlider("knConsidered",knConsidered,1,10);
    gui.addToggle("kbFindHoles",kbFindHoles);
    gui.addToggle("kbUseApproximation",kbUseApproximation);
	gui.loadFromXML();
    
    // There are 3 of ways of loading a shader:
    //
    //  1 - Using just the name of the shader and ledding ofShader look for .frag and .vert: 
    //      Ex.: shader.load( "myShader");
    //
    //  2 - Giving the right file names for each one: 
    //      Ex.: shader.load( "myShader.vert","myShader.frag");
    //
    //  3 - And the third one it«s passing the shader programa on a single string;
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
    
    
    ofEnableArbTex();
    ofImage leftEyeMaskImage;
    ofImage rightEyeMaskImage;
    ofImage noseMaskImage;
    ofImage mouthMaskImage;
    
    leftEyeMaskImage.loadImage("leftEyeMask.png");
    rightEyeMaskImage.loadImage("rightEyeMask.png");
    noseMaskImage.loadImage("noseMask.png");
    mouthMaskImage.loadImage("mouthMask.png");
    
    leftEyeMask.allocate(leftEyeMaskImage.getWidth(),leftEyeMaskImage.getHeight(),GL_RGBA);    
    rightEyeMask.allocate(rightEyeMaskImage.getWidth(),rightEyeMaskImage.getHeight(),GL_RGBA);    
    noseMask.allocate(noseMaskImage.getWidth(),noseMaskImage.getHeight(),GL_RGBA);    
    mouthMask.allocate(mouthMaskImage.getWidth(),mouthMaskImage.getHeight(),GL_RGBA);    
    
    leftEyeMask.begin();
    ofClear(0, 0, 0, 0);
    leftEyeMaskImage.draw(0,0);
    leftEyeMask.end();
    
    rightEyeMask.begin();
    ofClear(0, 0, 0, 0);
    rightEyeMaskImage.draw(0,0);
    rightEyeMask.end();
    
    noseMask.begin();
    ofClear(0, 0, 0, 0);
    noseMaskImage.draw(0,0);
    noseMask.end();
    
    mouthMask.begin();
    ofClear(0, 0, 0, 0);
    mouthMaskImage.draw(0,0);
    mouthMask.end();
    
    ofDisableArbTex();
    
}

//--------------------------------------------------------------
void testApp::update(){
	bool bNewFrame = false;
    if(bKinect)
    {
        kinect.update();
        kinect.setCameraTiltAngle(angle);
        // there is a new frame and we are connected
        if(kinect.isFrameNew()) {
            bNewFrame = true;
            // load grayscale depth image from the kinect source
            grayDepthThres.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
            colorImg.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
            
            // we do two thresholds - one for the far plane and one for the near plane
            // we then do a cvAnd to get the pixels which are a union of the two thresholds
            //            if(bThreshWithOpenCV) {
            grayThreshNear = grayDepthThres;
            grayThreshFar = grayDepthThres;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayDepthThres.getCvImage(), NULL);
            
            
            // update the cv images
            grayDepthThres.flagImageChanged();
            
            // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
            // also, find holes is set to true so we will get interior contours as well....
            contourFinder.findContours(grayDepthThres, kminArea,
                                       kmaxArea,
                                       knConsidered,
                                       kbFindHoles,
                                       kbUseApproximation);
            if(contourFinder.blobs.size()>0)
            {
                grayImage = colorImg;
                ofRectangle cur = contourFinder.blobs[0].boundingRect;
                processTracking(cur.x,cur.y,cur.width,cur.height,kinect.getTextureReference());
            }
        }
    }
    else
    {
#ifdef _USE_LIVE_VIDEO
        vidGrabber.grabFrame();
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
void testApp::processTracking(int x, int y , int w, int h , ofTexture &tex)
{
    facefinder.findHaarObjects(grayImage,x,y,w,h,minFaceAreaW,minFaceAreaH);
    if(facefinder.blobs.size()>0)
    {
        ofRectangle cur = facefinder.blobs[0].boundingRect;
        faceRect = cur;
        ofRect(cur.x, cur.y, cur.width, cur.height);
        
        normFace.setTexCoord(0,ofVec2f(cur.x, cur.y));
        normFace.setTexCoord(1,ofVec2f(cur.x+cur.width, cur.y));
        normFace.setTexCoord(2,ofVec2f(cur.x+cur.width, cur.y+cur.height));
        normFace.setTexCoord(3,ofVec2f(cur.x, cur.y+cur.height));
        
        
        
        faceBuffer.begin();	
        tex.bind();
        normFace.draw();
        tex.unbind();
        faceBuffer.end();
        
        faceBuffer.readToPixels(facePixels);
        colorImgs.setFromPixels(facePixels.getPixels(),BUFFER_SIZE,BUFFER_SIZE);
        grayImages = colorImgs;
        if(leftEyefinder.findHaarObjects(grayImages,eyeROIL,minArea,minArea)>0)
        {
            drawNormal(leftEyefinder,leftEyeRect,lefteyeBuffer,faceBuffer.getTextureReference(),normEyeLeft, leftEyeMask.getTextureReference(),leftEyeRectOffSet);
        }
        if(rightEyefinder.findHaarObjects(grayImages,eyeROIR,minArea,minArea)>0)
        {
            drawNormal(rightEyefinder,rightEyeRect,righteyeBuffer,faceBuffer.getTextureReference(),normEyeRight, rightEyeMask.getTextureReference(),rightEyeRectOffSet);
        }
        if(nosefinder.findHaarObjects(grayImages,noseROI,minArea,minArea)>0)
        {
            drawNormal(nosefinder,noseRect,noseBuffer,faceBuffer.getTextureReference(),normNose, noseMask.getTextureReference(),noseRectOffSet);
            
        }
        if(mouthfinder.findHaarObjects(grayImages,mouthROI,minArea,minArea)>0)
        {
            drawNormal(mouthfinder,mouthRect,mouthBuffer,faceBuffer.getTextureReference(),normMouth, mouthMask.getTextureReference(),mouthRectOffSet);
        }
        
    }
}
void testApp::drawNormal(ofxCvHaarFinder &finer , ofRectangle &rect, ofFbo &fbo, ofTexture &tex , ofMesh &normal , ofTexture &mask,ofRectangle &Offset )
{
    ofEnableAlphaBlending();
    ofRectangle curr = finer.blobs[0].boundingRect;
    rect.x = curr.x+Offset.x;
    rect.y = curr.y+Offset.y;
    rect.width = curr.width+Offset.width;
    rect.height = curr.height+Offset.height;
    ofRect(rect.x, rect.y, rect.width, rect.height);
    
    normal.setTexCoord(0,ofVec2f(rect.x, rect.y));
    normal.setTexCoord(1,ofVec2f(rect.x+rect.width, rect.y));
    normal.setTexCoord(2,ofVec2f(rect.x+rect.width, rect.y+rect.height));
    normal.setTexCoord(3,ofVec2f(rect.x, rect.y+rect.height));
    
    
    
    fbo.begin();	
    ofClear(0, 0, 0, 0);
    
    tex.bind();
    normal.draw();
    tex.unbind();
    
    fbo.end();
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(ofColor::white);
    if(bKinect)
    {
        kinect.drawDepth(10, 10, 400, 300);
		kinect.draw(420, 10, 400, 300);
		
		grayDepthThres.draw(10, 320, 400, 300);
		contourFinder.draw(10, 320, 400, 300);
    }
    else{
        vidGrabber.draw(0,0);
        
        
        ofPushMatrix();
        ofTranslate(faceRect.x, faceRect.y);
        ofScale(faceRect.width/BUFFER_SIZE, faceRect.height/BUFFER_SIZE);
        blurLEye.begin();
        {
            lefteyeBuffer.draw(0,0);//,leftEyeRect.width,leftEyeRect.height);
        }
        blurLEye.end();
        glPushMatrix();
        glTranslatef(leftEyeRect.x,leftEyeRect.y,0);
        {
            alphaMaskShader.begin();
            alphaMaskShader.setUniformTexture("maskTex", leftEyeMask, 1 );
            

            blurLEye.draw();
            
            alphaMaskShader.end();
        }
        glPopMatrix();
        
        blurREye.begin();
        {
            righteyeBuffer.draw(0,0);//,rightEyeRect.width,rightEyeRect.height);
        }
        blurREye.end();
        glPushMatrix();
        glTranslatef(rightEyeRect.x,rightEyeRect.y,0);
        {
            alphaMaskShader.begin();
            alphaMaskShader.setUniformTexture("maskTex", rightEyeMask, 1 );
            blurREye.draw();
            alphaMaskShader.end();
        }
        glPopMatrix();
        
        blurNose.begin();
        {
            noseBuffer.draw(0,0);//,noseRect.width,noseRect.height);
        }
        blurNose.end();
        glPushMatrix();
        glTranslatef(noseRect.x,noseRect.y,0);
        {
            alphaMaskShader.begin();
            alphaMaskShader.setUniformTexture("maskTex", noseMask, 1 );
            blurNose.draw(0,0,noseRect.width,noseRect.height);
            alphaMaskShader.end();
        }
        glPopMatrix();
        
        blurMouth.begin();
        {
            mouthBuffer.draw(0,0);//,mouthRect.width,mouthRect.height);
        }
        blurMouth.end();
        glPushMatrix();
        glTranslatef(mouthRect.x,mouthRect.y,0);
        {
            alphaMaskShader.begin();
            alphaMaskShader.setUniformTexture("maskTex", mouthMask, 1 );
            blurMouth.draw(0,0,mouthRect.width,mouthRect.height);
            alphaMaskShader.end();
        }
        glPopMatrix();
        ofPopMatrix();
        
        
    }
    ofPushMatrix();
    ofTranslate(camW,0);
    faceBuffer.draw(0,0);
    
    
    ofPushMatrix();
    ofTranslate(0,camH*0.5f);
    drawLeftEye();
    drawRightEye(); 
    drawNose(); 
    drawMouth();
    
    ofPopMatrix();
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::yellow);
    ofRect(eyeROIL);
    ofSetColor(ofColor::cyan);
    ofRect(eyeROIR);
    ofSetColor(ofColor::blue);
    ofRect(noseROI);
    ofSetColor(ofColor::gray);
    ofRect(mouthROI);
    ofPopStyle();
    
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::green);
    
    for(int i = 0; i < leftEyefinder.blobs.size(); i++) {
        ofRectangle cur = leftEyefinder.blobs[i].boundingRect;
        ofRect(cur.x, cur.y, cur.width, cur.height);
        ofDrawBitmapString("left eye",cur.x, cur.y);
    }
    ofPopStyle();
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::blue);
    for(int i = 0; i < rightEyefinder.blobs.size(); i++) {
        ofRectangle cur = rightEyefinder.blobs[i].boundingRect;
        ofRect(cur.x, cur.y, cur.width, cur.height);
        ofDrawBitmapString("right eye",cur.x, cur.y);
    }
    ofPopStyle();
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::white);
    for(int i = 0; i < mouthfinder.blobs.size(); i++) {
        ofRectangle cur = mouthfinder.blobs[i].boundingRect;
        ofRect(cur.x, cur.y, cur.width, cur.height);
        ofDrawBitmapString("mouth",cur.x, cur.y);
    }
    ofPopStyle();
    ofPushStyle();
    ofNoFill();
    ofSetColor(ofColor::black);
    for(int i = 0; i < nosefinder.blobs.size(); i++) {
        ofRectangle cur = nosefinder.blobs[i].boundingRect;
        ofRect(cur.x, cur.y, cur.width, cur.height);
        ofDrawBitmapString("nose",cur.x, cur.y);
    }
    ofPopStyle();
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
    gui.draw();
}
void testApp::drawLeftEye()
{
    alphaMaskShader.begin();
    alphaMaskShader.setUniformTexture("maskTex", leftEyeMask, 1 );
    lefteyeBuffer.draw(leftEyeRect.x,leftEyeRect.y,leftEyeRect.width,leftEyeRect.height);
    alphaMaskShader.end();
}
void testApp::drawRightEye()
{
    alphaMaskShader.begin();
    alphaMaskShader.setUniformTexture("maskTex", rightEyeMask, 1 );
    righteyeBuffer.draw(rightEyeRect.x,rightEyeRect.y,rightEyeRect.width,rightEyeRect.height);
    alphaMaskShader.end();
    
}
void testApp::drawNose()
{    
    alphaMaskShader.begin();
    alphaMaskShader.setUniformTexture("maskTex", noseMask, 1 );
    noseBuffer.draw(noseRect.x,noseRect.y,noseRect.width,noseRect.height);
    alphaMaskShader.end();
}
void testApp::drawMouth()
{   
    alphaMaskShader.begin();
    alphaMaskShader.setUniformTexture("maskTex", mouthMask, 1 );
    mouthBuffer.draw(mouthRect.x,mouthRect.y,mouthRect.width,mouthRect.height);
    alphaMaskShader.end();
}
void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
    
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch(key)
    {
        case '=':
            minArea++;
            break;
        case '-':
            minArea--;
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
    
}
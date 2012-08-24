#include "testApp.h"
int minArea =  50;
int minFaceAreaW =  50;
int minFaceAreaH =  50;

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
    
    
    leftEye.setup("lefteye","haarcascade_mcs_lefteye.xml","leftEyeMask.png",64,48);
    rightEye.setup("righteye","haarcascade_mcs_righteye.xml","rightEyeMask.png",64,48);
    nose.setup("nose","haarcascade_mcs_nose.xml","noseMask.png",48,64);
    mouth.setup("mouth","haarcascade_mcs_mouth.xml","mouthMask.png",64,48);
    
    
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
        colorImgFace.setFromPixels(facePixels.getPixels(),BUFFER_SIZE,BUFFER_SIZE);
        grayImageFace = colorImgFace;
        
        leftEye.update(grayImageFace,faceBuffer.getTextureReference());
        rightEye.update(grayImageFace,faceBuffer.getTextureReference());
        nose.update(grayImageFace,faceBuffer.getTextureReference());
        mouth.update(grayImageFace,faceBuffer.getTextureReference());
        
    }
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
        
        
        ofPopMatrix();
        
        
    }
    ofPushMatrix();
    ofTranslate(camW,0);
    {
        faceBuffer.draw(0,0);
        
        
        ofPushMatrix();
        ofTranslate(0,camH*0.5f);
        leftEye.drawEffect(alphaMaskShader);
        rightEye.drawEffect(alphaMaskShader);
        nose.drawEffect(alphaMaskShader);
        mouth.drawEffect(alphaMaskShader);
        
        ofPopMatrix();
    
    

    
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

void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
    
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch(key)
    {
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
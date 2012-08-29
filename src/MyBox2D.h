//
//  MyBox2D.h
//  FaceFeatureDetect
//
//  Created by james KONG on 29/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef FaceFeatureDetect_MyBox2D_h
#define FaceFeatureDetect_MyBox2D_h
#include "ofxBox2d.h"
#include "FaceTracking.h"
// A Custom Particle extedning the box2d circle
class CustomParticle : public ofxBox2dCircle {
	
public:
	
	void setupTheCustomData(FaceFeature * feature) {
		
//		static int colors[] = {0xcae72b, 0xe63b8f, 0x2bb0e7};
//		static string abc   = "abcdefghijklmnopqrstuvwxyz";
		
		// we are using a Data pointer because 
		// box2d needs to have a pointer not 
		// a referance
		setData(feature);
//		Data * theData = (Data*)getData();
//		
//		theData->id = ofRandom(0, 100);
//		theData->name += abc[(int)ofRandom(0, abc.size())];
//		theData->color.setHex(colors[(int)ofRandom(0, 3)]);
        
		printf("setting the custom data!\n");
		
	}
	
	void draw() {
		FaceFeature* theData = (FaceFeature*)getData();
		if(theData) {
			
			// Evan though we know the data object lets just 
			// see how we can get the data out from box2d
			// you would use this when using a contact listener
			// or tapping into box2d's solver.
			
			float radius = getRadius();
			ofPushMatrix();
			ofTranslate(getPosition());
			ofRotateZ(getRotation());
			//ofSetColor(theData->color);
			ofFill();
			ofCircle(0, 0, radius);	
			
			ofSetColor(255);
			//ofDrawBitmapString(theData->name, -5, 5);
			ofPopMatrix();
		}
	}
    
    
};

class MyBox2D{
public:
    void setup();
    void update();
    void draw();
    void exit();
    ofxBox2d box2d;
};


#endif

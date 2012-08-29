//
//  MyBox2D.h
//  FaceFeatureDetect
//
//  Created by james KONG on 29/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef FaceFeatureDetect_MyBox2D_h
#define FaceFeatureDetect_MyBox2D_h
#include "FaceTracking.h"
#include "ofxBox2d.h"
struct DataSet
{
    FaceFeature * feature;
    ofShader *shader;
};
// A Custom Particle extedning the box2d circle
class CustomParticle : public ofxBox2dCircle {
	
public:
	
	void setupTheCustomData(FaceFeature * feature , ofShader *shader) {
        
		setData(new DataSet);
        DataSet* theData = (DataSet*)getData();
        theData->feature = feature;
        theData->shader = shader;
		printf("setting the custom data!\n");
		
	}
	
	void draw() {
		DataSet* theData = (DataSet*)getData();
        FaceFeature * feature = theData->feature;
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
            feature->drawEffect(*theData->shader);
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
    void addParticle(CustomParticle &p);
    ofxBox2d box2d;
    vector <CustomParticle>		particles;
};


#endif

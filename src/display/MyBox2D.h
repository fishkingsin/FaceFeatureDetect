/*
 *  MyBox2D.h
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

#ifndef FaceFeatureDetect_MyBox2D_h
#define FaceFeatureDetect_MyBox2D_h
#include "FaceTracking.h"
#include "ofxBox2d.h"
struct DataSet
{
    FaceFeature * feature;
    ofShader *shader;
    float scaleX,scaleY;
};
// A Custom Particle extedning the box2d circle
class CustomParticle : public ofxBox2dCircle {
	
public:
	
	void setupTheCustomData(FaceFeature * feature , ofShader *shader , float scaleX, float scaleY) {
        
		setData(new DataSet);
        DataSet* theData = (DataSet*)getData();
        theData->feature = feature;
        theData->shader = shader;
        theData->scaleX = scaleX;
        theData->scaleY = scaleY;
		printf("setting the custom data!\n");
		
	}
	
	void draw() {
		DataSet* theData = (DataSet*)getData();
        FaceFeature * feature = theData->feature;
        ofShader * shader = theData->shader;
		if(theData) {
			
			// Evan though we know the data object lets just 
			// see how we can get the data out from box2d
			// you would use this when using a contact listener
			// or tapping into box2d's solver.
			
			float _width = getRadius();//getWidth();
//			float _height = getHeight();
			ofPushMatrix();
			ofTranslate(getPosition());
			ofRotateZ(getRotation());
			//ofSetColor(theData->color);
			ofNoFill();
			//ofCircle(0, 0, radius);	
            
            
            
            shader->begin();
            shader->setUniformTexture("maskTex", feature->mask, 1 );
            
            glPushMatrix();
            //glTranslatef(rect.x,rect.y,0);
            //glTranslatef(rect.x,rect.y,0);
            glScalef(theData->scaleX,theData->scaleY,1);
            {
//                feature->draw(0,0,feature->rect.width,feature->rect.height);
                feature->draw(-feature->rect.width*0.5,-feature->rect.height*0.5,feature->rect.width,feature->rect.height);
            }
            glPopMatrix();
            shader->end();
			
            ofPopMatrix();
            if(ofGetLogLevel()==OF_LOG_VERBOSE)ofxBox2dCircle::draw();
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
    ofxBox2d box2d[MAX_PLAYER];
    vector <CustomParticle>		particles;
    void clear(){
		while (!particles.empty())
		{
			
			CustomParticle *p = &particles.back();
			p->destroy();
			particles.pop_back();
		}

	};
};


#endif

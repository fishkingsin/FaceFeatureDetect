//
//  MyBox2D.cpp
//  FaceFeatureDetect
//
//  Created by james KONG on 29/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "MyBox2D.h"
void MyBox2D::setup()
{
    // Box2d
	box2d.init();
	box2d.setGravity(0, 20);
	box2d.createGround();
	box2d.setFPS(60.0);
}
void MyBox2D::update()
{
    box2d.update();	
}
void MyBox2D::draw()
{
    for(int i=0; i<particles.size(); i++) {
		particles[i].draw();
	}   
}
void MyBox2D::exit()
{
    
}
void MyBox2D::addParticle(CustomParticle &p) {
	
	particles.push_back(p);
}
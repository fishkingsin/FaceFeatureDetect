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

#include "MyBox2D.h"
void MyBox2D::setup()
{
    // Box2d
    
	for(int i = 0 ; i < MAX_PLAYER ; i++)
	{
	box2d[i].init();
	box2d[i].setGravity(0, 0.5);
    box2d[i].createBounds(camW*0.5*i,0,camW*0.5,camH);
//	box2d.createGround();
	box2d[i].setFPS(30);
	}
}
void MyBox2D::update()
{
	for(int i = 0 ; i < MAX_PLAYER ; i++)
	{
		box2d[i].update();	
	}
}
void MyBox2D::draw()
{
	for(int i = 0 ; i < MAX_PLAYER ; i++)
	{
	if(ofGetLogLevel()==OF_LOG_VERBOSE)
	{
		ofPushStyle();
		box2d[i].draw();
		ofPopStyle();
	}
	}
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

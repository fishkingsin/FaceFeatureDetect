/*
 *  FaceMapper.h
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

#ifndef FaceFeatureDetect_FaceMapper_h
#define FaceFeatureDetect_FaceMapper_h
#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxGLWarper.h"
#define gridRes 8
class FaceData
{
public:
    ofImage backgroundImage;
    ofImage overlayImage;
    ofImage faceImage;

    string imageFile;
    string prefix;
    ofxGLWarper warpper;
    FaceData()
    {
        
    }
	~FaceData()
    {
		backgroundImage.clear();
		overlayImage.clear();
		faceImage.clear();
    }
    void setup(string img_fn, string setting_fn)
    {

        backgroundImage.clear();
        overlayImage.clear();
        faceImage.clear();
        imageFile = img_fn;
        ofxXmlSettings xml;
        
        if(!xml.loadFile(setting_fn))
        {
            ofLog(OF_LOG_VERBOSE,"Faile to load "+setting_fn);
            xml.addTag("DATA");
            if(xml.pushTag("DATA"))
            {
                xml.setValue("PREFIX","a");
                xml.setValue("BACKGROUND","background.png");
                xml.setValue("OVERLAY","overlay.png");
				xml.popTag();
            }
            xml.saveFile(setting_fn);
        }
        else
        {
            faceImage.loadImage(imageFile);
            if(xml.pushTag("DATA"))
            {
                prefix = xml.getValue("PREFIX","a");
                backgroundImage.loadImage(xml.getValue("BACKGROUND","background.png"));
                overlayImage.loadImage(xml.getValue("OVERLAY","overlay.png"));
                warpper.setup(0,0,faceImage.width,faceImage.height);
                warpper.load("warpper_"+prefix+".xml");

                xml.popTag();
            }
            //save();
        }

    }
    void draw()
    {
        ofEnableAlphaBlending();
        backgroundImage.draw(0,0);
        warpper.begin();
        faceImage.draw(0,0);
        warpper.end();
        warpper.draw();
        
        overlayImage.draw(0,0);
        
    }
    void save()
    {
        ofFbo fbo;
        fbo.allocate(overlayImage.width,overlayImage.height,GL_RGB);
        fbo.begin();
        ofClear(0);
        this->draw();
        fbo.end();
        ofPixels pixels;
        pixels.allocate(overlayImage.width,overlayImage.height,OF_IMAGE_COLOR);
        fbo.readToPixels(pixels);
        string subImageName = imageFile.substr(0,imageFile.length()-4);
        ofSaveImage(pixels, subImageName+prefix+".jpg");
        
    }
    void saveSetting()
    {
        warpper.save("warpper_"+prefix+".xml");
    }
};
class DoubleFaceData : public FaceData
{
public:
    ofxGLWarper warpper2;
    ofImage faceImage2;
	~DoubleFaceData()
    {
		faceImage2.clear();
    }
    void setup(string img_fn,string img_fn2, string setting_fn)
    {
        faceImage2.clear();
        FaceData::setup(img_fn, setting_fn);
        faceImage2.loadImage(img_fn2);
        warpper2.setup(0,0,faceImage2.width,faceImage2.height);
        warpper2.load("warpper2_"+prefix+".xml");
        
    }
    void draw()
    {
        ofEnableAlphaBlending();
        backgroundImage.draw(0,0);
        
        ofPushMatrix();
        warpper.begin();
        faceImage.draw(0,0);
        warpper.end();
        warpper.draw();
        ofPopMatrix();
        
        ofPushMatrix();
        warpper2.begin();
        faceImage2.draw(0,0);
        warpper2.end();
        warpper2.draw();
        ofPopMatrix();
        
        overlayImage.draw(0,0);
        
    }
    void saveSetting()
    {
        FaceData::saveSetting();
        warpper.save("warpper2_"+prefix+".xml");
    }
};
class FaceMapper
{
public:
    void setup()
    {
        
    }
    void update()
    {
        
    }
};


#endif
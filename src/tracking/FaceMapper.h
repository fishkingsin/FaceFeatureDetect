//
//  FaceTracking.h
//  FaceFeatureDetect
//
//  Created by james KONG on 29/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

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
            save();
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
    void setup(string img_fn,string img_fn2, string setting_fn)
    {
        faceImage2.clear();
        FaceData::setup(img_fn, setting_fn);
        faceImage2.loadImage(img_fn2);
        warpper2.load("warpper2_"+prefix+".xml");
    }
    void draw()
    {
        ofEnableAlphaBlending();
        backgroundImage.draw(0,0);
        warpper.begin();
        faceImage.draw(0,0);
        warpper.end();
        warpper.draw();
        
        warpper2.begin();
        faceImage2.draw(0,0);
        warpper2.end();
        warpper2.draw();
        
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
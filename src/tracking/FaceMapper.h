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
class FaceData
{
public:
    ofImage backgroundImage;
    ofImage overlayImage;
    ofImage faceImage;
    ofMesh mapMesh;
    string imageFile;
    string prefix;
    void setup(string img_fn, string setting_fn)
    {
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
                xml.addTag("POINTS");
                if(xml.pushTag("POINTS"))
                {
                    
                    for(int i = 0; i < 4 ; i++)
                    {
                        xml.setValue("POINT:X",0.0f,i);
                        xml.setValue("POINT:Y",0.0f,i);
                        
                    }
                    xml.popTag();
                }
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
                mapMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
                if(xml.pushTag("POINTS"))
                {
                    int numTag = xml.getNumTags("POINT");
                    for(int i = 0; i < numTag ; i++)
                    {
                        float x = xml.getValue("POINT:X",0.0f);
                        float y = xml.getValue("POINT:Y",0.0f);
                        mapMesh.addVertex(ofVec2f(x,y));
                    }
                    xml.popTag();
                }
                mapMesh.addTexCoord(ofVec2f(0,0));
                mapMesh.addTexCoord(ofVec2f(faceImage.getWidth(),0));
                mapMesh.addTexCoord(ofVec2f(faceImage.getWidth(),faceImage.getHeight()));
                mapMesh.addTexCoord(ofVec2f(0,faceImage.getHeight()));

                xml.popTag();
            }
            save();
        }

    }
    void draw()
    {
        ofEnableAlphaBlending();
        backgroundImage.draw(0,0);
        //
        //faceImage.getTextureReference().bind();
        mapMesh.draw();
        //faceImage.draw(0,0);
        //faceImage.getTextureReference().unbind();
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
};

class FaceMapper
{
public:
    
};


#endif
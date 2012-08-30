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
#define gridRes 8
class FaceData
{
public:
    ofImage backgroundImage;
    ofImage overlayImage;
    ofImage faceImage;
    ofPoint tPt[4];
    ofPoint vPt[4];
    string imageFile;
    string prefix;
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
                //mapMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
                if(xml.pushTag("POINTS"))
                {
                    int numTag = xml.getNumTags("POINT");
                    for(int i = 0; i < numTag ; i++)
                    {
                        float x = xml.getValue("POINT:X",0.0f,i);
                        float y = xml.getValue("POINT:Y",0.0f,i);
                        vPt[i] = ofVec2f(x,y);
                        ofLog(OF_LOG_VERBOSE,"tpt %i x:%f y:%f" ,i,x,y );
                    }
                    xml.popTag();
                }
                tPt[0] = ofVec2f(0,0);
                tPt[1] = ofVec2f(faceImage.getWidth(),0);
                tPt[2] = ofVec2f(faceImage.getWidth(),faceImage.getHeight());
                tPt[3] = ofVec2f(0,faceImage.getHeight());

                xml.popTag();
            }
            save();
        }

    }
    void draw()
    {
        ofEnableAlphaBlending();
        backgroundImage.draw(0,0);
        faceImage.getTextureReference().bind();
        glBegin(GL_QUADS);
        
        
        glTexCoord2f(tPt[0].x, tPt[0].y);
        glVertex2f(vPt[0].x, vPt[0].y);
        glTexCoord2f(tPt[1].x, tPt[1].y);
        glVertex2f(vPt[1].x, vPt[1].y);
        glTexCoord2f(tPt[2].x, tPt[2].y);
        glVertex2f(vPt[2].x, vPt[2].y);
        glTexCoord2f(tPt[3].x, tPt[3].y);
        glVertex2f(vPt[3].x, vPt[3].y);
                
        glEnd();
        ofPoint bezSurfPoints[gridRes][gridRes];
        for(int i = 0; i <= gridRes; i++) {
            for(int j = 0; j <= gridRes; j++) {
                
                
                
                
                bezSurfPoints[i][j] = ofPoint(0, 0);
                
            } 
        } 

        faceImage.getTextureReference().unbind();
        for(int i = 0; i < gridRes; i++) {
            for(int j = 0; j < gridRes; j++) {
                faceImage.getTextureReference().bind();
                glBegin(GL_QUADS);  
                
                glTexCoord2f((i) * (faceImage.getWidth()/gridRes), (j) * (faceImage.getHeight()/gridRes));
                glVertex2f(bezSurfPoints[i][j].x, bezSurfPoints[i][j].y);  
                
                glTexCoord2f((i+1) * (faceImage.getWidth()/gridRes), (j) * (faceImage.getHeight()/gridRes));  
                glVertex2f(bezSurfPoints[i+1][j].x, bezSurfPoints[i+1][j].y);  
                
                glTexCoord2f((i+1) * (faceImage.getWidth()/gridRes), (j+1) * (faceImage.getHeight()/gridRes)); 
                glVertex2f(bezSurfPoints[i+1][j+1].x, bezSurfPoints[i+1][j+1].y);  
                
                glTexCoord2f((i) * (faceImage.getWidth()/gridRes), (j+1) * (faceImage.getHeight()/gridRes)); 
                glVertex2f(bezSurfPoints[i][j+1].x, bezSurfPoints[i][j+1].y);
                glEnd();  
                faceImage.getTextureReference().unbind(); 		
            }
        }

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
        
    }
};

class FaceMapper
{
public:
    
};


#endif
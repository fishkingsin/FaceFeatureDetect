#include "CountDown.h"

CountDown::CountDown()
{
    //ctor
    mCount = -1;;
    countImage = NULL;
    num = 0;
    bShow = false;
    scale = 2;
    bFlash = false;
    fireComplete  =true;
    //    rect.setColor(0,0,0);
    //    rect.setAlpha(0);
    //    rect.fadeOut();
    //    rect.setFadeSeconds(1);
}

CountDown::~CountDown()
{
    //dtor
}
void CountDown::setup(int _second)
{
    ofDirectory dir;
    dir.allowExt("png");
    num  = dir.listDir("images/counter");
    countImage = new ofImage[num];
    for (int i = 0 ; i < num ; i++)
    {
        countImage[i].loadImage(dir.getPath(i));
    }
    mTimer.setup(_second,true);
    stop();
    ofAddListener(mTimer.TIMER_REACHED,this,&CountDown::update);
    second = _second;
    shutter.loadSound("sounds/camera.wav");
    shutter.setVolume(0.5);
    button.loadSound("sounds/Morse.aiff");
    button.setVolume(0.5);
    shutter.setLoop(false);
    button.setLoop(false);
    //gui.addPage("CountDown");
    //    gui.addSlider("scale",scale,0,10);
    
}
void CountDown::update(ofEventArgs &args)
{
    if (mCount==num-1)
    {
        static ofEventArgs timerEventArgs;
        ofNotifyEvent(COUNTER_REACHED, timerEventArgs, this);
        ofLog(OF_LOG_NOTICE,string(typeid(*this).name())+" "+string(__FUNCTION__)+" Counter Reached");
        tween.setParameters(1,linearEase,ofxTween::easeOut,1,0,1000,500);
        fireComplete = false;
        shutter.play();
        stop();
        //rect.fadeIn();
        
        
    }
    else{
        button.play();
        ofLog(OF_LOG_NOTICE,"Counter : "+ofToString(mCount));
    }
    
    mCount++;
    
    //    if (mCount==num)
    //    {
    //        
    //        
    //         //rect.setAlpha(1);
    //        //rect.fadeOut();
    //    }
    //
    
    
    
    
    mCount%=num;
}
void CountDown::draw(float x ,float y, float w, float h)//bool blips)
{
    if (bShow)
    {
        ofPushStyle();
        ofEnableAlphaBlending();
        ofSetColor(255);
        ofFill();
        //for (int i = 0 ; i < num ; i++)
        if (mCount>-1)
        {
            int centerW = w/2;
            int centerH = h/2;
            ofImage *temp;
            /*if (mCount==num-2 && blips)
             {
             temp = &countImage[num-1];
             }
             else
             {*/
            temp = &countImage[mCount];
            //}
            float _w = temp->width*scale;
            float _h = temp->height*scale;
            
            int tW = _w/2;
            int tH = _h/2;
            int _x = centerW-tW;
            int _y = centerH-tH;
            
            temp->draw(x+_x,y+_y,_w,_h);
            
        }
        
        ofDisableAlphaBlending();
        ofPopStyle();
    }
    if(tween.isCompleted() && !fireComplete)
    {
        static ofEventArgs args;
        ofNotifyEvent(COMPLETE, args, this);
        fireComplete = true;
    }
    
    ofPushStyle();
    ofEnableAlphaBlending();
    ofFill();
    ofSetColor(255,255*tween.update());
    
    //rect.updateFade();
    ofRect(0,0,ofGetWidth(),ofGetHeight());
    ofDisableAlphaBlending();
    ofPopStyle();
    
    
}
void CountDown::start()
{
    mCount = 0;
    mTimer.startTimer();
    bShow = true;
}
void CountDown::stop()
{
    mTimer.stopTimer();
    bShow = false;
}

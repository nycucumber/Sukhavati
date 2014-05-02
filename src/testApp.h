#pragma once

//#define USE_TWO_KINECTS

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxOculusRift.h"
#include "ofx3DModelLoader.h"
#include "ofxUI.h"
#include "ofxOsc.h"
#include "particle.h"
#include "target.h"

#define PORT 4444
#define NUM_MSG_STRINGS 20

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void drawPointCloud();
    void closeKinect();
    void drawScene();
    
#ifdef USE_TWO_KINECTS
	ofxKinect kinect2;
    void drawAnotherPointCloud();
#endif
    ofSerial serial;
    //kinect
    ofxKinect kinect;
    //oculus rift
    int angle;
    ofxOculusRift oculusRift;
    ofEasyCam cam;
    ofVec3f camPos;
    ofx3DModelLoader roomModel;
    //lights setting
    ofLight pointLight;
    ofVec3f lightPos, lightDirection, roomModelPos, pointCloudPos,anotherPointCloudPos;
    ofColor lightColor;
    //texts
    ofTrueTypeFont zero;
    ofBoxPrimitive Room;
    float px,py,pz,p2x,p2y,p2z;
    bool showRoom;
    float kinectImageScale;
    float analogRead;
    float xangle,yangle,zangle;
    float x2angle,y2angle,z2angle;
    float roomRotateX,roomRotateY,roomRotateZ;
    //UI
    ofxUICanvas * gui;
    void exitUI();
    void guiEvent(ofxUIEventArgs &e);
    
    //particle system
    vector<particle> ps;
    vector<particle> ps2;
    bool firstRun2;
    bool firstRun;
    float meditationLevel;
    //OSC
    ofxOscReceiver receiver;
    
    //cam
    float camDistance;
    
    //sound
    ofSoundPlayer meditationSound;
    ofSoundPlayer distractionSound;
    
    
    //positions...
    float whole_scene_x,whole_scene_y,whole_scene_z;
    float scene_rx,scene_ry,scene_rz;
    
    ofShader blur;

    //Timer
    bool resetPosition;
    float ta, tb;
    bool raising;
    
    
};

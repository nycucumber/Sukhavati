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
#include "ofVbo.h"
#include "ofxAssimpModelLoader.h"


#define PORT 4444
#define NUM_MSG_STRINGS 20
#define TIME_TO_GETTING_TOGETHER 25

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
    void drawInitScene();
    void beginScnePositions();
    
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
    ofVideoPlayer tvScreen;
    ofxAssimpModelLoader tv;
    ofBoxPrimitive tvroom;
    ofMaterial tvroomMaterial;
   // ofxAssimpModelLoader roomModel;
    //lights setting
    ofLight light;
    ofVec3f lightPos, lightDirection, roomModelPos, pointCloudPos,anotherPointCloudPos;
    //texts
    ofTrueTypeFont zero;
    float px,py,pz,p2x,p2y,p2z;
    bool showRoom;
    bool showTV;
    float kinectImageScale;
    float analogRead;
    float xangle,yangle,zangle;
    float x2angle,y2angle,z2angle;
    float roomRotateX,roomRotateY,roomRotateZ;
    int step;
    //UI
    ofxUICanvas * gui;
    void exitUI();
    void guiEvent(ofxUIEventArgs &e);
    
    //particle system
    vector<particle> cloud;
    float cloudField;
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
    ofSoundPlayer bowlSound;
    ofSoundPlayer spaceSound;
    ofSoundPlayer pinkNoise;
    ofSoundPlayer aqua;
    float pinkNoiseVolume;
    bool bowlSoundSwitcher;
    
    //positions...
    float whole_scene_x,whole_scene_y,whole_scene_z, whole_scene_scale;
    float scene_rx,scene_ry,scene_rz;
    

    //Timer
    bool resetPosition;
   // float ta, tb;
    bool raising;
    
    
    //fianlly, the shader
    ofShader billboardShader;
    ofImage texture;
    float particleSize;
    
    
    //implementation part
    bool blackScreen;
    
    
    ofTrueTypeFont reading;
    
    //timing


    
    
    //plane
    float planeX, planeY, planeZ;
    
    //scene swtich
    bool scene_one;
    float tv_x, tv_y, tv_z;
    float image_x, image_y, image_z, image_scale;
    
    
    
};

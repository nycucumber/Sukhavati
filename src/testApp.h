#pragma once

#define USE_TWO_KINECTS

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxOculusRift.h"
#include "ofx3DModelLoader.h"



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
    
    
    ofxKinect kinect;
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
    float timeStampA;
    float timeStampB;
    float timeDifference;
    float analogRead;
    float xangle,yangle,zangle;
    float x2angle,y2angle,z2angle;

    float roomRotateX,roomRotateY,roomRotateZ;
    
    bool loseCalmness;
    
 
    
};

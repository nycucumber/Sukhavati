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
    
    ofxKinect kinect;
    int angle;
    ofxOculusRift oculusRift;
    ofEasyCam cam;
    ofVec3f camPos;
    ofx3DModelLoader roomModel;
    //lights setting
    ofLight pointLight;
    ofVec3f lightPos, lightDirection, roomModelPos, pointCloudPos;
    ofColor lightColor;
    //texts
    ofTrueTypeFont zero;
    ofBoxPrimitive Room;
    float px,py,pz;
    bool showRoom;
    float kinectImageScale;
    float timeStampA;
    float timeStampB;
    float timeDifference;
    
    float xangle,yangle,zangle;
    float roomRotateX,roomRotateY,roomRotateZ;
    
    bool loseCalmness;
    
};

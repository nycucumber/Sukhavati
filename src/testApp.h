#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxOculusRift.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
    //scene for oculus rift
    void drawScene();

   
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    //draw the point cloud
     void drawPointCloud();
    
    //kincet
    void closeKinect();
    
    ofxKinect kinect;
    ofxOculusRift oculusRift;
    ofLight light;
    ofCamera cam;
    ofVec3f camPos;
    
    bool bThresWithOpenCv;
    bool bDrawPointCloud;
    
    int nearThreshold;
    int farThreshold;
    
    int angle;
    int b = 1;
    int *a = &b;
    
    
		
};

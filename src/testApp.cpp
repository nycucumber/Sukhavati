#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofBackground(0, 0, 0);
    camPos.set(ofVec3f(1152,1572,0));
    cam.lookAt(ofVec3f(0,400,200));
    ofToggleFullscreen();
    ofSetLogLevel(OF_LOG_VERBOSE);
    //kinect code-----
    kinect.setRegistration();
    kinect.init();
    kinect.open();
#ifdef USE_TWO_KINECTS
    kinect2.init();
    kinect2.open();
#endif
    
    angle = 0;
    kinect.setCameraTiltAngle(angle);
    //set oculus rift camera to our ofcam
    oculusRift.baseCamera = &cam;
    oculusRift.setup();
    cam.begin();
    //Don't we need to draw stuff while camera is actived?
    cam.end();
    }

//--------------------------------------------------------------
void testApp::update(){
    
    ofBackground(0);
    oculusRift.baseCamera = &cam;
    if(oculusRift.isSetup()){
        ofRectangle viewport  = oculusRift.getOculusViewport();
    }
    
    kinect.update();
#ifdef USE_TWO_KINECTS
    kinect2.update();
#endif
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    
    
    if(oculusRift.isSetup()){
        ofSetColor(255, 255, 255);
        glEnable(GL_DEPTH_TEST);
        oculusRift.beginLeftEye();
		drawScene();
		oculusRift.endLeftEye();
		oculusRift.beginRightEye();
		drawScene();
		oculusRift.endRightEye();
		oculusRift.draw();
		glDisable(GL_DEPTH_TEST);//ANYBODY TELL ME WHAT DOES THIS DO?
    }
	else{
		cam.begin();
		drawScene();
		cam.end();

    }
}
//--------------------------------------------------------------

void testApp::drawScene()
{
	
//	ofPushMatrix();
//	ofRotate(90, 0, 0, -1);
//	ofDrawGridPlane(500.0f, 10.0f, false );
//	ofPopMatrix();
//	
	ofPushStyle();
    
    if(oculusRift.isSetup()){
        
    //////////PUSH MATRIX//////////////
		ofPushMatrix();
        //Don't know what is this...
		oculusRift.multBillboardMatrix();
        //Draw Room
        ofNoFill();
        ofDrawBox(0, ofGetHeight()/2, -800, 2400,2400,4000 );
        //coordinate
        ofSetColor(255, 0, 0);
        ofLine(0, 0, 0, 800, 0,0);
        ofSetColor(0, 255, 0);
        ofLine(0, 0, 0, 0,800,0);
        ofSetColor(0, 0, 255);
        ofLine(0, 0, 0, 0, 0, 800);
        ofPopStyle();
        //DRAW THE 1ST KINECT IMAGE
        ofSetColor(255,255,255);
        drawPointCloud();
        ofPopStyle();
        //DRAW THE 2ND KINECT IMAGE
        ofSetColor(255, 0, 0);
        drawAnotherPointCloud();
        ofPopStyle();
		ofPopMatrix();
        
    //////////POP MATRIX//////////////

        
	}
	
	
    
}

//--------------------------------------------------------------
void testApp::drawPointCloud(){
    int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 10;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0 && kinect.getDistanceAt(x,y) < 1300) {
				//mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                
			}
		}
	}
	glPointSize(1);
    //////////PUSH MATRIX//////////////

	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(1, -1, -1);
	ofTranslate(-100, -400, -100); // center the points a bit

	ofEnableDepthTest();
    
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
    //////////PUSH MATRIX//////////////

    
    
    
}

#ifdef USE_TWO_KINECTS
void testApp::drawAnotherPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh2;
	mesh2.setMode(OF_PRIMITIVE_POINTS);
	int step = 10;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect2.getDistanceAt(x, y) > 0 && kinect2.getDistanceAt(x,y) < 1200) {
				//mesh2.addColor(kinect2.getColorAt(x,y));
				mesh2.addVertex(kinect2.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(1);
    //////////PUSH MATRIX//////////////

	ofPushMatrix();
    ofScale(-1, -1, 1);
	ofTranslate(48, -214, -1550); // center the points a bit
	ofEnableDepthTest();
	mesh2.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
    //////////PUSH MATRIX//////////////

}
#endif


//--------------------------------------------------------------

void testApp::closeKinect(){
    kinect.setCameraTiltAngle(0);
    kinect.close();
    
#ifdef USE_TWO_KINECTS
    //kinect2.setCameraTiltAngle(0);
	kinect2.close();
#endif
    
    
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    switch (key) {
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30)angle=-30;
            kinect.setCameraTiltAngle(angle);
            break;
            
        case OF_KEY_UP:
            angle++;
            if(angle>30)angle=30;
            kinect.setCameraTiltAngle(angle);
            break;
            
        case 'x':
            camPos += ofVec3f(20,0,0);
            ofLog()<<"camera's position is: "<<cam.getPosition();
            
        case 'X':
            camPos += ofVec3f(-20,0,0);
            ofLog()<<"camera's position is: "<<cam.getPosition();
            
            
        case 'y':
            camPos -= ofVec3f(0,20,0);
              ofLog()<<"camera's position is: "<<cam.getPosition();
            
        case 'Y':
            camPos -= ofVec3f(0,-20,0);
            ofLog()<<"camera's position is: "<<cam.getPosition();
            
        case 'z':
            camPos -= ofVec3f(0,0,20);
              ofLog()<<"camera's position is: "<<cam.getPosition();
        case 'Z':
            camPos += ofVec3f(0,0,-20);
              ofLog()<<"camera's position is: "<<cam.getPosition();
        
          
            
    }
    
    

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

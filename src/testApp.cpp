#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofBackground(0, 0, 0);
    //ofToggleFullscreen();
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    //kinect code-----
    kinect.setRegistration();
    kinect.init();
    kinect.open();
   
    
    angle = 0;
    kinect.setCameraTiltAngle(angle);

    
    //oculus code----
    oculusRift.baseCamera = &cam;
    oculusRift.setup();
    //Don't we need to draw stuff while camera is actived?
    cam.begin();
    cam.end();
    
    camPos.set(0, 0,0);
    
    
    }

//--------------------------------------------------------------
void testApp::update(){
    
        
    oculusRift.baseCamera = &cam;
    if(oculusRift.isSetup()){
        ofRectangle viewport  = oculusRift.getOculusViewport();
    }
    
    kinect.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    if(oculusRift.isSetup()){
        ofSetColor(255, 255, 200);
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
		
		ofPushMatrix();
		oculusRift.multBillboardMatrix();
		ofSetColor(255, 0, 0);
        
        ofNoFill();
        ofDrawBox(0, ofGetHeight()/2, -800, 2400,2400,4000 );
        drawPointCloud();
        
        
     
      
        // camPos.set(ofVec3f(ofGetWidth()/2,1500,1400));
        // cam.setPosition(camPos);
      
        cam.lookAt(ofVec3f(0,400,200));
        
        
		//ofCircle(0,0,.5);
		ofPopMatrix();
        
	}
	
	ofPopStyle();
    
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
	glPointSize(3);
    
	ofPushMatrix();
    ofSetColor(255, 255, 255);
    
	// the projected points are 'upside down' and 'backwards'
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -2000); // center the points a bit
	ofEnableDepthTest();
    
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
    
    
    
}
//--------------------------------------------------------------

void testApp::closeKinect(){
    kinect.setCameraTiltAngle(0);
    kinect.close();
    
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
            
        case 'w':
            camPos += ofVec3f(0,0,1);
          ofLog()<<"camera's position is: "<<cam.getPosition();
        case 's':
            camPos -= ofVec3f(0,0,1);
              ofLog()<<"camera's position is: "<<cam.getPosition();
        case 'a':
            camPos -= ofVec3f(-1,0,0);
              ofLog()<<"camera's position is: "<<cam.getPosition();
        case 'd':
            camPos += ofVec3f(1,0,0);
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

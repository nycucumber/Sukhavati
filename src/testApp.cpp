#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    //general
    ofBackground(0, 0, 0);
    ofSetVerticalSync(true);
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableSmoothing();
    
    //lights
    ofSetSmoothLighting(true);
    pointLight.setPointLight();
    lightPos.set(ofVec3f(0,0,1500)); //point light position
    pointLight.setPosition(lightPos);
    pointLight.setPointLight();
    
    
    //3d model
    roomModel.loadModel("room.3ds");
    roomModel.setPosition(0, 0, 0);
    roomModel.setScale(50, 50, 50);
    
    //kinect
    kinect.setRegistration();
    kinect.init();
    kinect.open();
#ifdef USE_TWO_KINECTS
    kinect2.init();
    kinect2.open();
#endif
    angle = 0;
    kinect.setCameraTiltAngle(angle);
    
    //osculus rift
    oculusRift.baseCamera = &cam;
    cam.setDistance(500.f);
    oculusRift.setup();
    //text
    zero.loadFont("Helvetica.dfont", 5);
    ofToggleFullscreen();
    cam.begin();
    cam.end();
    
}

//--------------------------------------------------------------
void testApp::update(){
    cout<<cam.getX()<<","<<cam.getY()<<","<<cam.getZ();
    if(oculusRift.isSetup()){
        ofRectangle viewport  =  oculusRift.getOculusViewport();
    }
    //update Kinects
    kinect.update();
#ifdef USE_TWO_KINECTS
    kinect2.update();
#endif
    //ofLog() << "FarClip -> "<<cam.getFarClip()<< " Fov -> "  <<cam.getFov() << " NearClip -> " << cam.getNearClip();
    
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
        
        
    }else{
        cout<<"oculus rift not connected";
    }
    
    
}
//--------------------------------------------------------------

void testApp::drawScene()
{
	
    
    
    
    ofPushStyle();
    pointLight.enable();
    //Don't know what is this...
    oculusRift.multBillboardMatrix();
    
    
    //DRAW THE 1ST KINECT IMAGE
    ofPushMatrix();
    drawPointCloud();
#ifdef USE_TWO_KINECTS
    //DRAW THE 2ND KINECT IMAGE
    drawAnotherPointCloud();
#endif
    ofPopMatrix();
    
    //Draw 3D Room
    ofPushMatrix();
    ofSetColor(255, 255, 255);
    ofRotateX(270);
    ofTranslate(0,0,-1000);
    roomModel.draw();
    ofPopMatrix();
    

    pointLight.disable();
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
                if(x == 10 && y == 0){
                    ofLog() << " points cloud position: " << kinect.getWorldCoordinateAt(x, y);
                }
			}
		}
	}
    
	glPointSize(1);
    //////////PUSH MATRIX//////////////
	ofPushMatrix();
    ofPushStyle();
	// the projected points are 'upside down' and 'backwards'
	ofEnableDepthTest();
    ofSetColor(255);
	mesh.drawVertices();
	ofDisableDepthTest();
    ofPopStyle();
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
    ofPushStyle();

	ofEnableDepthTest();
    ofSetColor(255, 0, 0);
	mesh2.drawVertices();
	ofDisableDepthTest();
    ofPopStyle();
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
    
    int step = 500;
    
    
    switch (key) {
            
        case 'n':
            angle--;
            if(angle<-30)angle=-30;
            kinect.setCameraTiltAngle(angle);
            break;
        case 'm':
            angle++;
            if(angle>30)angle=30;
            kinect.setCameraTiltAngle(angle);
            break;
            
            //modify camera position by x,y,z key
        case 'x':
            camPos += ofVec3f(step,0,0);
        case 'X':
            camPos += ofVec3f(-step,0,0);
        case 'y':
            camPos += ofVec3f(0,step,0);
        case 'Y':
            camPos += ofVec3f(0,-step,0);
        case 'z':
            camPos += ofVec3f(0,0,step);
        case 'Z':
            camPos += ofVec3f(0,0,-step);
    }
    ofLog()<<"cam pos: "<<cam.getPosition();
    
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

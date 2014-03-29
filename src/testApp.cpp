#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    //general
    //ofBackground(0, 0, 0);
    ofSetVerticalSync(true);
    ofToggleFullscreen();
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofEnableSmoothing();
    
    // turn on smooth lighting
    ofSetSmoothLighting(true);
    lightPos.set(ofVec3f(0,0,0));
    ambientLight.setPosition(lightPos);
    
    //camera
    
    cam.setPosition(ofVec3f(0,-1000,0));
    //cam.lookAt(ofVec3f(0,1,0), ofVec3f(0,1,0));
    
    cam.begin();
    cam.end();//Don't we need to draw stuff while camera is actived?
    
    
    //3d model
    roomModel.loadModel("room.3ds");
    roomModel.setPosition(0, 0, 0);
    
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
    
    oculusRift.setup();
    
    //text
    zero.loadFont("Helvetica.dfont", 5);
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    ofBackground(0);
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
    ambientLight.enable();
    
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
    
    //////////PUSH MATRIX//////////////
    ofPushMatrix();
    ofScale(10, 10);
    roomModel.draw();
    ofPopMatrix();
    //////////POP MATRIX//////////////
    ambientLight.disable();
    
    
}
//--------------------------------------------------------------

void testApp::drawScene()
{
	
    //	ofPushMatrix();
    //	ofRotate(90, 0, 0, -1);
    //	ofDrawGridPlane(500.0f, 10.0f, false );
    //	ofPopMatrix();
    //
	    if(oculusRift.isSetup()){
       
        //////////PUSH MATRIX//////////////
		ofPushMatrix();
        //Don't know what is this...
		oculusRift.multBillboardMatrix();
        //Draw Room
        ofNoFill();
        ofDrawBox(0, 0, -800, 2400,3000,4000 );
        //coordinate
        ofSetColor(255, 0, 0);
        ofLine(0, 0, 0, 800, 0,0);
        ofSetColor(0, 255, 0);
        ofLine(0, 0, 0, 0,800,0);
        ofSetColor(0, 0, 255);
        ofLine(0, 0, 0, 0, 0, 800);
        ofPopStyle();
        ofSetColor(255);
        zero.drawString("0,0,0", 0, 0);
        ofDrawGrid();
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
	ofTranslate(48, -214, -1550);
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

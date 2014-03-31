#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    //general
    ofBackground(0, 0, 0);
    ofSetVerticalSync(true);
    
    ofEnableSmoothing();
    

    //lights
    ofSetSmoothLighting(true);
    pointLight.setPointLight();
    lightPos.set(ofVec3f(0,0,400)); //point light position
    pointLight.setPosition(lightPos);
    
    
    //3d model
    roomModel.loadModel("room.3ds");
    roomModel.setScale(50, 50, 50);
    roomModelPos.set(-90,-270,-930);
//    roomModelPos.set(0,0,0);

    showRoom = true;
    

    //kinect
    kinect.setRegistration();
    kinect.init();
    kinect.open();
    kinectImageScale = 0.3;
#ifdef USE_TWO_KINECTS
    kinect2.init();
    kinect2.open();
#endif
    
    
    angle = 0;
    kinect.setCameraTiltAngle(angle);
    //point cloud translation values:
    px = -100;
    py = -510;
    pz = 500;
    
    //osculus rift
    oculusRift.baseCamera = &cam;
    cam.setDistance(400.f);
    oculusRift.setup();
    
    
    //text
    zero.loadFont("Helvetica.dfont", 5);
    cam.begin();
    cam.end();
    
    //breath stats
    loseCalmness = false;
    timeStampA=0;
    timeStampB=0;
    
    //ofToggleFullscreen();

    
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    pointLight.setPosition(lightPos);
    
    pointCloudPos.set(px,py,pz);

   //timer
    if (loseCalmness) {
        timeStampB=ofGetElapsedTimef();
        timeDifference = timeStampB-timeStampA;
        
        cout<<timeDifference<<"|||" <<loseCalmness <<endl;
        if(timeDifference>5){
            loseCalmness = false;
        }
        
}
    
    
    
    
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
        oculusRift.beginLeftEye();
        drawScene();
        oculusRift.endLeftEye();
        
        oculusRift.beginRightEye();
        drawScene();
        oculusRift.endRightEye();
        
        oculusRift.draw();
        
    }else{
        cam.begin();
        drawScene();
        cam.end();
    }
    
    
}
//--------------------------------------------------------------

void testApp::drawScene()
{
	
    
    pointLight.enable();
    
    ofPushStyle();
    
    //pointLight.draw();
    
    
    
//    oculusRift.multBillboardMatrix();
    
    
    //DRAW THE 1ST KINECT IMAGE
    ofPushMatrix();
    drawPointCloud();
#ifdef USE_TWO_KINECTS
    //DRAW THE 2ND KINECT IMAGE
    drawAnotherPointCloud();
#endif
    ofPopMatrix();
    
    
    if(showRoom){
    ofEnableDepthTest();
    //--------Draw 3D Room-------
    ofPushMatrix();
    ofSetColor(255, 255, 255);
    ofRotateX(270);
    ofTranslate(roomModelPos);
    roomModel.draw();
    ofPopMatrix();
    //--------Draw 3D Room-------
    }
    
//    
//    //our geo. room
//    ofPushMatrix();
//    ofSetColor(255, 255, 255);
//    ofTranslate(roomModelPos);
//    ofNoFill();
//    Room.draw();
//    Room.drawWireframe();
//    ofPopMatrix();
//    
    
    
    ofPopStyle();
    pointLight.disable();
    
}

//--------------------------------------------------------------
void testApp::drawPointCloud(){
    int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
    if(loseCalmness==false){
        int step = 10;
        for(int y = 0; y < h; y += step) {
            for(int x = 0; x < w; x += step) {
                if(kinect.getDistanceAt(x, y) > 0 && kinect.getDistanceAt(x,y) < 1300) {
                    //mesh.addColor(kinect.getColorAt(x,y));
                    mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
                    //  ofLog() << " points cloud position: " << kinect.getWorldCoordinateAt(x, y);
                }
            }
        }
    }else{
        int step = 10;
        for(int y = 0; y < h; y += step) {
            for(int x = 0; x < w; x += step) {
                if(kinect.getDistanceAt(x, y) > 0 && kinect.getDistanceAt(x,y) < 1300) {
                    //mesh.addColor(kinect.getColorAt(x,y));
                    mesh.addVertex(kinect.getWorldCoordinateAt(x+ofRandom(-400+(timeDifference*80),400-(timeDifference*80)), y+ofRandom(-400+(timeDifference*80),400-(timeDifference*80))));
                    //  ofLog() << " points cloud position: " << kinect.getWorldCoordinateAt(x, y);
                }
            }
        }
    }
    
    
    
    //////////PUSH MATRIX//////////////
	ofPushMatrix();
    ofPushStyle();
    glPointSize(1);
    ofTranslate(pointCloudPos);
	ofEnableDepthTest();
    ofSetColor(0,0,0);
    ofScale(kinectImageScale,kinectImageScale,kinectImageScale);
    ofRotateY(90);
    ofRotateZ(90);
    ofRotateX(90);
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
    ofScale(kinectImageScale,kinectImageScale,kinectImageScale);
    ofSetColor(255, 0, 0);
    ofTranslate(pointCloudPos);
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
    
    float step = 30;
    ofVec3f *lightpos = &lightPos;
    
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
            //            *lightpos -= ofVec3f(-step,0,0);
            roomModelPos  += ofVec3f(step, 0,0);
            break;
        case 'X':
            //            *lightpos += ofVec3f(-step,0,0);
            roomModelPos += ofVec3f(-step,0,0);
            break;
        case 'y':
            //            *lightpos += ofVec3f(0,step,0);
            roomModelPos += ofVec3f(0,step,0);
            break;
            
        case 'Y':
            //            *lightpos += ofVec3f(0,-step,0);
            roomModelPos += ofVec3f(0,-step,0);
            break;
            
        case 'z':
            //            *lightpos += ofVec3f(0,0,step);
            roomModelPos += ofVec3f(0,0,step);
            break;
            
        case 'Z':
            //            *lightpos += ofVec3f(0,0,-step);
            roomModelPos += ofVec3f(0,0,-step);
            break;
            
        case 'r':
            cout <<" the ROOM's position is:  " <<  roomModelPos << endl;
            break;
            
        case 'l':
            cout << " the Light's position is:  " << lightPos << endl;
            break;
            
        case 'p':
            pz+=10;
            cout<< "Point Cloud's  position is "<< px<<","<<py<<","<<pz<<endl;
            break;
            
            
        case 'P':
            pz-=10;
            cout<< "Point Cloud's  position is "<< px<<","<<py<<","<<pz<<endl;
            break;
            
        case 'o':
            py+=10;
            cout<< "Point Cloud's  position is "<< px<<","<<py<<","<<pz<<endl;
            break;
            
        case 'O':
            py-=10;
            cout<< "Point Cloud's  position is "<< px<<","<<py<<","<<pz<<endl;
            break;
            
        case '[':
            px-=10;
            cout<< "Point Cloud's  position is "<< px<<","<<py<<","<<pz<<endl;
            break;
            
        case ']':
            px+=10;
            cout<< "Point Cloud's  position is "<< px<<","<<py<<","<<pz<<endl;
            break;
            
        case 'b':
            showRoom = !showRoom;
            break;
            
            
        case 'q':
            loseCalmness = true;
            timeStampA=ofGetElapsedTimef();
            break;
            
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

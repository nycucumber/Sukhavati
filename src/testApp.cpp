#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    //general
    ofBackground(0, 0, 0);
    //ofSetVerticalSync(true);
    ofEnableSmoothing();
    
    //kinect one image rotate parameters
    xangle = 179;
    yangle = 86;
    zangle = 0;
    //kinect two image rotate parameters
    x2angle = 457;
    y2angle = 181;
    z2angle = -3;
    
    //roomRotate
    roomRotateX=270;
    roomRotateY=0;
    roomRotateZ=0;
    
    //lights
    ofSetSmoothLighting(true);
    pointLight.setPointLight();
    lightPos.set(ofVec3f(0,0,400)); //point light position
    pointLight.setPosition(lightPos);
    
    
    //3d model
    roomModel.loadModel("room.3ds");
    roomModel.setScale(50, 50, 50);
    roomModelPos.set(-90,-270,-930);
    //roomModelPos.set(0,0,0);
    
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
    
    //point cloud translation values:
    px = -160;
    py = -530;
    pz = 280;
    
    p2x = -320;
    p2y = -510;
    p2z = 290;
    
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
    
   // ofToggleFullscreen();
    
    
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    //light position
    pointLight.setPosition(lightPos);
    //kinect #one position
    pointCloudPos.set(px,py,pz);
    //kinect #two position
    anotherPointCloudPos.set(p2x,p2y,p2z);
    
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
    pointLight.draw();
    
    //DRAW THE 1ST KINECT IMAGE
    ofPushMatrix();
    drawPointCloud();
    ofPopMatrix();
#ifdef USE_TWO_KINECTS
    //DRAW THE 2ND KINECT IMAGE
    ofPushMatrix();
    drawAnotherPointCloud();
    ofPopMatrix();
    
#endif
    
    
    
    if(showRoom){
        ofEnableDepthTest();
        //--------Draw 3D Room-------
        ofPushMatrix();
        ofSetColor(255, 255, 255);
        ofRotateX(270);
        ofRotateY(roomRotateY);
        ofTranslate(roomModelPos);
        roomModel.draw();
        ofPopMatrix();
    }
    
    ofPopStyle();
    pointLight.disable();
    
}

//--------------------------------------------------------------
void testApp::drawPointCloud(){
    int w = 640;
	int h = 480;
	ofMesh mesh;
    
	mesh.setMode(OF_PRIMITIVE_POINTS);
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
    //mess particles around
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
    ofRotateY(yangle);
    ofRotateZ(zangle);
    ofRotateX(xangle);
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
    if(loseCalmness==false){
        int step = 10;
        for(int y = 0; y < h; y += step) {
            for(int x = 0; x < w; x += step) {
                if(kinect2.getDistanceAt(x, y) > 0 && kinect2.getDistanceAt(x,y) < 1300) {
                    //mesh.addColor(kinect.getColorAt(x,y));
                    mesh2.addVertex(kinect2.getWorldCoordinateAt(x, y));
                    //  ofLog() << " points cloud position: " << kinect.getWorldCoordinateAt(x, y);
                }
            }
        }
    }
    else{
        int step = 10;
        for(int y = 0; y < h; y += step) {
            for(int x = 0; x < w; x += step) {
                if(kinect2.getDistanceAt(x, y) > 0 && kinect2.getDistanceAt(x,y) < 1300) {
                    //mesh.addColor(kinect.getColorAt(x,y));
                    mesh2.addVertex(kinect2.getWorldCoordinateAt(x+ofRandom(-400+(timeDifference*80),400-(timeDifference*80)), y+ofRandom(-400+(timeDifference*80),400-(timeDifference*80))));
                    //  ofLog() << " points cloud position: " << kinect.getWorldCoordinateAt(x, y);
                }
            }
        }
    }
    
    
    
    //////////PUSH MATRIX//////////////
	ofPushMatrix();
    ofPushStyle();
    ofEnableDepthTest();
    
    glPointSize(1);
    ofTranslate(anotherPointCloudPos);
    ofSetColor(255,0,0);
    ofScale(kinectImageScale,kinectImageScale,kinectImageScale);
    ofRotateY(x2angle);
    ofRotateZ(y2angle);
    ofRotateX(z2angle);
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
            
/////////////////////////modify camera position by x,y,z key////////////////////////////////////////////////
        case 'x':
            //            *lightpos -= ofVec3f(-step,0,0);
            xangle +=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            
            break;
        case 'X':
            //            *lightpos += ofVec3f(-step,0,0);
            xangle-=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            
            break;
        case 'y':
            //            *lightpos += ofVec3f(0,step,0);
            yangle+=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            
            break;
            
        case 'Y':
            //            *lightpos += ofVec3f(0,-step,0);
            yangle-=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            
            break;
            
        case 'z':
            //            *lightpos += ofVec3f(0,0,step);
            zangle+=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            
            break;
            
        case 'Z':
            //            *lightpos += ofVec3f(0,0,-step);
            zangle-=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            break;
            
/////////////////////////////////rotate kinect 2//////////////////////////////////////////////
            
            //modify camera position by x,y,z key
        case 'j':
            //            *lightpos -= ofVec3f(-step,0,0);
            x2angle +=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            
            break;
        case 'J':
            //            *lightpos += ofVec3f(-step,0,0);
            x2angle-=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            
            break;
        case 'k':
            //            *lightpos += ofVec3f(0,step,0);
            y2angle+=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            
            break;
            
        case 'K':
            //            *lightpos += ofVec3f(0,-step,0);
            y2angle-=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            
            break;
            
        case 'l':
            //            *lightpos += ofVec3f(0,0,step);
            z2angle+=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            
            break;
            
        case 'L':
            //            *lightpos += ofVec3f(0,0,-step);
            z2angle-=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            break;
            
        //////////KINECT ONE POSITION---------------
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
            
        //////////////KINECT TWO position   ---------------
        case 'f':
            p2z+=10;
            cout<< "Point 2 Cloud's  position is "<< p2x<<","<<p2y<<","<<p2z<<endl;
            break;
            
            
        case 'F':
            p2z-=10;
            cout<< "Point 2 Cloud's  position is "<< p2x<<","<<p2y<<","<<p2z<<endl;
            break;
            
        case 'g':
            p2y+=10;
            cout<< "Point 2 Cloud's  position is "<< p2x<<","<<p2y<<","<<p2z<<endl;
            break;
            
        case 'G':
            p2y-=10;
            cout<< "Point 2 Cloud's  position is "<< p2x<<","<<p2y<<","<<p2z<<endl;
            break;
            
        case 'h':
            p2x-=10;
            cout<< "Point 2 Cloud's  position is "<< p2x<<","<<p2y<<","<<p2z<<endl;
            break;
            
        case 'H':
            p2x+=10;
            cout<< "Point 2 Cloud's  position is "<< p2x<<","<<p2y<<","<<p2z<<endl;
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

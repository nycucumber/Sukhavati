#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    //general
    ofBackground(255, 255, 255);
    ofSetFrameRate(30);
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
   // pointLight.setPointLight();
    lightPos.set(ofVec3f(0,0,400)); //point light position
    pointLight.setPosition(lightPos);
    
    
    //3d model
    roomModel.loadModel("noneRoom.3ds");
     roomModel.setScale(50, 50, 50);
    //roomModel.setScale(400,400,400);
    roomModelPos.set(-90,-270,-930);
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

    
   // ofToggleFullscreen();
    
    //Serial Communication
    serial.listDevices();
    serial.setup();
    
    //GUI
    gui = new ofxUICanvas();
    gui->addSlider("Meditation_Level",0.0,100.0,100.0);
    gui->addToggle("FULLSCREEN", false);

    gui->autoSizeToFitWidgets();
  
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
    
    //osc
    meditationLevel = 100;
    receiver.setup(PORT);
    
    //particle system
    
    
}

//--------------------------------------------------------------
void testApp::update(){

    
    //osc:
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        if (m.getAddress() == "/meditation") {
            // cout<<m.getArgAsFloat(0)<<endl;
            meditationLevel = ofMap(m.getArgAsFloat(0), 0, 100, 0, 120);
            cout<<meditationLevel<<endl;
        }
    }
    
    //serial
    
    if(serial.available()>0){
        analogRead = serial.readByte();
        cout<< "serial input data -> " <<analogRead<<endl;
    }
    
    
    //light position
    pointLight.setPosition(lightPos);
    //kinect #one position
    pointCloudPos.set(px,py,pz);
    //kinect #two position
    anotherPointCloudPos.set(p2x,p2y,p2z);
    
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
	
    
   // pointLight.enable();
    
    ofPushStyle();
    
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
   // pointLight.disable();
    
}

//--------------------------------------------------------------
void testApp::drawPointCloud(){
    if(kinect.isFrameNew()){
     
        
        if(meditationLevel>=90){
            ofMesh mesh;
            ofMesh kinectData;
            kinectData.setMode(OF_PRIMITIVE_POINTS);
            mesh.setMode(OF_PRIMITIVE_POINTS);
            int w = 640;
            int h = 480;
            int step = 9;
            vector<target> targets;
            for(int y=0;y<h;y+=step){
                for(int x=0;x<w;x+=step){
                    if(kinect.getDistanceAt(x, y)>0&&kinect.getDistanceAt(x, y)<1500){
                        mesh.addVertex(ofVec3f(kinect.getWorldCoordinateAt(x, y)));
                    }
                }
            }
            ofPushMatrix();
            ofSetColor(0, 0, 0);
            // ofScale(1,-1,1);
            glPointSize(2);
            ofTranslate(pointCloudPos);
            ofEnableDepthTest();
            ofScale(kinectImageScale, kinectImageScale, kinectImageScale);
            ofRotateY(yangle);
            ofRotateZ(zangle);
            ofRotateX(xangle);
            mesh.draw();
            ofPopMatrix();
        }else{
            
            ofMesh mesh;
            ofMesh kinectData;
            kinectData.setMode(OF_PRIMITIVE_POINTS);
            mesh.setMode(OF_PRIMITIVE_POINTS);
            int w = 640;
            int h = 480;
            int step = 9;
            vector<target> targets;
            for(int y=0;y<h;y+=step){
                for(int x=0;x<w;x+=step){
                    if(kinect.getDistanceAt(x, y)>0&&kinect.getDistanceAt(x, y)<1500){
                        targets.push_back(ofVec3f(kinect.getWorldCoordinateAt(x, y)));
                        if(firstRun == false){
                            ps.push_back(ofVec3f(kinect.getWorldCoordinateAt(x, y)));
                        }
                        
                    }
                }
            }
            firstRun = true;
            
            
            //compare the amount between our Particle Vector and all kinect particles
            while(targets.size() > ps.size()){
                ps.push_back(ofVec3f(ofRandom(-300,300), ofRandom(-300,300),800));
            }
            //===============SET TARGETS===============
            for(int i=0;i<ps.size();i++){
                float minDistance = 100000;
                float index = 0;
                
                for(int b=0;b<targets.size();b++){
                    if(targets[b].choosen==false){
                        float distance;
                        distance = ps[i].location.distance(targets[b].location);
                        if(distance < minDistance){
                            minDistance = distance;
                            index = b;
                        }
                    }
                }
                // cout<<"the "<<i<<"'th particle's"<<" minimum distance is "<<minDistance<<" .......his index in [targets] is: "<<index<<endl;
                ps[i].seek(targets[index].location);
                ps[i].target_assigned = true;
                targets[index].choosen = true;
                ps[i].update();
                //adjust movement parameter based on Meditation Level
                ps[i].maxforce = ofMap(meditationLevel, 0, 90, 0.001, 5);
                ps[i].maxspeed = ofMap(meditationLevel, 0, 90, 20, 5);
                mesh.addVertex(ps[i].getPosition());
                
            }
            
            //=========GET CENTER POINT LOCATION=========
            //
            //        ofVec3f centerPoint;
            //        ofVec3f Sum;
            //        Sum.set(0, 0,0);
            //        int count = 0;
            //
            //        for(int i=0;i<ps.size();i+=20){
            //            count++;
            //            Sum+=ps[i].location;
            //        }
            //
            //        centerPoint = Sum / count;
            //        cout<<centerPoint<<endl;
            //
            //        for (int i = 0; i<ps.size(); i++){
            //            if(ps[i].target_assigned==false){
            //                ps[i].seek(centerPoint+ofVec3f(ofRandom(-300,300),ofRandom(-300,300),ofRandom(-300,300)));
            //            }
            //        }
            //
            
            
            
            
            //==================JUST DRAW===============
            ofPushMatrix();
            ofSetColor(0, 0, 0);
           // ofScale(1,-1,1);
            glPointSize(2);
            ofTranslate(pointCloudPos);
            ofEnableDepthTest();
            ofScale(kinectImageScale, kinectImageScale, kinectImageScale);
            ofRotateY(yangle);
            ofRotateZ(zangle);
            ofRotateX(xangle);
            mesh.draw();
            ofPopMatrix();
            
            //  cout<<targets.size()<<endl;  //particles amount
            
            
            //        for (int i = 0; i<ps.size(); i++){
            //            ps[i].target_assigned = false;
            //        }
            //
            
        }//end [meditationlevel < 80] if_statment
    }//end the [kinect.frameNew] if_statement
    
    
}



#ifdef USE_TWO_KINECTS
void testApp::drawAnotherPointCloud() {
    if(kinect2.isFrameNew()){
        
        
        if(meditationLevel>=90){
            ofMesh mesh;
            ofMesh kinectData;
            kinectData.setMode(OF_PRIMITIVE_POINTS);
            mesh.setMode(OF_PRIMITIVE_POINTS);
            int w = 640;
            int h = 480;
            int step = 13;
            vector<target> targets;
            for(int y=0;y<h;y+=step){
                for(int x=0;x<w;x+=step){
                    if(kinect2.getDistanceAt(x, y)>0&&kinect2.getDistanceAt(x, y)<1500){
                        mesh.addVertex(ofVec3f(kinect2.getWorldCoordinateAt(x, y)));
                    }
                }
            }
            ofPushMatrix();
            ofSetColor(0, 0, 0);
            // ofScale(1,-1,1);
            glPointSize(2);
            ofTranslate(pointCloudPos);
            ofEnableDepthTest();
            ofScale(kinectImageScale, kinectImageScale, kinectImageScale);
            ofRotateY(yangle);
            ofRotateZ(zangle);
            ofRotateX(xangle);
            mesh.draw();
            ofPopMatrix();
        }else{
            ofMesh mesh;
            ofMesh kinectData;
            kinectData.setMode(OF_PRIMITIVE_POINTS);
            mesh.setMode(OF_PRIMITIVE_POINTS);
            int w = 640;
            int h = 480;
            int step = 9;
            vector<target> targets;
            for(int y=0;y<h;y+=step){
                for(int x=0;x<w;x+=step){
                    if(kinect2.getDistanceAt(x, y)>0 && kinect2.getDistanceAt(x, y)<1500){
                        targets.push_back(ofVec3f(kinect2.getWorldCoordinateAt(x, y)));
                        if(firstRun2 == false){
                            ps2.push_back(ofVec3f(kinect2.getWorldCoordinateAt(x, y)));
                        }
                        
                    }
                }
            }
            firstRun2 = true;
            
            
            //compare the amount between our Particle Vector and all kinect particles
            while(targets.size() > ps2.size()){
                ps2.push_back(ofVec3f(ofRandom(-300,300), ofRandom(-300,300),800));
            }
            //===============SET TARGETS===============
            for(int i=0;i<ps2.size();i++){
                float minDistance = 100000;
                float index = 0;
                
                for(int b=0;b<targets.size();b++){
                    if(targets[b].choosen==false){
                        float distance;
                        distance = ps2[i].location.distance(targets[b].location);
                        if(distance < minDistance){
                            minDistance = distance;
                            index = b;
                        }
                    }
                }
                // cout<<"the "<<i<<"'th particle's"<<" minimum distance is "<<minDistance<<" .......his index in [targets] is: "<<index<<endl;
                ps2[i].seek(targets[index].location);
                ps2[i].target_assigned = true;
                targets[index].choosen = true;
                ps2[i].update();
                //adjust movement parameter based on Meditation Level
                ps2[i].maxforce = ofMap(meditationLevel, 0, 90, 0.001, 5);
                ps2[i].maxspeed = ofMap(meditationLevel, 0, 90, 20, 5);
                mesh.addVertex(ps2[i].getPosition());
                
            }
            
            //=========GET CENTER POINT LOCATION=========
            //
            //        ofVec3f centerPoint;
            //        ofVec3f Sum;
            //        Sum.set(0, 0,0);
            //        int count = 0;
            //
            //        for(int i=0;i<ps.size();i+=20){
            //            count++;
            //            Sum+=ps[i].location;
            //        }
            //
            //        centerPoint = Sum / count;
            //        cout<<centerPoint<<endl;
            //
            //        for (int i = 0; i<ps.size(); i++){
            //            if(ps[i].target_assigned==false){
            //                ps[i].seek(centerPoint+ofVec3f(ofRandom(-300,300),ofRandom(-300,300),ofRandom(-300,300)));
            //            }
            //        }
            //
            
            
            
            
            //==================JUST DRAW===============
            ofPushMatrix();
            ofPushStyle();
            ofSetColor(200, 0, 0);
            ofEnableDepthTest();
            // ofScale(1,-1,1);
            glPointSize(2);
            ofTranslate(anotherPointCloudPos);
            ofEnableDepthTest();
            ofScale(kinectImageScale, kinectImageScale, kinectImageScale);
            ofRotateY(y2angle);
            ofRotateZ(z2angle);
            ofRotateX(x2angle);
            mesh.draw();
            ofPopStyle();
            ofPopMatrix();
            
            
            //        for (int i = 0; i<ps.size(); i++){
            //            ps[i].target_assigned = false;
            //        }
            //
            
        }//end [meditationlevel < 80] if_statment
    }//end the [kinect.frameNew] if_statement
    
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

void testApp::exitUI(){
    gui->saveSettings("settings.xml");
    delete gui;
    
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
    
    
    if(e.getName() == "FULLSCREEN")
    {
        ofxUIToggle *toggle = e.getToggle();
        ofSetFullscreen(toggle->getValue());
        
    }else if(e.getName() == "Meditation_Level")
    {
        ofxUISlider *slider = e.getSlider();
        meditationLevel = slider->getScaledValue();
    }
    
    
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

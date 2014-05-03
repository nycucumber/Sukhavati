#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    //general
    ofBackground(255);
    //kinect one image rotate parameters
    xangle = 25;
    yangle = -79;
    zangle = -14; //-14
    //kinect two image rotate parameters
    x2angle = 0;
    y2angle = 94;
    z2angle = 0;
    
    //roomRotate
    roomRotateX=270;
    roomRotateY=0;
    roomRotateZ=0;
    
    //3d model
    roomModel.loadModel("final.3ds");
    roomModel.setScale(50, 50, 50);
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
    px = 170;
    py = 370;
    pz = -560;
    
    
    p2x = -280;
    p2y = 400;
    p2z = -260;
    
    
    step = 9;
    
    //osculus rift
    oculusRift.baseCamera = &cam;
    camDistance = 400.f;
    cam.setDistance(camDistance);
    oculusRift.setup();
    
    
    //camera
    cam.disableMouseInput();
    cam.begin();
    cam.end();
    
    //Serial Communication
    serial.listDevices();
    serial.setup();
    
    
    //GUI
    gui = new ofxUICanvas();
    gui->setTheme(0);
    gui->addFPS();
    gui->addSlider("Meditation_Level",0.0,100.0,100.0);
    gui->addSlider("background_color", 0.f, 255.f, 0.f);
    gui->addSlider("particle_Size", 0.1, 5.f, 2.f);
    gui->addSlider("Whole_Scene_Position_x", -600.f, 600.f, 404.f);
    gui->addSlider("Whole_Scene_Position_y", -600.f, 600.f, 333.f);
    gui->addSlider("Whole_Scene_Position_z", -600.f, 600.f, -304.f);
    gui->addSlider("Whole_Scene_Rotate_x", 0.f, 360.f, 81.f);
    gui->addSlider("Whole_Scene_Rotate_y", 0.f, 360.f, 266.f);
    gui->addSlider("Whole_Scene_Rotate_z", 0.f, 360.f, 83.f);
    gui->addToggle("FULLSCREEN", false);
    //    gui->addLabel("Point Cloud A-rotate-x, y, z//potition:p(x), o(y), [](z)////Point Cloud B - rotate:j(x), k(y), l(z)//position:h(x), g(y),f(z)");
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
    
    
    //osc
    meditationLevel = 100;
    receiver.setup(PORT);
    
    
    //sound
    meditationSound.loadSound("BOWL.mp3");
    meditationSound.setVolume(1.3);
    meditationSound.setMultiPlay(false);
    meditationSound.play();
    meditationSound.setLoop(true);
    distractionSound.loadSound("whitenoise.mp3");
    distractionSound.setVolume(0);
    distractionSound.play();
    distractionSound.setLoop(true);
    
    //y is the flying height
    
    whole_scene_x = 404;
    whole_scene_y = 333;
    whole_scene_z = -304;
    
    scene_rx = 81;
    scene_ry = 266;
    scene_rz = 83;
    
    //Primitive Box -  Room
    // room.set(900);
    
    resetPosition = true;
    
    //shader::
    if(ofGetGLProgrammableRenderer()){
		billboardShader.load("shadersGL3/Billboard");
	}else{
		billboardShader.load("shadersGL2/Billboard");
	}
    ofDisableArbTex();
	texture.loadImage("dot.png");
    particleSize = 2;
}
//--------------------------------------------------------------
void testApp::update(){
    //    room.setPosition(0, 0, 0);
    ofSoundUpdate();
    //OSC::::
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        if (m.getAddress() == "/meditation") {
            meditationLevel =m.getArgAsFloat(0);
            cout<<meditationLevel<<endl;
        }
    }
    //Sound Update
    distractionSound.setVolume(ofMap(meditationLevel, 0, 100, 1, 0.5));
    meditationSound.setVolume(ofMap(meditationLevel, 0, 100, 0, 1));
    //serial input
    if(serial.available()>0){
        analogRead = serial.readByte();
        cout<< "serial input data -> " <<analogRead<<endl;
    }
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
    
    //reset initial position for next user
    if(resetPosition == true){
        whole_scene_x = 404;
        whole_scene_y = 333;
        whole_scene_z = -304;
        scene_rx = 81;
        scene_ry = 266;
        scene_rz = 83;
        ta = ofGetElapsedTimef();
        raising = true;
        resetPosition = false;
    }
    
    if(raising){
        whole_scene_y -= 0.1;
        whole_scene_x -= 0.06;
    }
    if(whole_scene_y < -300 && raising){
        raising = false;
    }
    
    
    
    
}
//--------------------------------------------------------------
void testApp::draw(){
    if(oculusRift.isSetup()){
        ofNoFill();
        
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
    ofPushMatrix();
    ofRotateX(scene_rx);
    ofRotateY(scene_ry);
    ofRotateZ(scene_rz);
    ofTranslate(whole_scene_x, whole_scene_y, whole_scene_z);
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
    ofEnableDepthTest();
    ofPushMatrix();
    ofSetColor(255);
    ofRotateX(270);
    ofRotateY(roomRotateY);
    ofTranslate(roomModelPos);
    roomModel.draw();
    ofPopMatrix();
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::drawPointCloud(){
    //when user is in meditation mode, show the clear image of his/her body
    if(meditationLevel >= 80){
        ofVboMesh mesh;
        mesh.setUsage(GL_DYNAMIC_DRAW);
        mesh.setMode(OF_PRIMITIVE_POINTS);
        mesh.getNormals().resize(5000,ofVec3f(0));  //????????????????????????????????????????
        int w = 640;
        int h = 480;
        int i = 0;
        float t = (ofGetElapsedTimef()) * 0.9f;
        vector<target> targets;
        for(int y=0;y<h;y+=step){
            for(int x=0;x<w;x+=step){
                if(kinect.getDistanceAt(x, y)>0&&kinect.getDistanceAt(x, y)<1500){
                    mesh.addVertex(ofVec3f(kinect.getWorldCoordinateAt(x, y)));
                    mesh.setNormal(i,ofVec3f(particleSize+ofNoise(t+i),0,0));
                    i++;
                }
            }
        }
        ofPushMatrix();
        ofSetColor(255);
        //   glDisable(GL_DEPTH_TEST);
        
        //point one flip function...
        ofScale(-1,-1,1);
        ofRotateY(yangle);
        ofRotateZ(zangle);
        ofRotateX(xangle);
        ofTranslate(pointCloudPos);
        
        ofScale(kinectImageScale, kinectImageScale, kinectImageScale);
        billboardShader.begin();
        ofEnablePointSprites();
        texture.getTextureReference().bind();
        mesh.draw();
        texture.getTextureReference().unbind();
        ofDisablePointSprites();
        billboardShader.end();
        //glEnable(GL_DEPTH_TEST);
        
        ofPopMatrix();
    }else{
        ofVboMesh mesh;
        mesh.setUsage(GL_DYNAMIC_DRAW);
        mesh.setMode(OF_PRIMITIVE_POINTS);
        int w = 640;
        int h = 480;
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
            ps.push_back(ofVec3f(ofRandom(-300,300), ofRandom(-300,300) , 800));
        }
        //set normals
        mesh.getNormals().resize(ps.size(),ofVec3f(0));  //????????????????????????????????????????
        //===============SET TARGETS===============
        for(int i=0;i<ps.size();i++){
            //=========== if meditationlevel is too low, make particles move totally randomly ======
            if (meditationLevel < 15) {
                
                ps[i].seek(ofVec3f(
                                   ofNoise(ofGetElapsedTimef())*1000000*ofRandom(-1,1),
                                   ofNoise(ofGetElapsedTimef())*1000000*ofRandom(-1,1),
                                   ofNoise(ofGetElapsedTimef())*1000000*ofRandom(-1,1)
                                   )
                           );
                
                
                
                ps[i].update();
                ps[i].maxforce = ofNoise(ofGetElapsedTimef());
                ps[i].maxspeed = ofNoise(ofGetElapsedTimef()) * 20;
            }else{
                
                float minDistance = 10000000000000;
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
                //apply targets to each particle
                //                float randomN = ofRandom(0,100);
                //                float afterMap =  ofMap(meditationLevel, 10, 80, 50, 3);
                //                if(randomN > 0 && randomN < afterMap){
                //                    ps[i].seek(ofVec3f(
                //                                       ofGetElapsedTimef() * 100000 ,
                //                                       ofGetElapsedTimef() * 100000 ,
                //                                       ofGetElapsedTimef() * 100000
                //                                       )
                //                               );
                //                    ps[i].target_assigned = true;
                //                    targets[index].choosen = true;
                //                    ps[i].update();
                //                    //adjust movement parameter based on Meditation Level
                //                    ps[i].maxforce = ofNoise(ofGetElapsedTimeMicros()) * 1;
                //                    ps[i].maxspeed = ofNoise(ofGetElapsedTimeMicros()) * 30;
                //
                //                    // make a few particles to move completely randomly
                //                }else{
                ps[i].seek(targets[index].location);
                ps[i].target_assigned = true;
                targets[index].choosen = true;
                ps[i].update();
                //adjust movement parameter based on Meditation Level
                if(meditationLevel > 60){
                    ps[i].maxforce = ofNoise(ofGetElapsedTimeMicros()) * 2;
                    ps[i].maxspeed = ofNoise(ofGetElapsedTimeMicros()) * 10;
                }else if(meditationLevel > 40 && meditationLevel <= 60){
                    ps[i].maxforce = ofNoise(ofGetElapsedTimeMicros()) * 1;
                    ps[i].maxspeed = ofNoise(ofGetElapsedTimeMicros()) * 7;
                }else{
                    ps[i].maxforce = ofNoise(ofGetElapsedTimeMicros()) / 2 ;
                    ps[i].maxspeed = ofNoise(ofGetElapsedTimeMicros()) * 16;
                }
                //                }
            }
            mesh.addVertex(ps[i].getPosition());
            mesh.setNormal(i,ofVec3f(particleSize+ofNoise(ofGetElapsedTimef()+i),0,0));
            i++;
            
            
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
        ofSetColor(255);
        //point one flip function...
        // ofScale(1,-1,1);
        ofScale(-1, -1,1);
        ofRotateY(yangle);
        ofRotateZ(zangle);
        ofRotateX(xangle);
        ofTranslate(pointCloudPos);
        ofScale(kinectImageScale, kinectImageScale, kinectImageScale);
        billboardShader.begin();
        ofEnablePointSprites();
        texture.getTextureReference().bind();
        mesh.draw();
        texture.getTextureReference().unbind();
        ofDisablePointSprites();
        billboardShader.end();
        ofPopStyle();
        ofPopMatrix();
    }
}

#ifdef USE_TWO_KINECTS
void testApp::drawAnotherPointCloud() {
    if(kinect2.isFrameNew()){
        if(meditationLevel >= 80 && meditationLevel< 95){
            ofMesh mesh;
            mesh.setMode(OF_PRIMITIVE_POINTS);
            int w = 640;
            int h = 480;
            vector<target> targets;
            for(int y=0;y<h;y+=step){
                for(int x=0;x<w;x+=step){
                    if(kinect2.getDistanceAt(x, y)>0&&kinect2.getDistanceAt(x, y)<1500){
                        mesh.addVertex(ofVec3f(kinect2.getWorldCoordinateAt(x, y)));
                    }
                }
            }
            ofPushMatrix();
            ofSetColor(0,0,0);
            //point two flip function...
            ofScale(-1,-1,1);
            glPointSize(1);
            ofRotateY(y2angle);
            ofRotateZ(z2angle);
            ofRotateX(x2angle);
            ofTranslate(anotherPointCloudPos);
            ofScale(kinectImageScale, kinectImageScale, kinectImageScale);
            
            mesh.draw();
            ofPopMatrix();
        }else{
            ofMesh mesh;
            ofMesh kinectData;
            kinectData.setMode(OF_PRIMITIVE_POINTS);
            mesh.setMode(OF_PRIMITIVE_POINTS);
            int w = 640;
            int h = 480;
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
                ps2[i].maxforce = ofMap(meditationLevel, 0, 85, 0.01, 5);
                ps2[i].maxspeed = ofMap(meditationLevel, 0, 85, 20, 5);
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
            ofSetColor(0,0,0);
            ofScale(-1,-1,1);
            glPointSize(1);
            ofRotateY(y2angle);
            ofRotateZ(z2angle);
            ofRotateX(x2angle);
            ofTranslate(anotherPointCloudPos);
            ofScale(kinectImageScale, kinectImageScale, kinectImageScale);
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
    }else if(e.getName() == "camera_distance"){
        ofxUISlider *slider = e.getSlider();
        camDistance = slider->getScaledValue();
    }else if(e.getName() == "background_color"){
        ofxUISlider *slider = e.getSlider();
        ofBackground(slider->getScaledValue());
    }else if(e.getName() == "Whole_Scene_Position_x"){
        ofxUISlider *slider = e.getSlider();
        whole_scene_x = slider->getScaledValue();
    }else if(e.getName() == "Whole_Scene_Position_y"){
        ofxUISlider *slider = e.getSlider();
        whole_scene_y = slider->getScaledValue();
    }else if(e.getName() == "Whole_Scene_Position_z"){
        ofxUISlider *slider = e.getSlider();
        whole_scene_z = slider->getScaledValue();
    }else if(e.getName() == "Whole_Scene_Rotate_x"){
        ofxUISlider *slider = e.getSlider();
        scene_rx = slider->getScaledValue();
    }else if(e.getName() == "Whole_Scene_Rotate_y"){
        ofxUISlider *slider = e.getSlider();
        scene_ry = slider->getScaledValue();
    }else if(e.getName() == "Whole_Scene_Rotate_z"){
        ofxUISlider *slider = e.getSlider();
        scene_rz = slider->getScaledValue();
    }else if(e.getName() == "particle_Size"){
        ofxUISlider *slider = e.getSlider();
        particleSize = slider->getScaledValue();
    }
    
    
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    //  float step = 30;
    
    switch (key) {
            
            /////////////////////////modify camera position by x,y,z key////////////////////////////////////////////////
        case 'x':
            xangle +=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            
            break;
        case 'X':
            xangle-=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            
            break;
        case 'y':
            yangle+=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            
            break;
            
        case 'Y':
            yangle-=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            
            break;
            
        case 'z':
            zangle+=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            
            break;
            
        case 'Z':
            zangle-=1;
            cout<<xangle<<","<<yangle<<","<<zangle<<endl;
            break;
            
            /////////////////////////////////rotate kinect 2//////////////////////////////////////////////
            
        case 'j':
            x2angle +=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            
            break;
        case 'J':
            x2angle-=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            
            break;
        case 'k':
            y2angle+=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            
            break;
            
        case 'K':
            y2angle-=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            break;
        case 'l':
            z2angle+=1;
            cout<<x2angle<<","<<y2angle<<","<<z2angle<<endl;
            break;
        case 'L':
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
            
        case'm':
            meditationLevel++;
            break;
        case'M':
            meditationLevel--;
            break;
        case 'r':
            resetPosition = true;
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

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    //general
    ofBackground(255);
    
    //switch scene
    scene_one = true;
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
    roomModel.loadModel("APR14.3ds");
    roomModel.setScale(50, 50, 50);
    roomModelPos.set(-90,-270,-930);
    showRoom = true;
    showTV = true;
    
    
    //tv model
    tv.loadModel("tv.3ds");
    tv.setScale(2, 2, 2);
    tv.setPosition(tv_x, tv_y, tv_z);
    tvScreen.loadMovie("tvScreen.gif");
    tvScreen.play();
    
    
    //whole scene position
    beginScnePositions();
    
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
    
    //point cloud 2 position values:
    p2x = -280;
    p2y = 400;
    p2z = -260;
    
    step = 10;
    
    //osculus rift
    oculusRift.baseCamera = &cam;
    oculusRift.setup();
    cam.enableMouseInput();
    cam.enableMouseMiddleButton();
    cam.begin();
    cam.end();
    
    //plane
    planeX = 768.f;
    planeY = 0.f;
    planeZ = 0.f;
    //GUI
    gui = new ofxUICanvas();
    //    gui->setTheme();
    gui->addFPS();
    gui->addSlider("Meditation_Level",0.0,100.0,100.0);
    gui->addSlider("background_color", 0.f, 255.f, 0.f);
    gui->addSlider("particle_Size", 0.1, 5.f, 2.f);
    float a = 4000.f;
    gui->addSlider("Whole_Scene_Position_x", -a, a, whole_scene_x);
    gui->addSlider("Whole_Scene_Position_y", -a, a, whole_scene_y);
    gui->addSlider("Whole_Scene_Position_z", -a, a, whole_scene_z);
    
    gui->addSlider("Whole_Scene_Rotate_x", 0.f, 360.f, scene_rx);
    gui->addSlider("Whole_Scene_Rotate_y", 0.f, 360.f, scene_ry);
    gui->addSlider("Whole_Scene_Rotate_z", 0.f, 360.f, scene_rz);
    
    gui->addSlider("whole_scene_scale", -0.01, 20, whole_scene_scale);
    gui->addSlider("PlaneX",-a,a,planeX);
    gui->addSlider("PlaneY",-a,a,planeY);
    gui->addSlider("PlaneZ",-a,a,planeZ);
    
    gui->addSlider("tv_x", -1000, 1000, tv_x);
    gui->addSlider("tv_y", -1000, 1000, tv_y);
    gui->addSlider("tv_z", -1000, 1000, tv_z);
    
    gui->addSlider("gif_x", -1000, 1000, image_x);
    gui->addSlider("gif_y", -1000, 1000, image_y);
    gui->addSlider("gif_z", -1000, 1000, image_z);
    gui->addSlider("gif_scale", -0.1, 1, 1);
    
    
    
    gui->addToggle("FULLSCREEN", false);
    //    gui->addLabel("Point Cloud A-rotate-x, y, z//potition:p(x), o(y), [](z)////Point Cloud B - rotate:j(x), k(y), l(z)//position:h(x), g(y),f(z)");
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
    //osc
    meditationLevel = 100;
    receiver.setup(PORT);
    
    
    //sound
    bowlSound.loadSound("BOWL.mp3");
    bowlSound.setVolume(0);
    bowlSound.setMultiPlay(false);
    bowlSound.play();
    bowlSound.setLoop(true);
    bowlSoundSwitcher = false;
    
    spaceSound.loadSound("whitenoise.mp3");
    spaceSound.setVolume(0);
    spaceSound.play();
    spaceSound.setLoop(true);
    
    
    aqua.loadSound("Aqua_Syndrome_I.mp3");
    aqua.play();
    aqua.setLoop(true);
    
    
    //LIGHTING
    //ahahahahahahahahahahah
    
    
    
    resetPosition = true;
    
    //shader::
    if(ofGetGLProgrammableRenderer()){
		billboardShader.load("shadersGL3/Billboard");
	}else{
		billboardShader.load("shadersGL2/Billboard");
	}
    ofDisableArbTex();
    texture.loadImage("dot2.png");
    ofEnableAlphaBlending();
    particleSize = 4;
    
    
    //implementation
    blackScreen = true;
    cam.setFarClip(20000);
    
    //Timing:

    raising = true;
    
    //Ambient Cloud
    cloudField  = 3000;
    for (int i=0; i< 2000; i++) {
        cloud.push_back(ofVec3f(ofRandom(-cloudField,cloudField),ofRandom(-cloudField,cloudField),ofRandom(-cloudField,cloudField)));
    }
    
    
    
    //tv room
    tvroom.set(1800);
    tvroomMaterial.setShininess( 10 );
    tvroomMaterial.setDiffuseColor(ofColor(0,0,0));
	tvroomMaterial.setSpecularColor(ofColor(200, 200, 200, 255));
    
}
//--------------------------------------------------------------
void testApp::update(){
    
    //AMBIENT CLOUD MOVEMENT
    for (int i=0; i<cloud.size(); i++){
        if(ofGetFrameNum()%30==0){
            cloud[i].seek(ofVec3f(ofRandom(-cloudField,cloudField),ofRandom(-cloudField,cloudField),ofRandom(-cloudField,cloudField)));
        }
        cloud[i].update();
    }
    
    //OSC::::
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        if (m.getAddress() == "/meditation") {
                meditationLevel =m.getArgAsFloat(0);
        }
    }
    
    //SET SOUND VOLUME BASED ON PARAMETERS
    if(scene_one==false){
        spaceSound.setVolume(ofMap(meditationLevel, 0, 100, 1, 0));
        bowlSound.setVolume(ofMap(meditationLevel, 0, 100, 0, 1));
        aqua.setVolume(0);
    }else{
        spaceSound.setVolume(0);
        bowlSound.setVolume(0);
        aqua.setVolume(1);
    }
    
    //SET KINECT PARTICLES POSITIONS
    pointCloudPos.set(px,py,pz);
    anotherPointCloudPos.set(p2x,p2y,p2z);
    
    //SETUP OCULUS RIFT
    if(oculusRift.isSetup()){
        ofRectangle viewport  =  oculusRift.getOculusViewport();
    }
    
    
    //reset initial position for next user
    
    if(resetPosition == true){
        beginScnePositions();
        raising = true;
        meditationLevel = 0;
        resetPosition = false;
    }
//    
//    if(scene_one == false){ //when we get into scene two
//        //when we are in scene two, stop [aqua]
//        if(raising && !blackScreen){
//            if(whole_scene_x < 295){
//                whole_scene_x+=10;
//            }
//            if(whole_scene_y < -610){
//                whole_scene_y+=10;
//            }
//        }
//        if(whole_scene_x >= 295 && whole_scene_y >= -610){
//            raising=false;
//        }
//    }
//    
//    
    
    
    //UPLOAD ALL SOUNDS
    ofSoundUpdate();
    tvScreen.update();
    
    if(!scene_one){
        
    //update Kinects
    kinect.update();
#ifdef USE_TWO_KINECTS
    kinect2.update();
#endif
        
    }
    
   
}
//--------------------------------------------------------------
void testApp::draw(){
    
    
    
    if (blackScreen) {
        ofClear(255, 255, 255);
    }else{
        if(oculusRift.isSetup()){
           
//            ---------------LEFT EYE------------
            oculusRift.beginLeftEye();

            if(scene_one){
                drawInitScene();
            }else{
                drawScene();
            }
            
            oculusRift.endLeftEye();
            
//            ---------------RIGHT EYE-----------
            oculusRift.beginRightEye();
            
            if(scene_one){
                drawInitScene();
            }else{
                drawScene();
            }
            
            oculusRift.endRightEye();
            
            oculusRift.draw();
            
        }else{
            cam.begin();
            
            if(scene_one){
                drawInitScene();
            }else{
                drawScene();
            }
            
            cam.end();
        }
    }
    
    
}
//--------------------------------------------------------------
void testApp::drawInitScene(){

//    ofDrawGrid(1000.0f, 8.0f, true, true, true, true);

    

    ofEnableDepthTest();
    tvroom.drawFaces();

    light.enable();
    light.setPosition(0, 0, tv_z + 50);
    light.draw();
    tv.setScale(ofMap(tv_z, -1000, 0, 0.1, 1), ofMap(tv_z, -1000, 0, 0.1, 1), ofMap(tv_z, -1000, 0, 0.1, 1));
    ofTranslate(tv_x, tv_y,tv_z);
//    tv.setPosition(tv_x, tv_y, tv_z);
    tv.drawFaces();
    ofScale(image_scale * ofMap(tv_z, -1000, 0, 0.1, 1), image_scale * ofMap(tv_z, -1000, 0, 0.1, 1),image_scale * ofMap(tv_z, -1000, 0, 0.1, 1));
    ofRotateX(180);
    ofTranslate(image_x, image_y, image_z);

    tvScreen.draw(0, 0);


  
    light.disable();

    
}
//--------------------------------------------------------------

void testApp::drawScene()
{

    light.enable();
    ofPushMatrix();
    
    
    ofScale(whole_scene_scale,whole_scene_scale,whole_scene_scale);
    ofRotateX(scene_rx);
    ofRotateY(scene_ry);
    ofRotateZ(scene_rz);
    ofTranslate(whole_scene_x, whole_scene_y, whole_scene_z);
    ofPushMatrix();

    
    ofVboMesh theCloud;
    theCloud.setUsage(GL_DYNAMIC_DRAW);
    theCloud.setMode(OF_PRIMITIVE_POINTS);
    theCloud.getNormals().resize(cloudField,ofVec3f(0));
    for (int i=0; i<cloud.size();i++) {
        theCloud.addVertex(cloud[i].location);
        theCloud.setNormal(i,ofVec3f(
                                     ofMap(meditationLevel, 0, 100, 5, 0)
                                     +ofNoise(ofGetElapsedTimef()+i),0,0));
        
    }
    
    billboardShader.begin();
    glDisable(GL_DEPTH_TEST);
    ofEnablePointSprites();
    texture.getTextureReference().bind();
    theCloud.draw();
    texture.getTextureReference().unbind();
    ofDisablePointSprites();
    billboardShader.end();
    glEnable(GL_DEPTH_TEST);
	ofPopMatrix();
    
    
    //   ----   room   ----
    
    if(showRoom){
        ofPushMatrix();
        ofEnableDepthTest();
        ofSetColor(255);
        ofRotateX(270);
        ofRotateY(roomRotateY);
        ofTranslate(roomModelPos);
        roomModel.draw();
        ofPopMatrix();
    }
    //   ----   end room   ----
    
    ofPushMatrix();
    drawPointCloud();
    ofPopMatrix();
#ifdef USE_TWO_KINECTS
    ofPushMatrix();
    drawAnotherPointCloud();
    ofPopMatrix();
#endif
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::drawPointCloud(){
    //===================================================
    //DEEP MEDITATION MODE===============================
    //↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
    //===================================================
    if(meditationLevel >= 80){
        ofVboMesh mesh;
        mesh.setUsage(GL_DYNAMIC_DRAW);
        mesh.setMode(OF_PRIMITIVE_POINTS);
        mesh.getNormals().resize(5000,ofVec3f(0));
        int w = 640;
        int h = 480;
        int i = 0;
        float t = (ofGetElapsedTimef()) * 0.9f;
        vector<target> targets;
        for(int y=0;y<h;y+=step){
            for(int x=0;x<w;x+=step){
                if(kinect.getDistanceAt(x, y) > 200 && kinect.getDistanceAt(x, y) < 1300){
                    mesh.addVertex(ofVec3f(kinect.getWorldCoordinateAt(x, y)));
                    mesh.setNormal(i,ofVec3f(particleSize+ofNoise(t+i),0,0));
                    i++;
                }
            }
        }
        ofPushMatrix();
        ofSetColor(255);
        glDisable(GL_DEPTH_TEST);
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
        glEnable(GL_DEPTH_TEST);
        ofPopMatrix();
        //===================================================
        //LOW MEDITATION MODE===============================
        //↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
        //===================================================
    }else{
        ofVboMesh mesh;
        mesh.setUsage(GL_DYNAMIC_DRAW);
        mesh.setMode(OF_PRIMITIVE_POINTS);
        int w = 640;
        int h = 480;
        //Get original data from kinect and strore them into 'targets'
        vector<target> targets;
        for(int y=0;y<h;y+=step){
            for(int x=0;x<w;x+=step){
                if(kinect.getDistanceAt(x, y) > 200 && kinect.getDistanceAt(x, y) < 1300){
                    targets.push_back(ofVec3f(kinect.getWorldCoordinateAt(x, y)));
                    //arrange all of the particle to the correct position in the first time so they won't seek the same target...
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
        mesh.getNormals().resize(ps.size(),ofVec3f(0));
        //===============SET TARGETS===============
        for(int i=0;i<ps.size();i++){
            //=========== if meditationlevel is too low, make particles move totally randomly ======
            if (meditationLevel < 15) {
                if(ofGetFrameNum()%20==0){
                    
                    
                    ps[i].seek(ofVec3f(
                                       ofRandom(-6000,6000),ofRandom(-6000,6000),ofRandom(-6000,6000)
                                       )
                               );
                }
                ps[i].update();
            }else{
                float minDistance = 1000000;
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
                ps[i].seek(targets[index].location
                           + ofVec3f(
                                     ofNoise(ofGetElapsedTimef()) * 2000 * ofMap(meditationLevel, 15, 80, 1, 0) * ofRandom(-1,1),
                                     ofNoise(ofGetElapsedTimef()) * 2000 * ofMap(meditationLevel, 15, 80, 1, 0) * ofRandom(-1,1),
                                     ofNoise(ofGetElapsedTimef()) * 2000 * ofMap(meditationLevel, 15, 80, 1, 0) * ofRandom(-1,1)
                                     )
                           );
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
                }else if(meditationLevel >= 15){
                    ps[i].maxforce = ofNoise(ofGetElapsedTimeMicros()) / 2 ;
                    ps[i].maxspeed = ofNoise(ofGetElapsedTimeMicros()) * 16;
                }else{
                    if(ofGetFrameNum()%50==0){
                        ps[i].maxforce = ofRandom(-0.01,0.1);
                        ps[i].maxspeed = ofRandom(1,3);
                    }
                }
            }
            mesh.addVertex(ps[i].getPosition());
            mesh.setNormal(i,ofVec3f(particleSize+ofNoise(ofGetElapsedTimef()+i),0,0));
            i++;
        }
        //==================JUST DRAW===============
        ofPushMatrix();
        ofPushStyle();
        ofSetColor(255);
        //point one flip function...
        // ofScale(1,-1,1);
        glDisable(GL_DEPTH_TEST);
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
        glEnable(GL_DEPTH_TEST);
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
    }else if(e.getName() == "PlaneX"){
        ofxUISlider *slider = e.getSlider();
        planeX = slider->getScaledValue();
    }else if(e.getName() == "PlaneY"){
        ofxUISlider *slider = e.getSlider();
        planeY = slider->getScaledValue();
    }else if(e.getName() == "PlaneZ"){
        ofxUISlider *slider = e.getSlider();
        planeZ = slider->getScaledValue();
    }else if(e.getName() == "whole_scene_scale"){
        ofxUISlider *slider = e.getSlider();
        whole_scene_scale = slider->getScaledValue();
    }else if(e.getName() == "tv_x"){
        ofxUISlider *slider = e.getSlider();
        tv_x = slider->getScaledValue();
    }else if(e.getName() == "tv_y"){
        ofxUISlider *slider = e.getSlider();
        tv_y = slider->getScaledValue();
    }else if(e.getName() == "tv_z"){
        ofxUISlider *slider = e.getSlider();
        tv_z = slider->getScaledValue();
    }else if(e.getName() == "gif_x"){
        ofxUISlider *slider = e.getSlider();
        image_x = slider->getScaledValue();
    }else if(e.getName() == "gif_y"){
        ofxUISlider *slider = e.getSlider();
        image_y = slider->getScaledValue();
    }else if(e.getName() == "gif_z"){
        ofxUISlider *slider = e.getSlider();
        image_z = slider->getScaledValue();
    }else if(e.getName() == "gif_scale"){
        ofxUISlider *slider = e.getSlider();
        image_scale = slider->getScaledValue();
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
            
        case 'b':
            blackScreen = !blackScreen;
            break;
            
        case 's':
            scene_one = !scene_one;
            break;
            
    }
    
    
    
    
    
}

//--------------------------------------------------------------
void testApp::beginScnePositions(){
    whole_scene_x = 295;
    whole_scene_y = -610;
    whole_scene_z = -304;
    scene_rx = 81;
    scene_ry = 266;
    scene_rz = 83;
    whole_scene_scale = 1;
    image_x = -261.08;
    image_y = 54.89;
    image_z = -17.78;
    image_scale = 0.62;
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
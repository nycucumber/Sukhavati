//
//  particle.cpp
//  Sukhavati
//
//  Created by Yang Wang on 4/23/14.
//
//

#include "particle.h"

particle::particle(ofVec3f _location){
    acceleration = ofVec3f(0,0,0);
    velocity = ofVec3f(0,0,0);
    location = _location;
    maxspeed = ofRandom(3,10);
    //    maxspeed = 20;
    maxforce = ofRandom(0.1,0.4);
    //    maxforce = 10;
    target_assigned = false;
}


void particle::update(){
    velocity += acceleration;
    velocity.limit(maxspeed);
    location += velocity;
    acceleration = acceleration * 0;
    
}

void particle::seek(ofVec3f target){
    ofVec3f desired = target - location;
    desired.normalize();
    desired *= maxspeed;
    ofVec3f steer = desired - velocity;
    steer.limit(maxforce);
    acceleration += steer;
}


void particle::display(){
    float size = 1;
    ofPushMatrix();
    ofSetColor(255, 255, 255);
    ofDrawBox(location.x, location.y, location.z, size, size, size);
    ofPopMatrix();
}


ofVec3f particle::getPosition() {
    return location;
}
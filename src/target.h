//
//  target.h
//  Sukhavati
//
//  Created by Yang Wang on 4/23/14.
//
//

#ifndef __Sukhavati__target__
#define __Sukhavati__target__

#include <iostream>
#include "ofMain.h"



class target {
public:
    
    
    target(ofVec3f _location);
    
    ofVec3f location;
    bool choosen;
    
    
    
};
#endif /* defined(__PARTICLE_SYSTEM_TEST__target__) */

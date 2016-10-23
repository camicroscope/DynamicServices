//
//  AbstractCaMicroscope.h
//
//  Created by Amit on 1/22/16.
//  Copyright © 2016 Amit. All rights reserved.
//

#ifndef AbstractDynamicService
#define AbstractDynamicService

class AbstractCaMicroscope {
public:
    virtual void init() = 0;
    virtual void post() = 0;
};

#endif /* AbstractCaMicroscope_h */

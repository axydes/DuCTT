/*
 * Copyright © 2012, United States Government, as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All rights reserved.
 * 
 * The NASA Tensegrity Robotics Toolkit (NTRT) v1 platform is licensed
 * under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language
 * governing permissions and limitations under the License.
*/

#ifndef TG_PRISMATIC_INFO_H
#define TG_PRISMATIC_INFO_H

/**
 * @file tgPrismaticInfo.h
 * @brief Contains the definition of class tgPrismaticInfo. A prismatic actuator builder.
 * @author Alexander Xydes
 * @copyright Copyright (C) 2014 NASA Ames Research Center
 * $Id$
 */

#include "btBulletDynamicsCommon.h"
#include <iostream>

#include "tgPrismatic.h"

#include "core/tgBaseString.h"
#include "core/tgString.h"

#include "tgcreator/tgBuildSpec.h"
#include "tgcreator/tgConnectorInfo.h"
#include "tgcreator/tgNode.h"
#include "tgcreator/tgPair.h"
#include "tgcreator/tgRodInfo.h"
#include "tgcreator/tgStructure.h"
#include "tgcreator/tgStructureInfo.h"
#include "tgcreator/tgUtil.h"

class tgWorld;
class tgNode;

class tgPrismaticInfo: public tgConnectorInfo
{
public: 
       
    tgPrismaticInfo(const tgPrismatic::Config& config);
    
    tgPrismaticInfo(const tgPrismatic::Config& config, tgTags tags);

    tgPrismaticInfo(const tgPrismatic::Config& config, const tgPair& pair);
    
    virtual ~tgPrismaticInfo() {}
    
    /**
     * Create a tgConnectorInfo* from a tgPair
     */ 
    virtual tgConnectorInfo* createConnectorInfo(const tgPair& pair);
    
    void initConnector(tgWorld& world);
    
    virtual tgModel* createModel(tgWorld& world);
    
    const int getSegments() const;
    
    double getMass();

protected:

    void buildModel(tgWorld& world, tgModel* rod1, tgModel* rod2);

    tgPrismatic::Config m_config;
};

#endif

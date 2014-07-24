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

/**
 * @file DuCTTTestModel.cpp
 * @brief Contains the implementation of class DuCTTTestModel
 * @author Alexander Xydes
 * $Id$
 */
#include "DuCTTTestModel.h"

DuCTTTestModel::DuCTTTestModel(size_t segments) :
    m_segments(segments),
    tgModel() 
{
}

void DuCTTTestModel::addNodes(tgStructure& tetra, double edge, double height)
{
    // right
    tetra.addNode(-edge / 2.0, 0, tgUtil::round(std::sqrt(3.0) / 2.0 * height));
    // left
    tetra.addNode( edge / 2.0, 0, tgUtil::round(std::sqrt(3.0) / 2.0 * height));
    // front
    tetra.addNode(0, edge/2.0, 0);
    // back
    tetra.addNode(0, -edge/2.0, 0);
}

//add param for top or bottom prismatic actuator
void DuCTTTestModel::addPairs(tgStructure& tetra)
{
    tetra.addPair(3, 0, "back right rod");
    tetra.addPair(3, 1, "back left rod");
    tetra.addPair(2, 0, "front right rod");
    tetra.addPair(2, 1, "front left rod");
    tetra.addPair(0, 1, "top rod");
    tetra.addPair(2, 3, "bottom rod");
//    tetra.addPair(0, 1, "top prismatic");
//    tetra.addPair(2, 3, "bottom prismatic");
}

// Add muscles that connect the segments
void DuCTTTestModel::addMuscles(tgStructure& snake)
{
    const std::vector<tgStructure*> children = snake.getChildren();
    for (size_t i = 1; i < children.size(); ++i)
    {
        tgNodes n0 = children[i-1]->getNodes();
        tgNodes n1 = children[i  ]->getNodes();

        snake.addPair(n0[0], n1[0], "top right muscle");
        snake.addPair(n0[1], n1[1], "top left muscle");

        snake.addPair(n0[2], n1[0], "front right muscle");
        snake.addPair(n0[2], n1[1], "front left muscle");

        snake.addPair(n0[3], n1[0], "back right muscle");
        snake.addPair(n0[3], n1[1], "back left muscle");

        snake.addPair(n0[2], n1[2], "bottom front muscle");
        snake.addPair(n0[3], n1[3], "bottom back muscle");
    }
}

void DuCTTTestModel::mapMuscles(DuCTTTestModel::MuscleMap& muscleMap,
        tgModel& model)
{
    // Note that tags don't need to match exactly, we could create
    // supersets if we wanted to
    muscleMap["top right"] = model.find<tgLinearString>("top right muscle");
    muscleMap["top left"]  = model.find<tgLinearString>("top left muscle");

    muscleMap["front right"] = model.find<tgLinearString>("front right muscle");
    muscleMap["front left"]  = model.find<tgLinearString>("front left muscle");

    muscleMap["back right"] = model.find<tgLinearString>("back right muscle");
    muscleMap["back left"]  = model.find<tgLinearString>("back left muscle");

    muscleMap["bottom front"] = model.find<tgLinearString>("bottom front muscle");
    muscleMap["bottom back"]  = model.find<tgLinearString>("bottom back muscle");
}

void DuCTTTestModel::trace(const tgStructureInfo& structureInfo, tgModel& model)
{
    std::cout << "StructureInfo:" << std::endl
      << structureInfo    << std::endl
      << "Model: "        << std::endl
      << model            << std::endl;
}

void DuCTTTestModel::setup(tgWorld& world)
{
    const double edge = 30.0;
    const double height = tgUtil::round(std::sqrt(3.0)/2 * edge);
    std::cout << "edge: " << edge << "; height: " << height << std::endl;

    // Create the first tetrahedra
    tgStructure tetra;
    addNodes(tetra, edge, height);
    addPairs(tetra);

    // Move the first one so we can create the second
    tetra.move(btVector3(0.0, 2.0, 10.0));

    // Create our snake segments
    tgStructure snake;
    addSegments(snake, tetra, edge, m_segments);
    addMuscles(snake);

    // Create the build spec that uses tags to turn the structure into a real model
    // Note: This needs to be high enough or things fly apart...
    const double density = 4.2 / 300.0; // kg / length^3 - see app for length
    const double radius  = 0.5;
    const tgRod::Config rodConfig(radius, density);
    tgBuildSpec spec;
    spec.addBuilder("rod", new tgRodInfo(rodConfig));
    spec.addBuilder("prismatic", new tgPrismaticInfo(rodConfig, rodConfig));
    
    tgLinearString::Config muscleConfig(1000, 10);
    spec.addBuilder("muscle", new tgLinearStringInfo(muscleConfig));
    
    // Create your structureInfo
    tgStructureInfo structureInfo(snake, spec);
    // Use the structureInfo to build ourselves
    structureInfo.buildInto(*this, world);

    // We could now use tgCast::filter or similar to pull out the models (e.g. muscles)
    // that we want to control.    
    allMuscles = tgCast::filter<tgModel, tgLinearString> (getDescendants());
    mapMuscles(muscleMap, *this);

    trace(structureInfo, *this);

    // Actually setup the children
    tgModel::setup(world);
}

void DuCTTTestModel::step(double dt)
{
    if (dt < 0.0)
    {
        throw std::invalid_argument("dt is not positive");
    }
    else
    {
        // Notify observers (controllers) of the step so that they can take action
        notifyStep(dt);
        // Step any children
        tgModel::step(dt);
    }
}
    
const std::vector<tgLinearString*>&
DuCTTTestModel::getMuscles (const std::string& key) const
{
    const MuscleMap::const_iterator it = muscleMap.find(key);
    if (it == muscleMap.end())
    {
        throw std::invalid_argument("Key '" + key + "' not found in muscle map");
    }
    else
    {
        return it->second;
    }
}


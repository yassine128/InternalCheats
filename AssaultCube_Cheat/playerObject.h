#pragma once
#include "common.h"

struct playerObject {
    int* health;
    float* southPos;
    float* westPos;
    char* name;
    float* viewAngle; 

    std::string getPlayerDetails() {
        std::stringstream ss;
        ss << "Name: " << name << "\n";
        ss << "Health: " << std::to_string(*health) << "\n";
        ss << "South-North Position: " << std::to_string(*southPos) << "\n";
        ss << "West-East Position: " << std::to_string(*westPos) << "\n";
        return ss.str();
    }
};
#pragma once

#include "../config.h"

#include <memory>
#include <functional>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

class OrbitalFactory {
public:
    enum class PositionCallback {
       None,
       Sun,
       Mercury,
       Venus,
       Earth,
       Earth_Moon,
       Mars,
       Mars_Phobos,
       Mars_Demos,
       Saturn,
       Jupiter,
       Uranus,
       Neptune,
       Pluto
    };
    
    using PositionCallbackFun = std::function<void(double, glm::dvec3&)>;
    
    MECHANICS_EXPORT static const OrbitalFactory& instance();
    
    PositionCallbackFun orbitalSampler( PositionCallback ) const;
    
private:
    static std::unique_ptr<OrbitalFactory> _instance;
    OrbitalFactory() {}
};

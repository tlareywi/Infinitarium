#include "OrbitalFactory.hpp"

#include <libnova/mercury.h>
#include <libnova/venus.h>
#include <libnova/earth.h>
#include <libnova/lunar.h>
#include <libnova/mars.h>
#include <libnova/jupiter.h>
#include <libnova/saturn.h>
#include <libnova/uranus.h>
#include <libnova/neptune.h>
#include <libnova/pluto.h>

std::unique_ptr<OrbitalFactory> OrbitalFactory::_instance{nullptr};

const OrbitalFactory& OrbitalFactory::instance() {
    if (!_instance) {
        _instance = std::unique_ptr<OrbitalFactory>(new OrbitalFactory());
    }

    return *_instance;
}

OrbitalFactory::PositionCallbackFun OrbitalFactory::orbitalSampler( OrbitalFactory::PositionCallback id ) const {
    switch( id ) {
        case OrbitalFactory::PositionCallback::Mercury:
            return [](double jday, glm::dvec3& pos){
                ln_rect_posn p;
                ln_get_mercury_rect_helio(jday, &p);
                pos.x = p.X; pos.y = p.Y; pos.z = p.Z;
            };
            break;
        case OrbitalFactory::PositionCallback::Venus:
            return [](double jday, glm::dvec3& pos){
                ln_rect_posn p;
                ln_get_venus_rect_helio(jday, &p);
                pos.x = p.X; pos.y = p.Y; pos.z = p.Z;
            };
            break;
        case OrbitalFactory::PositionCallback::Earth:
            return [](double jday, glm::dvec3& pos){
                ln_rect_posn p;
                ln_get_earth_rect_helio(jday, &p);
                pos.x = p.X; pos.y = p.Y; pos.z = p.Z;
            };
            break;
        case OrbitalFactory::PositionCallback::Earth_Moon:
            return [](double jday, glm::dvec3& pos){
                ln_rect_posn p;
                ln_get_lunar_geo_posn(jday, &p, 1.0);
                pos.x = p.X; pos.y = p.Y; pos.z = p.Z;
            };
            break;
        case OrbitalFactory::PositionCallback::Mars:
            return [](double jday, glm::dvec3& pos){
                ln_rect_posn p;
                ln_get_mars_rect_helio(jday, &p);
                pos.x = p.X; pos.y = p.Y; pos.z = p.Z;
            };
            break;
        case OrbitalFactory::PositionCallback::Saturn:
            return [](double jday, glm::dvec3& pos){
                ln_rect_posn p;
                ln_get_saturn_rect_helio(jday, &p);
                pos.x = p.X; pos.y = p.Y; pos.z = p.Z;
            };
            break;
        case OrbitalFactory::PositionCallback::Jupiter:
            return [](double jday, glm::dvec3& pos){
                ln_rect_posn p;
                ln_get_jupiter_rect_helio(jday, &p);
                pos.x = p.X; pos.y = p.Y; pos.z = p.Z;
            };
            break;
        case OrbitalFactory::PositionCallback::Uranus:
            return [](double jday, glm::dvec3& pos){
                ln_rect_posn p;
                ln_get_uranus_rect_helio(jday, &p);
                pos.x = p.X; pos.y = p.Y; pos.z = p.Z;
            };
            break;
        case OrbitalFactory::PositionCallback::Neptune:
            return [](double jday, glm::dvec3& pos){
                ln_rect_posn p;
                ln_get_neptune_rect_helio(jday, &p);
                pos.x = p.X; pos.y = p.Y; pos.z = p.Z;
            };
            break;
        case OrbitalFactory::PositionCallback::Pluto:
            return [](double jday, glm::dvec3& pos){
                ln_rect_posn p;
                ln_get_pluto_rect_helio(jday, &p);
                pos.x = p.X; pos.y = p.Y; pos.z = p.Z;
            };
            break;
        default:
            return [](double jday, glm::dvec3& pos){ pos = glm::dvec3(0.0,0.0,0.0); };
    }
}


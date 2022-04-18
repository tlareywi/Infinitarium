//
//  Copyright © 2022 Blue Canvas Studios LLC. All rights reserved. Commercial use prohibited by license.
//

#pragma once

#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class Stats {
public:
	static Stats& Instance();
	unsigned int fps;

	glm::dvec3 eye;
	glm::dvec3 center;
	glm::dvec3 up;
	double distance;

private:
	Stats();
};
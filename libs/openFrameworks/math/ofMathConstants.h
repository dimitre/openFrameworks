#pragma once

// ofIndexType from here
#include "ofConstants.h"


#include <glm/detail/qualifier.hpp>
namespace glm {
	typedef vec<2, float, defaultp>		vec2;
	typedef vec<3, float, defaultp>		vec3;
	typedef vec<4, float, defaultp>		vec4;
}
using ofDefaultVec2 = glm::vec2;
using ofDefaultVec3 = glm::vec3;
using ofDefaultVec4 = glm::vec4;


#ifndef ANIMATION_PATTERN_H
#define ANIMATION_PATTERN_H

#include <map>
#include <string>
#include "Vector3.h"

struct SAnimationPattern {
	/*First Type is for indicating what type of animation, for example, "WALK_FORWARD" or "WALK_BACKWARD".
	Second Type is for translating the direction.*/
	std::map<std::string, Vector3>translateAnimation;
	/*First Type is for indicating what type of animation, for example, "SHAKE" or "SPIN".
	Second Type is for the rotation angle followed by rotation axis.*/
	std::map<std::string, std::pair<float, Vector3>>rotateAnimation;
};

#endif
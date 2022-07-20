#include "core/math/math.h"

namespace Eagle
{
	glm::mat4 Math::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		return glm::ortho(left, right, bottom, top, zNear, zFar);
	}
}
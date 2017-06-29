#include "Math/EasingFunctions.h"

namespace MathModule {

float EaseInQuadratic( float t, float b, float c, float d )
{
	t /= d;
	return c * t * t + b;
}

float EaseOutQuadratic( float t, float b, float c, float d )
{
	t /= d;
	return -c * t *( t - 2 ) + b;
}


float EaseInOutQuadratic( float t, float b, float c, float d )
{
	t /= d/2;
	if ( t < 1 ) {
		return c/2*t*t + b;
	}
	t--;
	return -c/2 * ( t*( t-2 ) - 1 ) + b;
}

} // namespace MathModule

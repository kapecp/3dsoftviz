#pragma once

namespace MathModule {

// code from http://gizma.com/easing/

/**
 * @brief EaseInQuadratic Quadratic interpolation of value b to b+c over duration.
 * (Hint : slow start)
 * @param t current time ( interpolation ) value
 * @param b initial ( before ) value
 * @param c value change to be made
 * @param d duration of  interpolation
 * @return interpolated value from b to b+c
 */
float EaseInQuadratic( float t, float b, float c, float d );

/**
 * @brief EaseOutQuadratic Quadratic interpolation of value b to b+c over duration.
 * (Hint : slow end)
 * @param t current time ( interpolation ) value
 * @param b initial ( before ) value
 * @param c value change to be made
 * @param d duration of  interpolation
 * @return interpolated value from b to b+c
 */
float EaseOutQuadratic( float t, float b, float c, float d );

/**
 * @brief EaseInOutQuadratic Quadratic interpolation of value b to b+c over duration.
 * (Hint : slow start and end)
 * @param t current time ( interpolation ) value
 * @param b initial ( before ) value
 * @param c value change to be made
 * @param d duration of  interpolation
 * @return interpolated value from b to b+c
 */
float EaseInOutQuadratic( float t, float b, float c, float d );
}

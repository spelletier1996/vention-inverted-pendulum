#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <cmath>

namespace tools {
inline double DegToRad(double degrees) { return (degrees * (M_PI / 180.0F)); }
inline double RadToDeg(double radians) { return (radians * (180.0F / M_PI)); }
} // namespace tools

#endif // TOOLS_HPP

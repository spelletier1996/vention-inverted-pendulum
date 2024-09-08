#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <cmath>

namespace tools {
inline auto DegToRad(double degrees) -> double { return (degrees * (M_PI / 180.0F)); }
inline auto RadToDeg(double radians) -> double { return (radians * (180.0F / M_PI)); }
} // namespace tools

#endif // TOOLS_HPP

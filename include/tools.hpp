#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <cmath>
#include <cstdint>
#include <cstring>
#include <span>
#include <vector>

namespace tools {
    
inline double DegToRad(double degrees) { return (degrees * (M_PI / 180.0F)); }
inline double RadToDeg(double radians) { return (radians * (180.0F / M_PI)); }

//!@brief Deserialize a byte span into a vector of types
//!
//!@tparam T The output type
//!@param data The bytes to deserialize
//!@param data_count The number of output types to deserialize
//!@return std::vector<T> The deserialized data
template <typename T>
auto Deserialize(std::span<const uint8_t> data, int data_count)
    -> std::vector<T> {
  std::vector<T> ret_val;
  const int type_size = sizeof(T);

  int offset = 0;
  for (int i = 0; i < data_count; i++) {
    offset += i * type_size;

    T val;
    std::memcpy(&val, data.data() + offset, type_size); // NOLINT
    ret_val.push_back(val);
  }
  return ret_val;
}

//!@brief Serialize a vector of types into a byte vector
//!
//!@tparam T Input type
//!@param data List of things to serialize
//!@return std::vector<uint8_t> The Serialized bytes
template <typename T>
auto Serialize(std::vector<T> data) -> std::vector<uint8_t> {
  const int type_size = sizeof(T);
  const int data_array_size = type_size * data.size();
  std::vector<uint8_t> buffer(data_array_size);

  int offset = 0;
  for (size_t i = 0; i < data.size(); i++) {
    std::memcpy(buffer.data() + offset, &data[i], type_size); // NOLINT
    offset += type_size;
  }
  return buffer;
}
} // namespace tools

#endif // TOOLS_HPP

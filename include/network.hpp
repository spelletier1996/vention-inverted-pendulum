#ifndef INCLUDE_NETWORK_HPP
#define INCLUDE_NETWORK_HPP

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

namespace utils {

//! Data structures for convinient interprocess communication
struct SimState {
  double position;
  double angle;
  double velocity;
  double angular_velocity;
};

struct SimCommand {
  double velocity;
  double disturbance;
  bool reset;
};

struct ControllerSettings {
  double position_Kp;
  double position_Ki;
  double position_Kd;
  double angle_Kp;
  double angle_Ki;
  double angle_Kd;
};

//!
//!@brief Simple mock network class using shared memory
//! Thread safe
//!@tparam T Data type to be shared
//!
template <typename T> class SharedMemory {
public:
  SharedMemory(std::string name) : name_(name){};

  //!
  //!@brief Init the memory and map objects
  //! Cannot be in constructor as they must be called in child class constructor
  //!@param shared_memory_object
  //!@param mutex
  //!
  void Init(std::unique_ptr<boost::interprocess::shared_memory_object>
                shared_memory_object,
            std::unique_ptr<boost::interprocess::named_mutex> mutex) {
    this->shared_memory_object_ = std::move(shared_memory_object);
    this->mutex_ = std::move(mutex);
  }

  //!
  //!@brief Read the shared memory
  //! thread safe
  //!@return T
  //!
  auto Read() -> T {
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(
        *this->mutex_.get());
    return *static_cast<T *>(this->mapped_region_->get_address());
  }

  //!
  //!@brief Write to the shared memory
  //! thread safe
  //!@param data
  //!
  void Write(const T &data) {
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(
        *this->mutex_.get());
    *static_cast<T *>(this->mapped_region_->get_address()) = data;
  }

protected:
  std::unique_ptr<boost::interprocess::shared_memory_object>
      shared_memory_object_;
  std::unique_ptr<boost::interprocess::mapped_region> mapped_region_;
  std::unique_ptr<boost::interprocess::named_mutex> mutex_;
  std::string name_;
};

template <typename T> class Server : public SharedMemory<T> {
public:
  Server(std::string name) : SharedMemory<T>(name) {
    // Remove existing objects as per boost docs
    boost::interprocess::shared_memory_object::remove(name.c_str());
    boost::interprocess::named_mutex::remove(name.c_str());
    // Must be called after child class constructor
    this->Init(
        std::move(std::make_unique<boost::interprocess::shared_memory_object>(
            boost::interprocess::create_only, name.c_str(),
            boost::interprocess::read_write)),
        std::move(std::make_unique<boost::interprocess::named_mutex>(
            boost::interprocess::create_only, name.c_str())));

    // Shared memory is always zero, must truncate to size of T
    this->shared_memory_object_->truncate(sizeof(T));

    // Ensure shared memory was created
    if (!this->shared_memory_object_->get_name()) {
      throw std::runtime_error("Failed to create shared memory object");
    }

    this->mapped_region_ = std::make_unique<boost::interprocess::mapped_region>(
        *this->shared_memory_object_.get(), boost::interprocess::read_write);
  }
  ~Server() {
    boost::interprocess::shared_memory_object::remove(this->name_.c_str());
    boost::interprocess::named_mutex::remove(this->name_.c_str());
  }
};

template <typename T> class Client : public SharedMemory<T> {
public:
  Client(std::string name) : SharedMemory<T>(name) {
    // Attempt to open the shared memory object
    int max_retries = 5;
    int retry_delay = 500;
    for (int i = 0; i < max_retries; ++i) {
      try {
        this->Init(
            std::move(
                std::make_unique<boost::interprocess::shared_memory_object>(
                    boost::interprocess::open_only, name.c_str(),
                    boost::interprocess::read_write)),
            std::move(std::make_unique<boost::interprocess::named_mutex>(
                boost::interprocess::open_only, name.c_str())));
      } catch (const boost::interprocess::interprocess_exception &e) {
        printf("Failed to open shared memory object: %s\n", e.what());
        std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay));
      }
    }

    // Map the shared memory object
    this->mapped_region_ = std::make_unique<boost::interprocess::mapped_region>(
        *this->shared_memory_object_.get(), boost::interprocess::read_write);
  }
};

} // namespace utils

#endif // INCLUDE_NETWORK_HPP

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace ndyn::sensor {

template <typename AddressT, typename DeviceAddressT>
class BusTransmission<BusType::FAKE_BUS, AddressT, DeviceAddressT> final {
 private:
  std::unordered_map<AddressT, uint8_t> values_{};

 protected:
  Bus<BusType::FAKE_BUS, AddressT, DeviceAddressT>* bus_;

 public:
  BusTransmission(Bus<BusType::FAKE_BUS, AddressT, DeviceAddressT>& bus) : bus_(&bus) {
    bus_->lock();
  }

  BusTransmission(BusTransmission&& rhs) : bus_(rhs.bus_) { rhs.bus_ = nullptr; }
  BusTransmission& operator=(BusTransmission&& rhs) {
    bus_ = rhs.bus_;
    rhs.bus_ = nullptr;
    return *this;
  }

  BusTransmission() = delete;
  BusTransmission(const BusTransmission&) = delete;
  BusTransmission& operator=(const BusTransmission&) = delete;

  ~BusTransmission() {
    if (bus_) {
      bus_->unlock();
    }
  }

  bool write(AddressT location, uint8_t value) {
    values_[location] = value;
    return true;
  }

  template <size_t ARRAY_SIZE>
  bool write(AddressT location, size_t num_bytes, const std::array<uint8_t, ARRAY_SIZE>& values) {
    for (size_t i = 0; i < num_bytes; ++i) {
      values_[location + i] = values.at(i);
    }
    return false;
  }

  bool read(AddressT location, uint8_t& buffer) {
    if (values_.count(location) > 0) {
      buffer = values_[location];
    } else {
      buffer = 0;
    }
    return true;
  }

  template <size_t ARRAY_SIZE>
  size_t read(AddressT location, std::array<uint8_t, ARRAY_SIZE>& buffer) {
    for (size_t i = 0; i < buffer.size(); ++i) {
      if (values_.count(location + i) > 0) {
        buffer[i] = values_[location + i];
      } else {
        buffer[i] = 0;
      }
    }
    return buffer.size();
  }
};

}  // namespace ndyn::sensor

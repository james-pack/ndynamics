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

  bool write(AddressT location, uint16_t value) {
    for (size_t i = 0; i < sizeof(value); ++i) {
      values_[location + i] = value & 0xff;
      value = (value >> 8);
    }
    return true;
  }

  bool write(AddressT location, uint32_t value) {
    for (size_t i = 0; i < sizeof(value); ++i) {
      values_[location + i] = value & 0xff;
      value = (value >> 8);
    }
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

  bool read(AddressT location, uint16_t& buffer) {
    for (size_t i = 0; i < sizeof(buffer); ++i) {
      if (values_.count(location + i) > 0) {
        buffer = (buffer << 8) | values_[location + i];
      } else {
        buffer = (buffer << 8);
      }
    }
    return true;
  }

  bool read(AddressT location, uint32_t& buffer) {
    for (size_t i = 0; i < sizeof(buffer); ++i) {
      if (values_.count(location + i) > 0) {
        buffer = (buffer << 8) | values_[location + i];
      } else {
        buffer = (buffer << 8);
      }
    }
    return true;
  }

  template <typename T>
  bool read(AddressT location, T& buffer) {
    for (size_t i = 0; i < sizeof(buffer); ++i) {
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

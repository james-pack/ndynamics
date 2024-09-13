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
    bus_->lock(this);
  }

  BusTransmission(BusTransmission&& rhs) : bus_(rhs.bus_) {
    if (bus_->owns_lock(&rhs)) {
      bus_->unlock();
      bus_->lock(this);
    }
    rhs.bus_ = nullptr;
  }

  BusTransmission& operator=(BusTransmission&& rhs) {
    bus_ = rhs.bus_;
    if (bus_->owns_lock(&rhs)) {
      bus_->unlock();
      bus_->lock(this);
    }
    rhs.bus_ = nullptr;
    return *this;
  }

  BusTransmission() = delete;
  BusTransmission(const BusTransmission&) = delete;
  BusTransmission& operator=(const BusTransmission&) = delete;

  ~BusTransmission() {
    if (bus_ && bus_->owns_lock(this)) {
      bus_->unlock();
    }
  }

  operator bool() const { return bus_ != nullptr && bus_->owns_lock(this); }

  bool write(AddressT location, uint8_t value) {
    if (*this) {
      values_[location] = value;
      return true;
    } else {
      return false;
    }
  }

  bool write(AddressT location, uint16_t value) {
    if (*this) {
      for (size_t i = 0; i < sizeof(value); ++i) {
        values_[location + i] = value & 0xff;
        value = (value >> 8);
      }
      return true;
    } else {
      return false;
    }
  }

  bool write(AddressT location, uint32_t value) {
    if (*this) {
      for (size_t i = 0; i < sizeof(value); ++i) {
        values_[location + i] = value & 0xff;
        value = (value >> 8);
      }
      return true;
    } else {
      return false;
    }
  }

  template <size_t ARRAY_SIZE>
  bool write(AddressT location, size_t num_bytes, const std::array<uint8_t, ARRAY_SIZE>& values) {
    if (*this) {
      for (size_t i = 0; i < num_bytes; ++i) {
        values_[location + i] = values.at(i);
      }
      return true;
    } else {
      return false;
    }
  }

  bool read(AddressT location, uint8_t& buffer) {
    if (*this) {
      if (values_.count(location) > 0) {
        buffer = values_[location];
      } else {
        buffer = 0;
      }
      return true;
    } else {
      return false;
    }
  }

  bool read(AddressT location, uint16_t& buffer) {
    if (*this) {
      for (size_t i = 0; i < sizeof(buffer); ++i) {
        if (values_.count(location + i) > 0) {
          buffer = (buffer << 8) | values_[location + i];
        } else {
          buffer = (buffer << 8);
        }
      }
      return true;
    } else {
      return false;
    }
  }

  bool read(AddressT location, uint32_t& buffer) {
    if (*this) {
      for (size_t i = 0; i < sizeof(buffer); ++i) {
        if (values_.count(location + i) > 0) {
          buffer = (buffer << 8) | values_[location + i];
        } else {
          buffer = (buffer << 8);
        }
      }
      return true;
    } else {
      return false;
    }
  }

  template <size_t ARRAY_SIZE>
  size_t read(AddressT location, size_t bytes_to_read, std::array<uint8_t, ARRAY_SIZE>& buffer) {
    if (*this) {
      for (size_t i = 0; i < bytes_to_read; ++i) {
        if (values_.count(location + i) > 0) {
          buffer[i] = values_[location + i];
        } else {
          buffer[i] = 0;
        }
      }
      return buffer.size();
    } else {
      return false;
    }
  }
};

}  // namespace ndyn::sensor

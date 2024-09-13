/**
 * Generic classes and other structures for reading and writing on buses local to a single
 * device. These classes lock in the semantics of communicating over a bus. They are meant to cover
 * I2C, SPI, and CAN bus, but they should also be extensible to other buses with similar timings,
 * requirements, and features.
 */
#pragma once

#include <array>
#include <cstdint>

namespace ndyn::sensor {

enum class BusType : uint8_t {
  FAKE_BUS,
  I2C,
  SPI,
  CAN,
};

template <BusType BUS_TYPE, typename AddressT, typename DeviceAddressT = uint8_t>
class Bus;

template <BusType BUS_TYPE, typename AddressT, typename DeviceAddressT>
class BusTransmission final {
 protected:
  Bus<BUS_TYPE, AddressT, DeviceAddressT>* bus_;

 public:
  BusTransmission(Bus<BUS_TYPE, AddressT, DeviceAddressT>& bus) : bus_(&bus) { bus_->lock(); }

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

  bool write(AddressT location, uint8_t value) { return false; }

  template <size_t ARRAY_SIZE>
  bool write(AddressT location, size_t num_bytes, const std::array<uint8_t, ARRAY_SIZE>& value) {
    return false;
  }

  bool read(AddressT location, uint8_t& buffer) { return false; };

  template <size_t ARRAY_SIZE>
  size_t read(AddressT location, std::array<uint8_t, ARRAY_SIZE>& buffer) {
    return 0;
  }
};

template <BusType BUS_TYPE, typename AddressT, typename DeviceAddressT>
class Bus final {
 private:
  void lock();
  void unlock();

  friend class BusTransmission<BUS_TYPE, AddressT, DeviceAddressT>;

 public:
  constexpr Bus() = default;
  constexpr Bus(Bus&&) = default;
  constexpr Bus& operator=(Bus&&) = default;

  Bus(const Bus&) = delete;
  constexpr Bus& operator=(const Bus&) = delete;

  BusTransmission<BUS_TYPE, AddressT, DeviceAddressT> initiate(DeviceAddressT addr) {
    return BusTransmission<BUS_TYPE, AddressT, DeviceAddressT>(*this);
  }
};

}  // namespace ndyn::sensor
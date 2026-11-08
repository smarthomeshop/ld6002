#pragma once

#include <array>
#include <cstdint>

#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

namespace esphome::ld6002 {

class LD6002Component : public Component, public uart::UARTDevice {
 public:
  explicit LD6002Component(bool fall_detection) : fall_detection_(fall_detection) {}
  void loop() override;

#ifdef USE_SENSOR
  void set_breathing_rate_sensor(sensor::Sensor *sensor) { this->breathing_rate_ = sensor; }
  void set_heart_rate_sensor(sensor::Sensor *sensor) { this->heart_rate_ = sensor; }
  void set_target_depth_sensor(sensor::Sensor *sensor) { this->target_depth_ = sensor; }
#endif
#ifdef USE_BINARY_SENSOR
  void set_presence_binary_sensor(binary_sensor::BinarySensor *sensor) { this->presence_ = sensor; }
  void set_fall_detected_binary_sensor(binary_sensor::BinarySensor *sensor) { this->fall_detected_ = sensor; }
#endif

 protected:
  void feed_byte_(uint8_t byte, uint32_t now);
  void handle_report_(uint32_t now);

  const bool fall_detection_;
  std::array<uint8_t, 8> header_{};
  std::array<uint8_t, 4> payload_{};
  uint8_t header_used_{0};
  uint16_t payload_size_{0};
  uint16_t payload_used_{0};
  uint8_t payload_xor_{0};
  uint32_t last_byte_at_{0};

#ifdef USE_SENSOR
  sensor::Sensor *breathing_rate_{nullptr};
  sensor::Sensor *heart_rate_{nullptr};
  sensor::Sensor *target_depth_{nullptr};
  uint32_t last_breathing_rate_at_{0};
  uint32_t last_heart_rate_at_{0};
  uint32_t last_target_depth_at_{0};
#endif
#ifdef USE_BINARY_SENSOR
  binary_sensor::BinarySensor *presence_{nullptr};
  binary_sensor::BinarySensor *fall_detected_{nullptr};
  uint32_t last_presence_at_{0};
  uint32_t last_fall_at_{0};
#endif
};

}  // namespace esphome::ld6002

#include "ld6002.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "esphome/core/hal.h"

namespace esphome::ld6002 {

void LD6002Component::loop() {
  const uint32_t now = millis();
  if (now - this->last_byte_at_ > 100)
    this->header_used_ = 0;

  uint8_t byte;
  while (this->available() && this->read_byte(&byte)) {
    this->last_byte_at_ = now;
    this->feed_byte_(byte, now);
  }

#ifdef USE_SENSOR
  if (this->breathing_rate_ && std::isfinite(this->breathing_rate_->raw_state) &&
      now - this->last_breathing_rate_at_ > 5000)
    this->breathing_rate_->publish_state(NAN);
  if (this->heart_rate_ && std::isfinite(this->heart_rate_->raw_state) && now - this->last_heart_rate_at_ > 5000)
    this->heart_rate_->publish_state(NAN);
  if (this->target_depth_ && std::isfinite(this->target_depth_->raw_state) && now - this->last_target_depth_at_ > 3000)
    this->target_depth_->publish_state(NAN);
#endif
#ifdef USE_BINARY_SENSOR
  if (this->presence_ && this->presence_->has_state() && now - this->last_presence_at_ > 2000)
    this->presence_->invalidate_state();
  if (this->fall_detected_ && this->fall_detected_->has_state() && now - this->last_fall_at_ > 2000)
    this->fall_detected_->invalidate_state();
#endif
}

void LD6002Component::feed_byte_(uint8_t byte, uint32_t now) {
  if (this->header_used_ < this->header_.size()) {
    if (this->header_used_ == 0 && byte != 0x01)
      return;
    this->header_[this->header_used_++] = byte;
    if (this->header_used_ < this->header_.size())
      return;

    uint8_t header_xor = 0;
    for (uint8_t field : this->header_)
      header_xor ^= field;

    this->payload_size_ = (static_cast<uint16_t>(this->header_[3]) << 8) | this->header_[4];
    if (header_xor != 0xFF || this->payload_size_ > 4096) {
      // A stray start byte can precede the header of a real report.
      const auto next_start = std::find(this->header_.begin() + 1, this->header_.end(), 0x01);
      this->header_used_ = this->header_.end() - next_start;
      if (this->header_used_ != 0)
        std::memmove(this->header_.data(), &*next_start, this->header_used_);
      return;
    }
    if (this->payload_size_ == 0) {
      this->header_used_ = 0;
      return;
    }
    this->payload_used_ = 0;
    this->payload_xor_ = 0;
    return;
  }

  if (this->payload_used_ < this->payload_size_) {
    if (this->payload_used_ < this->payload_.size())
      this->payload_[this->payload_used_] = byte;
    this->payload_used_++;
    this->payload_xor_ ^= byte;
    return;
  }

  this->header_used_ = 0;
  if ((this->payload_xor_ ^ byte) == 0xFF)
    this->handle_report_(now);
}

void LD6002Component::handle_report_(uint32_t now) {
  const uint16_t type = (static_cast<uint16_t>(this->header_[5]) << 8) | this->header_[6];
#ifdef USE_BINARY_SENSOR
  if (this->fall_detection_ && this->payload_size_ == 1 && this->payload_[0] <= 1) {
    if (type == 0x0E02 && this->fall_detected_) {
      this->last_fall_at_ = now;
      this->fall_detected_->publish_state(this->payload_[0] == 1);
    } else if (type == 0x0F09 && this->presence_) {
      this->last_presence_at_ = now;
      this->presence_->publish_state(this->payload_[0] == 1);
    }
  }
#endif
#ifdef USE_SENSOR
  if (this->payload_size_ != 4)
    return;

  // TinyFrame's header uses big-endian integers. The ESP32 float payload is little-endian.
  float value;
  std::memcpy(&value, this->payload_.data(), sizeof(value));
  if (!std::isfinite(value) || value <= 0)
    return;

  if (this->fall_detection_) {
    if (type == 0x0E0E && this->target_depth_) {
      this->last_target_depth_at_ = now;
      this->target_depth_->publish_state(value);
    }
  } else if (type == 0x0A14 && this->breathing_rate_) {
    this->last_breathing_rate_at_ = now;
    this->breathing_rate_->publish_state(value);
  } else if (type == 0x0A15 && this->heart_rate_) {
    this->last_heart_rate_at_ = now;
    this->heart_rate_->publish_state(value);
  }
#endif
}

}  // namespace esphome::ld6002

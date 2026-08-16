#pragma once

#include <atomic>
#include <cstdint>

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/core/hal.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"


namespace esphome {
namespace actron_ultima {


// -----------------------------
// Protocol configuration
// -----------------------------

// Measured:
//
// Logic 1:
// falling-edge gap < 1000 us
//
// Logic 0:
// falling-edge gap > 1000 us
//
// Longest observed data gap:
// ~2500 us
//
// Shortest observed frame gap:
// ~60000 us
//
// 10000 us therefore gives a huge
// margin between data and frames.

static constexpr uint32_t BIT_THRESHOLD_US = 1000;

static constexpr uint32_t FRAME_GAP_US = 10000;

static constexpr uint8_t FRAME_BITS = 40;


class ActronUltima : public Component {
 public:

  void setup() override;

  void loop() override;

  void dump_config() override;

  float get_setup_priority() const override {
    return setup_priority::IO;
  }


  void set_pin(GPIOPin *pin) {
    this->pin_ = pin;
  }


  void set_bit_string_sensor(
      text_sensor::TextSensor *sensor) {

    this->bit_string_sensor_ = sensor;
  }

  void set_setpoint_sensor(sensor::Sensor *sensor) {
    this->setpoint_sensor_ = sensor;
  }
  
  void set_cool_sensor(binary_sensor::BinarySensor *s) {
    this->cool_sensor_ = s;
  }
  
  void set_auto_sensor(binary_sensor::BinarySensor *s) {
    this->auto_sensor_ = s;
  }
  
  void set_run_sensor(binary_sensor::BinarySensor *s) {
    this->run_sensor_ = s;
  }
  
  void set_timer_sensor(binary_sensor::BinarySensor *s) {
    this->timer_sensor_ = s;
  }
  
  void set_fan_cont_sensor(binary_sensor::BinarySensor *s) {
    this->fan_cont_sensor_ = s;
  }
  
  void set_fan_high_sensor(binary_sensor::BinarySensor *s) {
    this->fan_high_sensor_ = s;
  }
  
  void set_fan_mid_sensor(binary_sensor::BinarySensor *s) {
    this->fan_mid_sensor_ = s;
  }
  
  void set_fan_low_sensor(binary_sensor::BinarySensor *s) {
    this->fan_low_sensor_ = s;
  }
  
  void set_heat_sensor(binary_sensor::BinarySensor *s) {
    this->heat_sensor_ = s;
  }
  
  void set_inside_sensor(binary_sensor::BinarySensor *s) {
    this->inside_sensor_ = s;
  }
  
  void set_zone1_sensor(binary_sensor::BinarySensor *s) {
    this->zone1_sensor_ = s;
  }
  
  void set_zone2_sensor(binary_sensor::BinarySensor *s) {
    this->zone2_sensor_ = s;
  }
  
  void set_zone3_sensor(binary_sensor::BinarySensor *s) {
    this->zone3_sensor_ = s;
  }
  
  void set_zone4_sensor(binary_sensor::BinarySensor *s) {
    this->zone4_sensor_ = s;
  }
  
  void set_zone5_sensor(binary_sensor::BinarySensor *s) {
    this->zone5_sensor_ = s;
  }
  
  void set_zone6_sensor(binary_sensor::BinarySensor *s) {
    this->zone6_sensor_ = s;
  }
  
  void set_zone7_sensor(binary_sensor::BinarySensor *s) {
    this->zone7_sensor_ = s;
  }
  
  void set_zone8_sensor(binary_sensor::BinarySensor *s) {
    this->zone8_sensor_ = s;
  }


 protected:

  static void IRAM_ATTR gpio_interrupt(
      ActronUltima *arg);


  GPIOPin *pin_{nullptr};

  text_sensor::TextSensor *bit_string_sensor_{nullptr};


  // -----------------------------
  // ISR-only working state
  // -----------------------------

  volatile uint32_t last_edge_us_{0};

  volatile uint8_t bit_index_{0};

  uint64_t working_bits_{0};


  // -----------------------------
  // ISR -> main loop
  // -----------------------------

  std::atomic<uint64_t> completed_bits_{0};

  std::atomic<bool> frame_ready_{false};

  sensor::Sensor *setpoint_sensor_{nullptr};
  
  binary_sensor::BinarySensor *cool_sensor_{nullptr};
  binary_sensor::BinarySensor *auto_sensor_{nullptr};
  binary_sensor::BinarySensor *run_sensor_{nullptr};
  binary_sensor::BinarySensor *timer_sensor_{nullptr};
  
  binary_sensor::BinarySensor *fan_cont_sensor_{nullptr};
  binary_sensor::BinarySensor *fan_high_sensor_{nullptr};
  binary_sensor::BinarySensor *fan_mid_sensor_{nullptr};
  binary_sensor::BinarySensor *fan_low_sensor_{nullptr};
  
  binary_sensor::BinarySensor *heat_sensor_{nullptr};
  binary_sensor::BinarySensor *inside_sensor_{nullptr};
  
  binary_sensor::BinarySensor *zone1_sensor_{nullptr};
  binary_sensor::BinarySensor *zone2_sensor_{nullptr};
  binary_sensor::BinarySensor *zone3_sensor_{nullptr};
  binary_sensor::BinarySensor *zone4_sensor_{nullptr};
  binary_sensor::BinarySensor *zone5_sensor_{nullptr};
  binary_sensor::BinarySensor *zone6_sensor_{nullptr};
  binary_sensor::BinarySensor *zone7_sensor_{nullptr};
  binary_sensor::BinarySensor *zone8_sensor_{nullptr};
};

}  // namespace actron_ultima
}  // namespace esphome

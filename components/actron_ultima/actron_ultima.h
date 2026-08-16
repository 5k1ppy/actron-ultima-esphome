#pragma once

#include <atomic>
#include <cstdint>

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/core/hal.h"
#include "esphome/components/text_sensor/text_sensor.h"


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

};

}  // namespace actron_ultima
}  // namespace esphome

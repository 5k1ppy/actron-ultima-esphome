#include "actron_ultima.h"

#include "esphome/core/log.h"


namespace esphome {
namespace actron_ultima {


static const char *const TAG = "actron_ultima";


// ============================================================
// GPIO interrupt
//
// Called on every FALLING EDGE from the conditioned POWER line.
//
// Your measured frame:
//
//      falling edge 0
//           |
//           | gap = bit 0
//           |
//      falling edge 1
//           |
//           | gap = bit 1
//           |
//      falling edge 2
//
//      ...
//
//      falling edge 40
//
// 41 falling edges therefore produce 40 data intervals.
// ============================================================

void IRAM_ATTR ActronUltima::gpio_interrupt(
    ActronUltima *arg) {

  const uint32_t now_us = micros();

  const uint32_t delta_us =
      now_us - arg->last_edge_us_;

  arg->last_edge_us_ = now_us;


  // ----------------------------------------------------------
  // FRAME BOUNDARY
  //
  // A large interval means this falling edge is the
  // FIRST edge of a new frame.
  //
  // The large interval itself is NOT a data bit.
  // ----------------------------------------------------------

  if (delta_us > FRAME_GAP_US) {

    arg->bit_index_ = 0;

    arg->working_bits_ = 0;

    return;
  }


  // ----------------------------------------------------------
  // Ignore anything after 40 bits until another proper
  // frame boundary occurs.
  // ----------------------------------------------------------

  if (arg->bit_index_ >= FRAME_BITS) {
    return;
  }


  // ----------------------------------------------------------
  // DATA DECODING
  //
  // Short gap -> 1
  // Long gap  -> 0
  //
  // working_bits_ starts cleared, so a zero requires
  // no action.
  // ----------------------------------------------------------

  if (delta_us < BIT_THRESHOLD_US) {

    arg->working_bits_ |=
        (1ULL << arg->bit_index_);
  }


  arg->bit_index_++;


  // ----------------------------------------------------------
  // Complete frame received.
  // ----------------------------------------------------------

  if (arg->bit_index_ == FRAME_BITS) {

    arg->completed_bits_.store(
        arg->working_bits_,
        std::memory_order_release
    );

    arg->frame_ready_.store(
        true,
        std::memory_order_release
    );
  }
}


// ============================================================
// Setup
// ============================================================

void ActronUltima::setup() {

  ESP_LOGCONFIG(
      TAG,
      "Setting up Actron Ultima raw decoder..."
  );


  if (this->pin_ == nullptr) {

    ESP_LOGE(
        TAG,
        "No GPIO pin configured"
    );

    return;
  }


  this->pin_->setup();


  auto *internal_pin =
      static_cast<InternalGPIOPin *>(
          this->pin_
      );


  internal_pin->attach_interrupt(
      gpio_interrupt,
      this,
      gpio::INTERRUPT_FALLING_EDGE
  );


  ESP_LOGCONFIG(
      TAG,
      "Decoder ready"
  );
}


// ============================================================
// Main loop
//
// We deliberately do string formatting OUTSIDE the interrupt.
// ============================================================

void ActronUltima::loop() {

  if (!this->frame_ready_.exchange(
          false,
          std::memory_order_acq_rel)) {

    return;
  }


  const uint64_t bits =
      this->completed_bits_.load(
          std::memory_order_acquire
      );


  char frame[41];


  for (uint8_t i = 0; i < FRAME_BITS; i++) {

    frame[i] =
        ((bits >> i) & 1ULL)
            ? '1'
            : '0';
  }


  frame[40] = '\0';


  // Print EVERY complete frame to ESPHome logs.
  ESP_LOGD(
      TAG,
      "Frame: %s",
      frame
  );


  // Publish to Home Assistant / ESPHome API.
  if (this->bit_string_sensor_ != nullptr) {

    this->bit_string_sensor_->publish_state(
        frame
    );
  }
}


// ============================================================
// Configuration logging
// ============================================================

void ActronUltima::dump_config() {

  ESP_LOGCONFIG(
      TAG,
      "Actron Ultima Raw Decoder:"
  );

  LOG_PIN(
      "  POWER input pin: ",
      this->pin_
  );

  ESP_LOGCONFIG(
      TAG,
      "  Bit threshold: %u us",
      BIT_THRESHOLD_US
  );

  ESP_LOGCONFIG(
      TAG,
      "  Frame gap threshold: %u us",
      FRAME_GAP_US
  );

  ESP_LOGCONFIG(
      TAG,
      "  Frame length: %u bits",
      FRAME_BITS
  );
}


}  // namespace actron_ultima
}  // namespace esphome

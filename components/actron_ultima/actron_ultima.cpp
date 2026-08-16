#include "actron_ultima.h"

#include "esphome/core/log.h"


namespace esphome {
namespace actron_ultima {


static const char *const TAG = "actron_ultima";

static int decode_digit(
    bool a,
    bool b,
    bool c,
    bool d,
    bool e,
    bool f,
    bool g) {

  uint8_t mask =
      (a << 0) |
      (b << 1) |
      (c << 2) |
      (d << 3) |
      (e << 4) |
      (f << 5) |
      (g << 6);

  switch (mask) {
    case 0x3F: return 0;
    case 0x06: return 1;
    case 0x5B: return 2;
    case 0x4F: return 3;
    case 0x66: return 4;
    case 0x6D: return 5;
    case 0x7D: return 6;
    case 0x07: return 7;
    case 0x7F: return 8;
    case 0x6F: return 9;
    default: return -1;
  }
}

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


  if (this->bit_string_sensor_ != nullptr) {
    if (!this->bit_string_sensor_->has_state() ||
        this->bit_string_sensor_->state != frame) {

      ESP_LOGD(TAG, "Frame changed: %s", frame);
      this->bit_string_sensor_->publish_state(frame);
    }
  }
    
  auto bit = [bits](uint8_t n) -> bool {
   return ((bits >> n) & 1ULL) != 0;
  };

  if (this->cool_sensor_ != nullptr)
    this->cool_sensor_->publish_state(bit(0));

  if (this->auto_sensor_ != nullptr)
    this->auto_sensor_->publish_state(bit(1));

  if (this->run_sensor_ != nullptr)
    this->run_sensor_->publish_state(bit(3));

  if (this->timer_sensor_ != nullptr)
    this->timer_sensor_->publish_state(bit(7));

  if (this->fan_cont_sensor_ != nullptr)
    this->fan_cont_sensor_->publish_state(bit(8));

  if (this->fan_high_sensor_ != nullptr)
    this->fan_high_sensor_->publish_state(bit(9));

  if (this->fan_mid_sensor_ != nullptr)
    this->fan_mid_sensor_->publish_state(bit(10));

  if (this->fan_low_sensor_ != nullptr)
    this->fan_low_sensor_->publish_state(bit(11));

  if (this->heat_sensor_ != nullptr)
    this->heat_sensor_->publish_state(bit(15));

  if (this->inside_sensor_ != nullptr)
    this->inside_sensor_->publish_state(bit(33));

  if (this->zone1_sensor_ != nullptr)
    this->zone1_sensor_->publish_state(bit(21));

  if (this->zone2_sensor_ != nullptr)
    this->zone2_sensor_->publish_state(bit(14));

  if (this->zone3_sensor_ != nullptr)
    this->zone3_sensor_->publish_state(bit(12));

  if (this->zone4_sensor_ != nullptr)
    this->zone4_sensor_->publish_state(bit(13));

  if (this->zone5_sensor_ != nullptr)
    this->zone5_sensor_->publish_state(!bit(2));

  if (this->zone6_sensor_ != nullptr)
    this->zone6_sensor_->publish_state(!bit(6));

  if (this->zone7_sensor_ != nullptr)
    this->zone7_sensor_->publish_state(!bit(5));

  if (this->zone8_sensor_ != nullptr)
    this->zone8_sensor_->publish_state(!bit(4));

  const int digit1 = decode_digit(
      bit(39),
      bit(35),
      bit(34),
      bit(32),
      bit(36),
      bit(38),
      bit(37)
  );

  const int digit2 = decode_digit(
      bit(31),
      bit(24),
      bit(29),
      bit(30),
      bit(27),
      bit(25),
      bit(26)
  );

  const int digit3 = decode_digit(
      bit(20),
      bit(19),
      bit(16),
      bit(23),
      bit(22),
      bit(17),
      bit(18)
  );

  if (
      digit1 >= 0 &&
      digit2 >= 0 &&
      digit3 >= 0
  ) {

    float value =
        digit1 * 100 +
        digit2 * 10 +
        digit3;

    if (bit(28))
      value /= 10.0f;

    if (this->setpoint_sensor_ != nullptr)
      this->setpoint_sensor_->publish_state(value);
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
      static_cast<unsigned long>(BIT_THRESHOLD_US)
  );

  ESP_LOGCONFIG(
      TAG,
      "  Frame gap threshold: %u us",
      static_cast<unsigned long>(FRAME_GAP_US)
  );

  ESP_LOGCONFIG(
      TAG,
      "  Frame length: %u bits",
      FRAME_BITS
  );
}


}  // namespace actron_ultima
}  // namespace esphome

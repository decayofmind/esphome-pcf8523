#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/time/real_time_clock.h"

namespace esphome {
namespace pcf8523 {

class PCF8523Component : public time::RealTimeClock, public i2c::I2CDevice {
  public:
    void setup() override;
    void update() override;
    void dump_config() override;
    float get_setup_priority() const override;
    void read_time();
    void write_time();

  protected:
    bool read_rtc_();
    bool write_rtc_();
    union PCF8523Reg {
      struct {
        // Control_1 register
        uint8_t correction_int : 1;
        uint8_t alarm_int_enabled : 1;
        uint8_t second_int_enabled : 1;
        uint8_t hour_mode : 1;
        uint8_t sw_reset : 1;
        bool stop : 1;
        uint8_t : 1;
        uint8_t cap_sel : 1;

        // Control_2 register
        uint8_t countdown_timer_b : 1;
        uint8_t countdown_timer_a : 1;
        uint8_t watchdog_timer : 1;
        uint8_t alarm_int : 1;
        uint8_t second_int : 1;
        uint8_t countdown_b_int : 1;
        uint8_t countdown_a_int : 1;
        uint8_t watchdog_int : 1;

        // Control_3 register
        uint8_t battery_low_int : 1;
        uint8_t battery_switch_int : 1;
        uint8_t battery_status : 1;
        uint8_t battery_low_flag : 1;
        uint8_t battery_switch_flog : 1;
        uint8_t : 1;
        uint8_t power_management : 3;

        // Seconds register
        uint8_t second : 4;
        uint8_t second_10 : 3;
        bool clock_int : 1;

        // Minutes register
        uint8_t minute : 4;
        uint8_t minute_10 : 3;
        uint8_t : 1;

        // Hours register
        uint8_t hour : 4;
        uint8_t hour_10 : 2;
        uint8_t hour_12h : 4;
        uint8_t hour_12h_10 : 1;
        uint8_t hour_12h_ampm : 1;
        uint8_t : 2;

        // Days register
        uint8_t day : 4;
        uint8_t day_10 : 2;
        uint8_t : 2;

        // Weekdays register
        uint8_t weekday : 3;
        uint8_t : 5;

        // Months register
        uint8_t month : 4;
        uint8_t month_10 : 1;
        uint8_t : 3;

        // Years register
        uint8_t year : 4;
        uint8_t year_10 : 4;

        // Minute Alarm register
        uint8_t minute_alarm : 4;
        uint8_t minute_alarm_10 : 3;
        bool minute_alarm_enabled : 1;

        // Hour Alarm register
        uint8_t hour_alarm : 4;
        uint8_t hour_alarm_10 : 2;
        uint8_t hour_alarm_12h : 4;
        uint8_t hour_alarm_12h_10 : 1;
        uint8_t hour_alarm_12h_ampm : 1;
        uint8_t : 1;
        bool hour_alarm_enabled : 1;

        // Day Alarm register
        uint8_t day_alarm : 4;
        uint8_t day_alarm_10 : 2;
        uint8_t : 1;
        bool day_alarm_enabled : 1;

        // Weekday Alarm register
        uint8_t weekday_alarm : 3;
        uint8_t : 4;
        bool weekday_alarm_enabled : 1;

        // Offset register
        uint8_t offset : 7;
        uint8_t offset_mode : 1;

        // Timer and CLKOUT register
        uint8_t timer_b_enabled : 1;
        uint8_t timer_a_enabled : 2;
        uint8_t clkout_freq : 3;
        uint8_t timer_b_int_mode : 1;
        uint8_t timer_a_int_mode : 1;

        // Tmr_A_freq_ctrl register
        uint8_t timer_a_src_clk : 3;
        uint8_t : 5;

        // Timer A register
        uint8_t timer_a : 8;

        // Tmr_B_freq_ctrl register
        uint8_t timer_b_src_clk : 3;
        uint8_t : 1;
        uint8_t timer_b_pulse_width : 3;
        uint8_t : 1;

        // Timer B register
        uint8_t timer_b : 8;
      } reg;
      mutable uint8_t  raw[sizeof(reg)];
    } pcf8523_;
};

template<typename... Ts> class WriteAction : public Action<Ts...>, public Parented<PCF8523Component> {
  public:
  void play(Ts... x) override { this->parent_->write_time(); }
};
 
template<typename... Ts> class ReadAction : public Action<Ts...>, public Parented<PCF8523Component> {
  public:
  void play(Ts... x) override { this->parent_->read_time(); }
};
} // namespace pcf8523
} // namespace esphome

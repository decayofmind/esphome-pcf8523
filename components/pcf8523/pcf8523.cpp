#include "pcf8523.h"
#include "esphome/core/log.h"

// Datasheet:
// - https://www.nxp.com/docs/en/data-sheet/PCF8523.pdf

namespace esphome {
namespace pcf8523 {

static const char *const TAG = "PCF8523";

void PCF8523Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up PCF8523...");
  if (!this->read_rtc_()) {
    this->mark_failed();
  }
}

void PCF8523Component::update() { this->read_time(); }

void PCF8523Component::dump_config() {
  ESP_LOGCONFIG(TAG, "PCF8523:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with PCF8523 failed!");
  }
  ESP_LOGCONFIG(TAG, "  Timezone: '%s'", this->timezone_.c_str());
}

float PCF8523Component::get_setup_priority() const { return setup_priority::DATA; }

void PCF8523Component::read_time() {
  if (!this->read_rtc_()) {
    return;
  }
  if (pcf8523_.reg.osc_stop) {
    ESP_LOGW(TAG, "RTC halted, not syncing to system clock.");
    return;
  }
  ESPTime rtc_time{
    .second = uint8_t(pcf8523_.reg.second + 10 * pcf8523_.reg.second_10),
    .minute = uint8_t(pcf8523_.reg.minute + 10u * pcf8523_.reg.minute_10),
    .hour = uint8_t(pcf8523_.reg.hour + 10u * pcf8523_.reg.hour_10),
    .day_of_week = uint8_t(pcf8523_.reg.weekday),
    .day_of_month = uint8_t(pcf8523_.reg.day + 10u * pcf8523_.reg.day_10),
    .day_of_year = 1,  // ignored by recalc_timestamp_utc(false)
    .month = uint8_t(pcf8523_.reg.month + 10u * pcf8523_.reg.month_10),
    .year = uint16_t(pcf8523_.reg.year + 10u * pcf8523_.reg.year_10 + 2000),
    .is_dst = false,  // not used
    .timestamp = 0,   // overwritten by recalc_timestamp_utc(false)
  };
  rtc_time.recalc_timestamp_utc(false);
  if (!rtc_time.is_valid()) {
    ESP_LOGE(TAG, "Invalid RTC time, not syncing to system clock.");
    return;
  }
  time::RealTimeClock::synchronize_epoch_(rtc_time.timestamp);
}

void PCF8523Component::write_time() {
  auto now = time::RealTimeClock::utcnow();
  if (!now.is_valid()) {
    ESP_LOGE(TAG, "Invalid system time, not syncing to RTC.");
    return;
  }
  pcf8523_.reg.year = (now.year - 2000) % 10;
  pcf8523_.reg.year_10 = (now.year - 2000) / 10 % 10;
  pcf8523_.reg.month = now.month % 10;
  pcf8523_.reg.month_10 = now.month / 10;
  pcf8523_.reg.day = now.day_of_month % 10;
  pcf8523_.reg.day_10 = now.day_of_month / 10;
  pcf8523_.reg.weekday = now.day_of_week;
  pcf8523_.reg.hour = now.hour % 10;
  pcf8523_.reg.hour_10 = now.hour / 10;
  pcf8523_.reg.minute = now.minute % 10;
  pcf8523_.reg.minute_10 = now.minute / 10;
  pcf8523_.reg.second = now.second % 10;
  pcf8523_.reg.second_10 = now.second / 10;
  pcf8523_.reg.osc_stop = false;

  // Make sure power management is ON just in case
  //
  pcf8523_.reg.power_management = 0b000;

  this->write_rtc_();
}

bool PCF8523Component::read_rtc_() {
  if (!this->read_bytes(0, this->pcf8523_.raw, sizeof(this->pcf8523_.raw))) {
    ESP_LOGE(TAG, "Can't read I2C data.");
    return false;
  }
  ESP_LOGD(TAG, "Read  %0u%0u:%0u%0u:%0u%0u 20%0u%0u-%0u%0u-%0u%0u  OSC:%s CLKOUT:%0u", pcf8523_.reg.hour_10,
           pcf8523_.reg.hour, pcf8523_.reg.minute_10, pcf8523_.reg.minute, pcf8523_.reg.second_10, pcf8523_.reg.second,
           pcf8523_.reg.year_10, pcf8523_.reg.year, pcf8523_.reg.month_10, pcf8523_.reg.month, pcf8523_.reg.day_10,
           pcf8523_.reg.day, ONOFF(!pcf8523_.reg.osc_stop), pcf8523_.reg.clkout_control);
 
  return true;
}

bool PCF8523Component::write_rtc_() {
  if (!this->write_bytes(0, this->pcf8523_.raw, sizeof(this->pcf8523_.raw))) {
    ESP_LOGE(TAG, "Can't write I2C data.");
    return false;
  }
  ESP_LOGD(TAG, "Write %0u%0u:%0u%0u:%0u%0u 20%0u%0u-%0u%0u-%0u%0u  OSC:%s CLKOUT:%0u", pcf8523_.reg.hour_10,
           pcf8523_.reg.hour, pcf8523_.reg.minute_10, pcf8523_.reg.minute, pcf8523_.reg.second_10, pcf8523_.reg.second,
           pcf8523_.reg.year_10, pcf8523_.reg.year, pcf8523_.reg.month_10, pcf8523_.reg.month, pcf8523_.reg.day_10,
           pcf8523_.reg.day, ONOFF(!pcf8523_.reg.osc_stop), pcf8523_.reg.clkout_control);
  return true;
}
} // namespace pcf8523
} // namespace esphome

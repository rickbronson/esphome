#pragma once
#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"

#include <map>

namespace esphome {
namespace ld2451 {

#define CHECK_BIT(var, pos) (((var) >> (pos)) & 1)

// Commands
static const uint8_t CMD_ENABLE_CONF = 0x00FF;
static const uint8_t CMD_SET_DETECT_CONFIG = 0x0002;
static const uint8_t CMD_QUERY_TARGET_DETECTION = 0x0012;
static const uint8_t CMD_SET_RADAR_SENSITIVITY = 0x0003;
static const uint8_t CMD_QUERY_RADAR_SENSITIVITY = 0x0013;
static const uint8_t CMD_DISABLE_CONF = 0x00FE;
static const uint8_t CMD_GATE_SENS = 0x0064;
static const uint8_t CMD_VERSION = 0x00A0;
static const uint8_t CMD_SET_BAUD_RATE = 0x00A1;
static const uint8_t CMD_BT_PASSWORD = 0x00A9;
static const uint8_t CMD_MAC = 0x00A5;
static const uint8_t CMD_RESET = 0x00A2;
static const uint8_t CMD_RESTART = 0x00A3;
static const uint8_t CMD_BLUETOOTH = 0x00A4;

enum BaudRateStructure : uint8_t {
  BAUD_RATE_9600 = 1,
  BAUD_RATE_19200 = 2,
  BAUD_RATE_38400 = 3,
  BAUD_RATE_57600 = 4,
  BAUD_RATE_115200 = 5,
  BAUD_RATE_230400 = 6,
  BAUD_RATE_256000 = 7,
  BAUD_RATE_460800 = 8
};

static const std::map<std::string, uint8_t> BAUD_RATE_ENUM_TO_INT{
    {"9600", BAUD_RATE_9600},     {"19200", BAUD_RATE_19200},   {"38400", BAUD_RATE_38400},
    {"57600", BAUD_RATE_57600},   {"115200", BAUD_RATE_115200}, {"230400", BAUD_RATE_230400},
    {"256000", BAUD_RATE_256000}, {"460800", BAUD_RATE_460800}};

enum MoveDirectionStructure : uint8_t { MOVE_DIRECTION_AWAY = 0x00, MOVE_DIRECTION_APPROACH = 0x01 , MOVE_DIRECTION_BOTH = 0x02 };

static const std::map<std::string, uint8_t> MOVE_DIRECTION_ENUM_TO_INT {
	{"away", MOVE_DIRECTION_AWAY},
	{"approach", MOVE_DIRECTION_APPROACH},
	{"both", MOVE_DIRECTION_BOTH},
	};

static const std::map<uint8_t, std::string> MOVE_DIRECTION_INT_TO_ENUM{
	{MOVE_DIRECTION_AWAY, "away"},
	{MOVE_DIRECTION_APPROACH, "approach"},
	{MOVE_DIRECTION_BOTH, "both"}};

// Commands values
static const uint8_t CMD_MAX_MOVE_VALUE = 0x0000;
static const uint8_t CMD_MAX_STILL_VALUE = 0x0001;
static const uint8_t CMD_DURATION_VALUE = 0x0002;
// Command Header & Footer
static const uint8_t CMD_FRAME_HEADER[4] = {0xFD, 0xFC, 0xFB, 0xFA};
static const uint8_t CMD_FRAME_END[4] = {0x04, 0x03, 0x02, 0x01};
// Data Header & Footer
static const uint8_t DATA_FRAME_HEADER[4] = {0xF4, 0xF3, 0xF2, 0xF1};
static const uint8_t DATA_FRAME_END[4] = {0xF8, 0xF7, 0xF6, 0xF5};

enum PeriodicDataStructure : uint8_t {
  PD_ANGLE = 0,
  PD_DISTANCE = 1,
  PD_DIRECTION = 2,
  PD_SPEED = 3,
  PD_SNR = 4,
  PD_SZ = 5,  // 5 bytes array size
};

enum AckDataStructure : uint8_t { COMMAND = 6, COMMAND_STATUS = 7 };
	
//  char cmd[2] = {enable ? 0xFF : 0xFE, 0x00};
class LD2451Component : public Component, public uart::UARTDevice {
#ifdef USE_SENSOR
  SUB_SENSOR(target_angle)
  SUB_SENSOR(target_distance)
  SUB_SENSOR(target_direction)
  SUB_SENSOR(target_speed)
  SUB_SENSOR(target_snr)
#endif
#ifdef USE_TEXT_SENSOR
  SUB_TEXT_SENSOR(version)
  SUB_TEXT_SENSOR(mac)
#endif
#ifdef USE_SELECT
  SUB_SELECT(baud_rate)
  SUB_SELECT(move_direction)
#endif
#ifdef USE_SWITCH
  SUB_SWITCH(bluetooth)
#endif
#ifdef USE_BUTTON
  SUB_BUTTON(reset)
  SUB_BUTTON(restart)
  SUB_BUTTON(query)
#endif
#ifdef USE_NUMBER
  SUB_NUMBER(max_detect_distance)
  SUB_NUMBER(min_speed)
  SUB_NUMBER(no_target_timeout)
  SUB_NUMBER(effective_trigger_time)  // The alarm information will be reported only when the number of consecutive detections is met.
	SUB_NUMBER(snr_threshold_level)  // 3-8: The larger the value, the lower the sensitivity and the more difficult it is to detect the target.
#endif

 public:
  LD2451Component();
  void setup() override;
  void dump_config() override;
  void loop() override;
#ifdef USE_NUMBER
  void set_detect_config();
  void set_radar_sensitivity();
#endif
  void set_throttle(uint16_t value) { this->throttle_ = value; };
  void set_bluetooth_password(const std::string &password);
  void read_all_info();
  void restart_and_read_all_info();
  void set_bluetooth(bool enable);
  void set_baud_rate(const std::string &state);
  void factory_reset();

 protected:
  int two_byte_to_int_(char firstbyte, char secondbyte) { return (int16_t) (secondbyte << 8) + firstbyte; }
  void send_command_(uint8_t command_str, const uint8_t *command_value, int command_value_len);
  void set_config_mode_(bool enable);
  void handle_periodic_data_(uint8_t *buffer, int len);
  bool handle_ack_data_(uint8_t *buffer, int len);
  void readline_(int readch, uint8_t *buffer, int len);
  void query_parameters_();
  void get_version_();
  void get_mac_();
  void get_detect_config_();
  void get_radar_sensitivity_();
  void restart_();

  int32_t last_periodic_millis_ = millis();
  uint16_t throttle_;
  std::string version_;
  std::string mac_;
  std::string move_direction_;
	int32_t effective_trigger_time_;
	int32_t snr_threshold_level_;
	int32_t max_detect_distance_;
	int32_t min_speed_;
  int32_t no_target_timeout_;
};

}  // namespace ld2451
}  // namespace esphome

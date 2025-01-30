#include "ld2451.h"

#include <utility>
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#define highbyte(val) (uint8_t)((val) >> 8)
#define lowbyte(val) (uint8_t)((val) &0xff)

namespace esphome {
namespace ld2451 {

static const char *const TAG = "ld2451";

LD2451Component::LD2451Component() {}

void LD2451Component::dump_config() {
  ESP_LOGCONFIG(TAG, "LD2451:");
#ifdef USE_SWITCH
  LOG_SWITCH("  ", "BluetoothSwitch", this->bluetooth_switch_);
#endif
#ifdef USE_BUTTON
  LOG_BUTTON("  ", "ResetButton", this->reset_button_);
  LOG_BUTTON("  ", "RestartButton", this->restart_button_);
  LOG_BUTTON("  ", "QueryButton", this->query_button_);
#endif
#ifdef USE_SENSOR
  LOG_SENSOR("  ", "TargetAngleSensor", this->target_angle_sensor_);
  LOG_SENSOR("  ", "TargetDistanceSensor", this->target_distance_sensor_);
  LOG_SENSOR("  ", "TargetDirectionSensor", this->target_direction_sensor_);
  LOG_SENSOR("  ", "TargetSpeedSensor", this->target_speed_sensor_);
  LOG_SENSOR("  ", "TargetSNR", this->target_snr_sensor_);
#endif
#ifdef USE_TEXT_SENSOR
  LOG_TEXT_SENSOR("  ", "VersionTextSensor", this->version_text_sensor_);
  LOG_TEXT_SENSOR("  ", "MacTextSensor", this->mac_text_sensor_);
#endif
#ifdef USE_SELECT
  LOG_SELECT("  ", "MoveDirectionSelect", this->move_direction_select_);
  LOG_SELECT("  ", "BaudRateSelect", this->baud_rate_select_);
#endif
#ifdef USE_NUMBER
  LOG_NUMBER("  ", "EffectiveTriggerTime", this->effective_trigger_time_number_);
  LOG_NUMBER("  ", "SnrThresholdLevel", this->snr_threshold_level_number_);
  LOG_NUMBER("  ", "MaxDetectDistance", this->max_detect_distance_number_);
  LOG_NUMBER("  ", "MinSpeed", this->min_speed_number_);
  LOG_NUMBER("  ", "NoTargetTimeout", this->no_target_timeout_number_);
#endif
  this->read_all_info();
  ESP_LOGCONFIG(TAG, "  Throttle_ : %ums", this->throttle_);
  ESP_LOGCONFIG(TAG, "  MAC Address : %s", const_cast<char *>(this->mac_.c_str()));
  ESP_LOGCONFIG(TAG, "  Firmware Version : %s", const_cast<char *>(this->version_.c_str()));
}

void LD2451Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up LD2451...");
  this->read_all_info();
  ESP_LOGCONFIG(TAG, "Mac Address : %s", const_cast<char *>(this->mac_.c_str()));
  ESP_LOGCONFIG(TAG, "Firmware Version : %s", const_cast<char *>(this->version_.c_str()));
  ESP_LOGCONFIG(TAG, "LD2451 setup complete.");
}

void LD2451Component::read_all_info() {
  this->set_config_mode_(true);
  this->get_version_();
  this->get_mac_();
	this->get_detect_config_();
  this->get_radar_sensitivity_();
  this->set_config_mode_(false);
#ifdef USE_SELECT
  const auto baud_rate = std::to_string(this->parent_->get_baud_rate());
  if (this->baud_rate_select_ != nullptr && this->baud_rate_select_->state != baud_rate) {
    this->baud_rate_select_->publish_state(baud_rate);
  }
#endif
}

void LD2451Component::restart_and_read_all_info() {
  this->set_config_mode_(true);
  this->restart_();
  this->set_timeout(1000, [this]() { this->read_all_info(); });
}

void LD2451Component::loop() {
  const int max_line_length = 80;
  static uint8_t buffer[max_line_length];

  while (available()) {
    this->readline_(read(), buffer, max_line_length);
  }
}

void LD2451Component::send_command_(uint8_t command, const uint8_t *command_value, int command_value_len) {
  ESP_LOGV(TAG, "Sending COMMAND %02X", command);
  // frame start bytes
  this->write_array(CMD_FRAME_HEADER, 4);
  // length bytes
  int len = 2;
  if (command_value != nullptr)
    len += command_value_len;
  this->write_byte(lowbyte(len));
  this->write_byte(highbyte(len));

  // command
  this->write_byte(lowbyte(command));
  this->write_byte(highbyte(command));

  // command value bytes
  if (command_value != nullptr) {
    for (int i = 0; i < command_value_len; i++) {
      this->write_byte(command_value[i]);
    }
  }
  // frame end bytes
  this->write_array(CMD_FRAME_END, 4);
  // FIXME to remove
  delay(50);  // NOLINT
}

#define MY_DEBUG
void LD2451Component::handle_periodic_data_(uint8_t *buffer, int len) {
#ifdef MY_DEBUG
  char cntr2, buf[200], *p_buf = buf;
	const char* units[] = {"deg", "m", "0=away", "km/h", "snr", };

	if (len >= 17) {
		ESP_LOGV(TAG, "Will handle Periodic Data");
		for (cntr2 = 8; cntr2 < len - 4; cntr2++) {
			p_buf += sprintf(p_buf, "%3d %s, ", (unsigned) *(buffer + cntr2), units[(cntr2 - 8) % 5]);
			}
		ESP_LOGV(TAG, buf);
		}
#endif
	if (len < 17)
    return;  // 4 frame start bytes + 2 length bytes + (2 quanity/info + 5 vehicle byte) + 4 frame end bytes
  int32_t cntr, target_quanity = buffer[6];
	if (!target_quanity)
		return;
  if (buffer[0] != DATA_FRAME_HEADER[0] || buffer[1] != DATA_FRAME_HEADER[1] ||
		buffer[2] != DATA_FRAME_HEADER[2] || buffer[3] != DATA_FRAME_HEADER[3])  // check 4 frame start bytes
    return;

  /*
    Reduce data update rate to prevent home assistant database size grow fast
  */
  int32_t current_millis = millis();
  if (current_millis - last_periodic_millis_ < this->throttle_)
    return;
  last_periodic_millis_ = current_millis;

	for (buffer += 8, cntr = 0; cntr < 1 /* debug only */; cntr++, buffer += PD_SZ) {  // move buffer ahead to start of array
  /*
  ANGLE = byte 0,
  DISTANCE = 1,
  DIRECTION = 2,
  SPEED = 3,
  SNR = 4,
  */
#ifdef USE_SENSOR
  if (this->target_angle_sensor_ != nullptr) {
    int new_target_angle = buffer[PD_ANGLE];
    if (this->target_angle_sensor_->get_state() != new_target_angle)
      this->target_angle_sensor_->publish_state(new_target_angle);
  }
  if (this->target_distance_sensor_ != nullptr) {
    int new_target_distance = buffer[PD_DISTANCE];
    if (this->target_distance_sensor_->get_state() != new_target_distance)
      this->target_distance_sensor_->publish_state(new_target_distance);
  }
  if (this->target_direction_sensor_ != nullptr) {
    int new_target_direction = buffer[PD_DIRECTION];
    if (this->target_direction_sensor_->get_state() != new_target_direction)
      this->target_direction_sensor_->publish_state(new_target_direction);
  }
  if (this->target_speed_sensor_ != nullptr) {
    int new_target_speed = buffer[PD_SPEED];
    if (this->target_speed_sensor_->get_state() != new_target_speed)
      this->target_speed_sensor_->publish_state(new_target_speed);
  }
  if (this->target_snr_sensor_ != nullptr) {
    int new_target_snr = buffer[PD_SNR];
    if (this->target_snr_sensor_->get_state() != new_target_snr)
      this->target_snr_sensor_->publish_state(new_target_snr);
  }
		}
#endif
}

const char VERSION_FMT[] = "%u.%02X.%02X%02X%02X%02X";

std::string format_version(uint8_t *buffer) {
  std::string::size_type version_size = 256;
  std::string version;
  do {
    version.resize(version_size + 1);
    version_size = std::snprintf(&version[0], version.size(), VERSION_FMT, buffer[13], buffer[12], buffer[17],
                                 buffer[16], buffer[15], buffer[14]);
  } while (version_size + 1 > version.size());
  version.resize(version_size);
  return version;
}

const char MAC_FMT[] = "%02X:%02X:%02X:%02X:%02X:%02X";

const std::string UNKNOWN_MAC("unknown");
const std::string NO_MAC("08:05:04:03:02:01");

std::string format_mac(uint8_t *buffer) {
  std::string::size_type mac_size = 256;
  std::string mac;
  do {
    mac.resize(mac_size + 1);
    mac_size = std::snprintf(&mac[0], mac.size(), MAC_FMT, buffer[10], buffer[11], buffer[12], buffer[13], buffer[14],
                             buffer[15]);
  } while (mac_size + 1 > mac.size());
  mac.resize(mac_size);
  if (mac == NO_MAC) {
    return UNKNOWN_MAC;
  }
  return mac;
}

bool LD2451Component::handle_ack_data_(uint8_t *buffer, int len) {
  ESP_LOGV(TAG, "Handling ACK DATA for COMMAND %02X", buffer[COMMAND]);
  if (len < 10) {
    ESP_LOGE(TAG, "Error with last command : incorrect length");
    return true;
  }
  if (buffer[0] != CMD_FRAME_HEADER[0] || buffer[1] != CMD_FRAME_HEADER[1] || buffer[2] != CMD_FRAME_HEADER[2] || buffer[3] != CMD_FRAME_HEADER[3]) {  // check 4 frame start bytes
    ESP_LOGE(TAG, "Error with last command : incorrect Header");
    return true;
  }
  if (buffer[COMMAND_STATUS] != 0x01) {
    ESP_LOGE(TAG, "Error with last command : status != 0x01");
    return true;
  }
  if (this->two_byte_to_int_(buffer[8], buffer[9]) != 0x00) {
    ESP_LOGE(TAG, "Error with last command , last buffer was: %u , %u", buffer[8], buffer[9]);
    return true;
  }

  switch (buffer[COMMAND]) {
    case lowbyte(CMD_ENABLE_CONF):
      ESP_LOGV(TAG, "Handled Enable conf command");
      break;
    case lowbyte(CMD_DISABLE_CONF):
      ESP_LOGV(TAG, "Handled Disabled conf command");
      break;
    case lowbyte(CMD_SET_BAUD_RATE):
      ESP_LOGV(TAG, "Handled baud rate change command");
#ifdef USE_SELECT
      if (this->baud_rate_select_ != nullptr) {
        ESP_LOGE(TAG, "Change baud rate component config to %s and reinstall", this->baud_rate_select_->state.c_str());
      }
#endif
      break;
    case lowbyte(CMD_VERSION):
      this->version_ = format_version(buffer);
      ESP_LOGV(TAG, "FW Version is: %s", const_cast<char *>(this->version_.c_str()));
#ifdef USE_TEXT_SENSOR
      if (this->version_text_sensor_ != nullptr) {
        this->version_text_sensor_->publish_state(this->version_);
      }
#endif
      break;
    case lowbyte(CMD_QUERY_TARGET_DETECTION): {
      std::string move_direction = MOVE_DIRECTION_INT_TO_ENUM.at(buffer[11]);
      this->max_detect_distance_ = buffer[10];
      this->min_speed_ = buffer[12];
      this->no_target_timeout_ = buffer[13];
      ESP_LOGV(TAG, "Max detect distance is: %d", this->max_detect_distance_);
      ESP_LOGV(TAG, "Move direction is: %s", const_cast<char *>(move_direction.c_str()));
      ESP_LOGV(TAG, "Min speed is: %d", this->min_speed_);
      ESP_LOGV(TAG, "No target timeout is: %d", this->no_target_timeout_);
#ifdef USE_NUMBER
      if (this->max_detect_distance_number_ != nullptr &&
          (!this->max_detect_distance_number_->has_state() ||
           this->max_detect_distance_number_->state != this->max_detect_distance_)) {
        this->max_detect_distance_number_->publish_state(this->max_detect_distance_);
      }
      if (this->min_speed_number_ != nullptr &&
          (!this->min_speed_number_->has_state() ||
           this->min_speed_number_->state != this->min_speed_)) {
        this->min_speed_number_->publish_state(this->min_speed_);
      }
      if (this->no_target_timeout_number_ != nullptr &&
          (!this->no_target_timeout_number_->has_state() ||
           this->no_target_timeout_number_->state != this->no_target_timeout_)) {
        this->no_target_timeout_number_->publish_state(this->no_target_timeout_);
      }
#endif
#ifdef USE_SELECT
      if (this->move_direction_select_ != nullptr &&
          this->move_direction_select_->state != move_direction) {
        this->move_direction_select_->publish_state(move_direction);
      }
#endif
    } break;
	case lowbyte(CMD_QUERY_RADAR_SENSITIVITY): {
      this->effective_trigger_time_ = buffer[10];
      this->snr_threshold_level_ = buffer[11];
      ESP_LOGV(TAG, "effective_trigger_time_ is: %d", this->effective_trigger_time_);
      ESP_LOGV(TAG, "snr_threshold_level_ is: %d", this->snr_threshold_level_);
#ifdef USE_NUMBER
      if (this->effective_trigger_time_number_ != nullptr &&
          (!this->effective_trigger_time_number_->has_state() ||
           this->effective_trigger_time_number_->state != this->effective_trigger_time_)) {
        this->effective_trigger_time_number_->publish_state(this->effective_trigger_time_);
      }
      if (this->snr_threshold_level_number_ != nullptr &&
          (!this->snr_threshold_level_number_->has_state() ||
           this->snr_threshold_level_number_->state != this->snr_threshold_level_)) {
        this->snr_threshold_level_number_->publish_state(this->snr_threshold_level_);
      }
#endif
		} break;
    case lowbyte(CMD_MAC):
      if (len < 20) {
        return false;
      }
      this->mac_ = format_mac(buffer);
      ESP_LOGV(TAG, "MAC Address is: %s", const_cast<char *>(this->mac_.c_str()));
#ifdef USE_TEXT_SENSOR
      if (this->mac_text_sensor_ != nullptr) {
        this->mac_text_sensor_->publish_state(this->mac_);
      }
#endif
#ifdef USE_SWITCH
      if (this->bluetooth_switch_ != nullptr) {
        this->bluetooth_switch_->publish_state(this->mac_ != UNKNOWN_MAC);
      }
#endif
      break;
    case lowbyte(CMD_GATE_SENS):
      ESP_LOGV(TAG, "Handled sensitivity command");
      break;
    case lowbyte(CMD_BLUETOOTH):
      ESP_LOGV(TAG, "Handled bluetooth command");
      break;
    case lowbyte(CMD_SET_DETECT_CONFIG):
      ESP_LOGV(TAG, "Handled set detect config command");
      break;
    case lowbyte(CMD_SET_RADAR_SENSITIVITY):
      ESP_LOGV(TAG, "Handled set radar sensitivity command");
      break;
    case lowbyte(CMD_BT_PASSWORD):
      ESP_LOGV(TAG, "Handled set bluetooth password command");
      break;
    default:
      break;
  }

  return true;
}

void LD2451Component::readline_(int readch, uint8_t *buffer, int len) {
  static int pos = 0;

  if (readch >= 0) {
    if (pos < len - 1) {
      buffer[pos++] = readch;
      buffer[pos] = 0;
    } else {
      pos = 0;
    }
    if (pos >= 4) {
      if (buffer[pos - 4] == DATA_FRAME_END[0] && buffer[pos - 3] == DATA_FRAME_END[1] && buffer[pos - 2] == DATA_FRAME_END[2] && buffer[pos - 1] == DATA_FRAME_END[3]) {
        this->handle_periodic_data_(buffer, pos);
        pos = 0;  // Reset position index ready for next time
      } else if (buffer[pos - 4] == CMD_FRAME_END[0] && buffer[pos - 3] == CMD_FRAME_END[1] && buffer[pos - 2] == CMD_FRAME_END[2] &&
                 buffer[pos - 1] == CMD_FRAME_END[3]) {
        ESP_LOGV(TAG, "Will handle ACK Data");
        if (this->handle_ack_data_(buffer, pos)) {
          pos = 0;  // Reset position index ready for next time
        } else {
          ESP_LOGV(TAG, "ACK Data incomplete");
        }
      }
    }
  }
}

void LD2451Component::set_config_mode_(bool enable) {
  uint8_t cmd = enable ? CMD_ENABLE_CONF : CMD_DISABLE_CONF;
  uint8_t cmd_value[2] = {0x01, 0x00};
  this->send_command_(cmd, enable ? cmd_value : nullptr, 2);
}

void LD2451Component::set_bluetooth(bool enable) {
  this->set_config_mode_(true);
  uint8_t enable_cmd_value[2] = {0x01, 0x00};
  uint8_t disable_cmd_value[2] = {0x00, 0x00};
  this->send_command_(CMD_BLUETOOTH, enable ? enable_cmd_value : disable_cmd_value, 2);
  this->set_timeout(200, [this]() { this->restart_and_read_all_info(); });
}

void LD2451Component::set_baud_rate(const std::string &state) {
  this->set_config_mode_(true);
  uint8_t cmd_value[2] = {BAUD_RATE_ENUM_TO_INT.at(state), 0x00};
  this->send_command_(CMD_SET_BAUD_RATE, cmd_value, 2);
  this->set_timeout(200, [this]() { this->restart_(); });
}

void LD2451Component::set_bluetooth_password(const std::string &password) {
  if (password.length() != 6) {
    ESP_LOGE(TAG, "set_bluetooth_password(): invalid password length, must be exactly 6 chars '%s'", password.c_str());
    return;
  }
  this->set_config_mode_(true);
  uint8_t cmd_value[6];
  std::copy(password.begin(), password.end(), std::begin(cmd_value));
  this->send_command_(CMD_BT_PASSWORD, cmd_value, 6);
  this->set_config_mode_(false);
}

void LD2451Component::factory_reset() {
  this->set_config_mode_(true);
  this->send_command_(CMD_RESET, nullptr, 0);
  this->set_timeout(200, [this]() { this->restart_and_read_all_info(); });
}

void LD2451Component::restart_() { this->send_command_(CMD_RESTART, nullptr, 0); }

void LD2451Component::get_version_() { this->send_command_(CMD_VERSION, nullptr, 0); }

void LD2451Component::get_mac_() {
  uint8_t cmd_value[2] = {0x01, 0x00};
  this->send_command_(CMD_MAC, cmd_value, 2);
}

void LD2451Component::get_detect_config_() { this->send_command_(CMD_QUERY_TARGET_DETECTION, nullptr, 0); }

void LD2451Component::get_radar_sensitivity_() { this->send_command_(CMD_QUERY_RADAR_SENSITIVITY, nullptr, 0); }

#ifdef USE_NUMBER

void LD2451Component::set_detect_config() {
  if (!this->max_detect_distance_number_->has_state() || !this->move_direction_select_->has_state() ||
		!this->min_speed_number_->has_state() || !this->no_target_timeout_number_->has_state()) {
    return;
  }
  int max_detect_distance = static_cast<int>(this->max_detect_distance_number_->state);
  int move_direction = this->move_direction_select_->index_of(this->move_direction_select_->state).value();
  int min_speed = static_cast<int>(this->min_speed_number_->state);
  int no_target_timeout = static_cast<int>(this->no_target_timeout_number_->state);
  uint8_t value[] = {
		lowbyte(max_detect_distance),
		lowbyte(move_direction),
		lowbyte(min_speed),
		lowbyte(no_target_timeout),
		};
	
  this->set_config_mode_(true);
  this->send_command_(CMD_SET_DETECT_CONFIG, value, sizeof(value));
  delay(50);  // NOLINT
  this->get_detect_config_();
  this->set_config_mode_(false);
}

void LD2451Component::set_radar_sensitivity() {
  if (!this->effective_trigger_time_number_->has_state() || !this->snr_threshold_level_number_->has_state()) {
    return;
  }
  int effective_trigger_time = static_cast<int>(this->effective_trigger_time_number_->state);
  int snr_threshold_level = static_cast<int>(this->snr_threshold_level_number_->state);
  uint8_t value[] = {
		lowbyte(effective_trigger_time),
		lowbyte(snr_threshold_level),
		0, 0,
		};
  this->set_config_mode_(true);
  this->send_command_(CMD_SET_RADAR_SENSITIVITY, value, sizeof(value));
  delay(50);  // NOLINT
  this->get_radar_sensitivity_();
  this->set_config_mode_(false);
}
#endif

}  // namespace ld2451
}  // namespace esphome

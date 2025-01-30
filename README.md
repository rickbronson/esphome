
  Experimental addition of the Hi-Link (Shenzhen Hailingke Electronics Co., Ltd) LD2451 Vehicle Status Detection Sensor

  Below is an example yaml file:


```
substitutions:
  ld_status_pin: GPIO2
  ld_rx_pin: GPIO13
  ld_tx_pin: GPIO14
  mini_led: GPIO33

esphome:
  name: radar
  friendly_name: radar

esp32:
  board: esp32dev
  framework:
    type: esp-idf
#    type: arduino

# Enable logging
logger:
  level: verbose
  logs:
    component: error

# Enable Home Assistant API
api:
  reboot_timeout: 3600s

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

  # Enable fallback hotspot (captive portal) in case wifi connection fails
  ap:
    ssid: "Radar Fallback Hotspot"

web_server:
  port: 80

captive_portal:

external_components:
  - source:
      type: local
      path: esphome/components
    components: [ ld2451 ]
# workaround for web_server_idf:070][httpd]: Only application/x-www-form-urlencoded supported for POST request
  - source: github://esphome/esphome@2024.5.5
    components: [ http_request, json ]

uart:
  id: ld2451_uart
  tx_pin: $ld_tx_pin
  rx_pin: $ld_rx_pin
  baud_rate: 115200
  parity: NONE
  stop_bits: 1

ld2451:
  uart_id: ld2451_uart
  throttle: 1500ms

select:
  - platform: ld2451
    move_direction:
      name: Move direction

button:
  - platform: ld2451
    factory_reset:
      name: "Factory reset module"
    restart:
      name: "Restart module"
    query_params:
      name: "Query parameters"

number:
  - platform: ld2451
    effective_trigger_time:
      name: Effective trigger time
    snr_threshold_level:
      name: SNR threshold level
    max_detect_distance:
      name: Max detection distance
    min_speed:
      name: Minimum speed
    no_target_timeout:
      name: No target delay time
  
text_sensor:
  - platform: ld2451
    version:
      name: "Module version" 
    mac_address:
      name: "Module mac address"

switch:
  - platform: ld2451
    bluetooth:
      name: "Bluetooth control"
  - platform: gpio
    pin: $mini_led
    id: red_led
    name: "Mini LED"
    inverted: True

binary_sensor:
  - platform: gpio
    pin: 
      number: $ld_status_pin
      mode: INPUT_PULLUP
    name: "OT1_Status"
    filters:
      - delayed_on_off: 1s

sensor:
  - platform: ld2451
    target_angle:
      name: "Angle (degrees)"
    target_distance:
      name: "Distance (meters)"
    target_direction:
      name: "Direction (0=away,1=approach)"
    target_speed:
      name: "Speed (km/h)"
    target_snr:
      name: "Signal/noise ratio"
```

# ESPHome [![Discord Chat](https://img.shields.io/discord/429907082951524364.svg)](https://discord.gg/KhAMKrd) [![GitHub release](https://img.shields.io/github/release/esphome/esphome.svg)](https://GitHub.com/esphome/esphome/releases/)

<a href="https://esphome.io/">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="https://esphome.io/_static/logo-text-on-dark.svg", alt="ESPHome Logo">
    <img src="https://esphome.io/_static/logo-text-on-light.svg" alt="ESPHome Logo">
  </picture>
</a>

**Documentation:** https://esphome.io/

For issues, please go to [the issue tracker](https://github.com/esphome/issues/issues).

For feature requests, please see [feature requests](https://github.com/esphome/feature-requests/issues).

[![ESPHome - A project from the Open Home Foundation](https://www.openhomefoundation.org/badges/esphome.png)](https://www.openhomefoundation.org/)

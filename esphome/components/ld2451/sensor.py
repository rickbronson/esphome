import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_DISTANCE,
    UNIT_METER,
    UNIT_DEGREES,
    UNIT_CENTIMETER,
    UNIT_PERCENT,
    UNIT_KILOMETER_PER_HOUR,
    CONF_LIGHT,
    DEVICE_CLASS_ILLUMINANCE,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_SIGNAL,
    ICON_FLASH,
    ICON_MOTION_SENSOR,
    ICON_LIGHTBULB,
)
from . import CONF_LD2451_ID, LD2451Component

DEPENDENCIES = ["ld2451"]
CONF_TARGET_ANGLE = "target_angle"
CONF_TARGET_DISTANCE = "target_distance"
CONF_TARGET_DIRECTION = "target_direction"
CONF_TARGET_SPEED = "target_speed"
CONF_TARGET_SNR = "target_snr"
CONF_MOVE_ENERGY = "move_energy"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LD2451_ID): cv.use_id(LD2451Component),
        cv.Optional(CONF_TARGET_ANGLE): sensor.sensor_schema(
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_DEGREES,
            icon=ICON_SIGNAL,
        ),
        cv.Optional(CONF_TARGET_DISTANCE): sensor.sensor_schema(
            device_class=DEVICE_CLASS_DISTANCE,
            unit_of_measurement=UNIT_METER,
            icon=ICON_SIGNAL,
        ),
        cv.Optional(CONF_TARGET_DIRECTION): sensor.sensor_schema(
            icon=ICON_MOTION_SENSOR,
        ),
        cv.Optional(CONF_TARGET_SPEED): sensor.sensor_schema(
            unit_of_measurement=UNIT_KILOMETER_PER_HOUR,
            icon=ICON_FLASH,
        ),
        cv.Optional(CONF_TARGET_SNR): sensor.sensor_schema(
            icon=ICON_FLASH,
        ),
    }
)

CONFIG_SCHEMA = CONFIG_SCHEMA.extend(
    {
        cv.Optional(f"g{x}"): cv.Schema(
            {
            }
        )
        for x in range(9)
    }
)


async def to_code(config):
    ld2451_component = await cg.get_variable(config[CONF_LD2451_ID])
    if target_angle_config := config.get(CONF_TARGET_ANGLE):
        sens = await sensor.new_sensor(target_angle_config)
        cg.add(ld2451_component.set_target_angle_sensor(sens))
    if target_distance_config := config.get(CONF_TARGET_DISTANCE):
        sens = await sensor.new_sensor(target_distance_config)
        cg.add(ld2451_component.set_target_distance_sensor(sens))
    if target_direction_config := config.get(CONF_TARGET_DIRECTION):
        sens = await sensor.new_sensor(target_direction_config)
        cg.add(ld2451_component.set_target_direction_sensor(sens))
    if target_speed_config := config.get(CONF_TARGET_SPEED):
        sens = await sensor.new_sensor(target_speed_config)
        cg.add(ld2451_component.set_target_speed_sensor(sens))
    if target_snr_config := config.get(CONF_TARGET_SNR):
        sens = await sensor.new_sensor(target_snr_config)
        cg.add(ld2451_component.set_target_snr_sensor(sens))

import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_TIMEOUT,
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    DEVICE_CLASS_SPEED,
    UNIT_SECOND,
    UNIT_PERCENT,
    ENTITY_CATEGORY_CONFIG,
    ICON_MOTION_SENSOR,
    ICON_TIMELAPSE,
    ICON_LIGHTBULB,
    ICON_SIGNAL,
    ICON_RULER,
    ICON_POWER,
)
from .. import CONF_LD2451_ID, LD2451Component, ld2451_ns

EffectiveTriggerTimeNumber = ld2451_ns.class_("EffectiveTriggerTimeNumber", number.Number)
SnrThresholdLevelNumber = ld2451_ns.class_("SnrThresholdLevelNumber", number.Number)
MaxDetectDistanceNumber = ld2451_ns.class_("MaxDetectDistanceNumber", number.Number)
MinSpeedNumber = ld2451_ns.class_("MinSpeedNumber", number.Number)
NoTargetTimeoutNumber = ld2451_ns.class_("NoTargetTimeoutNumber", number.Number)

CONF_EFFECTIVE_TRIGGER_TIME = "effective_trigger_time"
CONF_SNR_THRESHOLD_LEVEL = "snr_threshold_level"
CONF_MAX_DETECT_DISTANCE = "max_detect_distance"
CONF_MIN_SPEED = "min_speed"
CONF_NO_TARGET_TIMEOUT = "no_target_timeout"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LD2451_ID): cv.use_id(LD2451Component),
        cv.Optional(CONF_MAX_DETECT_DISTANCE): number.number_schema(
            MaxDetectDistanceNumber,
            device_class=DEVICE_CLASS_DISTANCE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_MOTION_SENSOR,
        ),
        cv.Optional(CONF_EFFECTIVE_TRIGGER_TIME): number.number_schema(
            EffectiveTriggerTimeNumber,
            device_class=DEVICE_CLASS_DURATION,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_TIMELAPSE,
        ),
        cv.Optional(CONF_SNR_THRESHOLD_LEVEL): number.number_schema(
            SnrThresholdLevelNumber,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_SIGNAL,
        ),
        cv.Optional(CONF_MAX_DETECT_DISTANCE): number.number_schema(
            MaxDetectDistanceNumber,
            device_class=DEVICE_CLASS_DISTANCE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_RULER,
        ),
        cv.Optional(CONF_MIN_SPEED): number.number_schema(
            MinSpeedNumber,
            device_class=DEVICE_CLASS_SPEED,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_POWER,
        ),
        cv.Optional(CONF_NO_TARGET_TIMEOUT): number.number_schema(
            NoTargetTimeoutNumber,
            device_class=DEVICE_CLASS_DURATION,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_TIMELAPSE,
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
    if timeout_config := config.get(CONF_TIMEOUT):
        n = await number.new_number(
            timeout_config, min_value=0, max_value=65535, step=1
        )
        await cg.register_parented(n, config[CONF_LD2451_ID])
        cg.add(ld2451_component.set_timeout_number(n))
    if effective_trigger_time_config := config.get(CONF_EFFECTIVE_TRIGGER_TIME):
        n = await number.new_number(
            effective_trigger_time_config, min_value=1, max_value=10, step=1
        )
        await cg.register_parented(n, config[CONF_LD2451_ID])
        cg.add(ld2451_component.set_effective_trigger_time_number(n))
    if snr_threshold_level_config := config.get(CONF_SNR_THRESHOLD_LEVEL):
        n = await number.new_number(
            snr_threshold_level_config, min_value=3, max_value=8, step=1
        )
        await cg.register_parented(n, config[CONF_LD2451_ID])
        cg.add(ld2451_component.set_snr_threshold_level_number(n))
    if max_detect_distance_config := config.get(CONF_MAX_DETECT_DISTANCE):
        n = await number.new_number(
            max_detect_distance_config, min_value=10, max_value=255, step=1
        )
        await cg.register_parented(n, config[CONF_LD2451_ID])
        cg.add(ld2451_component.set_max_detect_distance_number(n))
    if min_speed_config := config.get(CONF_MIN_SPEED):
        n = await number.new_number(
            min_speed_config, min_value=1, max_value=120, step=1
        )
        await cg.register_parented(n, config[CONF_LD2451_ID])
        cg.add(ld2451_component.set_min_speed_number(n))
    if no_target_timeout_config := config.get(CONF_NO_TARGET_TIMEOUT):
        n = await number.new_number(
            no_target_timeout_config, min_value=0, max_value=255, step=1
        )
        await cg.register_parented(n, config[CONF_LD2451_ID])
        cg.add(ld2451_component.set_no_target_timeout_number(n))

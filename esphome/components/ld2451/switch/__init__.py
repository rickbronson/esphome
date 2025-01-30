import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_SWITCH,
    ICON_BLUETOOTH,
    ENTITY_CATEGORY_CONFIG,
    ICON_PULSE,
)
from .. import CONF_LD2451_ID, LD2451Component, ld2451_ns

BluetoothSwitch = ld2451_ns.class_("BluetoothSwitch", switch.Switch)

CONF_BLUETOOTH = "bluetooth"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_LD2451_ID): cv.use_id(LD2451Component),
    cv.Optional(CONF_BLUETOOTH): switch.switch_schema(
        BluetoothSwitch,
        device_class=DEVICE_CLASS_SWITCH,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon=ICON_BLUETOOTH,
    ),
}


async def to_code(config):
    ld2451_component = await cg.get_variable(config[CONF_LD2451_ID])
    if bluetooth_config := config.get(CONF_BLUETOOTH):
        s = await switch.new_switch(bluetooth_config)
        await cg.register_parented(s, config[CONF_LD2451_ID])
        cg.add(ld2451_component.set_bluetooth_switch(s))

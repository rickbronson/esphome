import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    CONF_BAUD_RATE,
    ICON_THERMOMETER,
    ICON_SCALE,
    ICON_RULER,
)
from .. import CONF_LD2451_ID, LD2451Component, ld2451_ns

BaudRateSelect = ld2451_ns.class_("BaudRateSelect", select.Select)
MoveDirectionSelect = ld2451_ns.class_("MoveDirectionSelect", select.Select)

CONF_MOVE_DIRECTION = "move_direction"


CONFIG_SCHEMA = {
    cv.GenerateID(CONF_LD2451_ID): cv.use_id(LD2451Component),
    cv.Optional(CONF_BAUD_RATE): select.select_schema(
        BaudRateSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon=ICON_THERMOMETER,
    ),
    cv.Optional(CONF_MOVE_DIRECTION): select.select_schema(
        MoveDirectionSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
        icon=ICON_RULER,
    ),
}

async def to_code(config):
    ld2451_component = await cg.get_variable(config[CONF_LD2451_ID])
    if baud_rate_config := config.get(CONF_BAUD_RATE):
        s = await select.new_select(
            baud_rate_config,
            options=[
                "9600",
                "19200",
                "38400",
                "57600",
                "115200",
                "230400",
                "256000",
                "460800",
            ],
        )
        await cg.register_parented(s, config[CONF_LD2451_ID])
        cg.add(ld2451_component.set_baud_rate_select(s))
    if move_direction_config := config.get(CONF_MOVE_DIRECTION):
        s = await select.new_select(
            move_direction_config, options=["away", "approach", "both"]
        )
        await cg.register_parented(s, config[CONF_LD2451_ID])
        cg.add(ld2451_component.set_move_direction_select(s))

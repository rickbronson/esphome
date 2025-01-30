import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID, CONF_THROTTLE, CONF_TIMEOUT, CONF_PASSWORD
from esphome import automation
from esphome.automation import maybe_simple_id

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@sebcaps", "@regevbr"]
MULTI_CONF = True

ld2451_ns = cg.esphome_ns.namespace("ld2451")
LD2451Component = ld2451_ns.class_("LD2451Component", cg.Component, uart.UARTDevice)

CONF_LD2451_ID = "ld2451_id"

CONF_MAX_MOVE_DISTANCE = "max_move_distance"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(LD2451Component),
        cv.Optional(CONF_THROTTLE, default="1000ms"): cv.All(
            cv.positive_time_period_milliseconds,
            cv.Range(min=cv.TimePeriod(milliseconds=1)),
        ),
    }
)

for i in range(9):
    CONFIG_SCHEMA = CONFIG_SCHEMA.extend(
        cv.Schema(
            {
            }
        )
    )

CONFIG_SCHEMA = cv.All(
    CONFIG_SCHEMA.extend(uart.UART_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "ld2451",
    require_tx=True,
    require_rx=True,
    parity="NONE",
    stop_bits=1,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    cg.add(var.set_throttle(config[CONF_THROTTLE]))


CALIBRATION_ACTION_SCHEMA = maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(LD2451Component),
    }
)


# Actions
BluetoothPasswordSetAction = ld2451_ns.class_(
    "BluetoothPasswordSetAction", automation.Action
)


BLUETOOTH_PASSWORD_SET_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(LD2451Component),
        cv.Required(CONF_PASSWORD): cv.templatable(cv.string_strict),
    }
)


@automation.register_action(
    "bluetooth_password.set", BluetoothPasswordSetAction, BLUETOOTH_PASSWORD_SET_SCHEMA
)
async def bluetooth_password_set_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_PASSWORD], args, cg.std_string)
    cg.add(var.set_password(template_))
    return var

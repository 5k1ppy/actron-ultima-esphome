from typing import Any

from esphome import pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PIN
from esphome.cpp_helpers import gpio_pin_expression

# AUTO_LOAD = ["text_sensor", "sensor", "binary_sensor"]

CONF_ACTRON_ULTIMA_ID = "actron_ultima_id"

actron_ultima_ns = cg.esphome_ns.namespace("actron_ultima")

ActronUltima = actron_ultima_ns.class_(
    "ActronUltima",
    cg.Component
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(ActronUltima),
        cv.Required(CONF_PIN): pins.gpio_input_pin_schema,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config: dict[str, Any]) -> None:
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)

    pin = await gpio_pin_expression(config[CONF_PIN])

    if pin is not None:
        cg.add(var.set_pin(pin))

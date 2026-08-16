import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import text_sensor

from . import ActronUltima


CONF_ACTRON_ULTIMA_ID = "actron_ultima_id"
CONF_BIT_STRING = "bit_string"


CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ACTRON_ULTIMA_ID):
            cv.use_id(ActronUltima),

        cv.Required(CONF_BIT_STRING):
            text_sensor.text_sensor_schema(),
    }
)


async def to_code(config):
    parent = await cg.get_variable(
        config[CONF_ACTRON_ULTIMA_ID]
    )

    sens = await text_sensor.new_text_sensor(
        config[CONF_BIT_STRING]
    )

    cg.add(
        parent.set_bit_string_sensor(sens)
    )

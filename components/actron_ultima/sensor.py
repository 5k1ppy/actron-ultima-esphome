import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import sensor

from . import ActronUltima


CONF_ACTRON_ULTIMA_ID = "actron_ultima_id"
CONF_SETPOINT_TEMP = "setpoint_temp"


CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ACTRON_ULTIMA_ID):
            cv.use_id(ActronUltima),

        cv.Optional(CONF_SETPOINT_TEMP):
            sensor.sensor_schema(
                unit_of_measurement="°C",
                accuracy_decimals=1,
                device_class="temperature",
                state_class="measurement",
            ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(
        config[CONF_ACTRON_ULTIMA_ID]
    )

    if CONF_SETPOINT_TEMP in config:
        sens = await sensor.new_sensor(
            config[CONF_SETPOINT_TEMP]
        )

        cg.add(
            parent.set_setpoint_sensor(sens)
        )

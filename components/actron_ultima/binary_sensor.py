import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import binary_sensor

from . import ActronUltima


CONF_ACTRON_ULTIMA_ID = "actron_ultima_id"

CONF_COOL = "cool"
CONF_AUTO = "auto_mode"
CONF_RUN = "run"
CONF_TIMER = "timer"

CONF_FAN_CONT = "fan_cont"
CONF_FAN_HIGH = "fan_high"
CONF_FAN_MID = "fan_mid"
CONF_FAN_LOW = "fan_low"

CONF_HEAT = "heat"
CONF_INSIDE = "inside"

CONF_ZONE_1 = "zone_1"
CONF_ZONE_2 = "zone_2"
CONF_ZONE_3 = "zone_3"
CONF_ZONE_4 = "zone_4"
CONF_ZONE_5 = "zone_5"
CONF_ZONE_6 = "zone_6"
CONF_ZONE_7 = "zone_7"
CONF_ZONE_8 = "zone_8"


BINARY_SENSORS = {
    CONF_COOL: "set_cool_sensor",
    CONF_AUTO: "set_auto_sensor",
    CONF_RUN: "set_run_sensor",
    CONF_TIMER: "set_timer_sensor",

    CONF_FAN_CONT: "set_fan_cont_sensor",
    CONF_FAN_HIGH: "set_fan_high_sensor",
    CONF_FAN_MID: "set_fan_mid_sensor",
    CONF_FAN_LOW: "set_fan_low_sensor",

    CONF_HEAT: "set_heat_sensor",
    CONF_INSIDE: "set_inside_sensor",

    CONF_ZONE_1: "set_zone1_sensor",
    CONF_ZONE_2: "set_zone2_sensor",
    CONF_ZONE_3: "set_zone3_sensor",
    CONF_ZONE_4: "set_zone4_sensor",
    CONF_ZONE_5: "set_zone5_sensor",
    CONF_ZONE_6: "set_zone6_sensor",
    CONF_ZONE_7: "set_zone7_sensor",
    CONF_ZONE_8: "set_zone8_sensor",
}


schema = {
    cv.Required(CONF_ACTRON_ULTIMA_ID):
        cv.use_id(ActronUltima),
}

for key in BINARY_SENSORS:
    schema[cv.Optional(key)] = (
        binary_sensor.binary_sensor_schema()
    )


CONFIG_SCHEMA = cv.Schema(schema)


async def to_code(config):
    parent = await cg.get_variable(
        config[CONF_ACTRON_ULTIMA_ID]
    )

    for key, setter in BINARY_SENSORS.items():

        if key not in config:
            continue

        sens = await binary_sensor.new_binary_sensor(
            config[key]
        )

        cg.add(
            getattr(parent, setter)(sens)
        )

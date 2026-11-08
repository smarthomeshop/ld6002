import esphome.codegen as cg
from esphome.components import binary_sensor, sensor, uart
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_MODEL,
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_OCCUPANCY,
    STATE_CLASS_MEASUREMENT,
    UNIT_BEATS_PER_MINUTE,
    UNIT_METER,
)

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["binary_sensor", "sensor"]
MULTI_CONF = True

ld6002_ns = cg.esphome_ns.namespace("ld6002")
LD6002Component = ld6002_ns.class_("LD6002Component", cg.Component, uart.UARTDevice)

BASE_SCHEMA = cv.Schema({cv.GenerateID(): cv.declare_id(LD6002Component)}).extend(
    cv.COMPONENT_SCHEMA
).extend(uart.UART_DEVICE_SCHEMA)

CONFIG_SCHEMA = cv.typed_schema(
    {
        "ld6002": BASE_SCHEMA.extend(
            {
                cv.Optional("breathing_rate"): sensor.sensor_schema(
                    unit_of_measurement="breaths/min",
                    accuracy_decimals=1,
                    state_class=STATE_CLASS_MEASUREMENT,
                ),
                cv.Optional("heart_rate"): sensor.sensor_schema(
                    unit_of_measurement=UNIT_BEATS_PER_MINUTE,
                    accuracy_decimals=1,
                    state_class=STATE_CLASS_MEASUREMENT,
                ),
            }
        ),
        "ld6002c": BASE_SCHEMA.extend(
            {
                cv.Optional("presence"): binary_sensor.binary_sensor_schema(
                    device_class=DEVICE_CLASS_OCCUPANCY,
                ),
                cv.Optional("fall_detected"): binary_sensor.binary_sensor_schema(),
                cv.Optional("target_depth"): sensor.sensor_schema(
                    unit_of_measurement=UNIT_METER,
                    accuracy_decimals=2,
                    device_class=DEVICE_CLASS_DISTANCE,
                    state_class=STATE_CLASS_MEASUREMENT,
                ),
            }
        ),
    },
    key=CONF_MODEL,
    lower=True,
)


def FINAL_VALIDATE_SCHEMA(config):
    # Hi-Link has published plain LD6002 manuals for both baud rates.
    baud_rate = 115200 if config[CONF_MODEL] == "ld6002c" else None
    return uart.final_validate_device_schema(
        "ld6002",
        baud_rate=baud_rate,
        require_rx=True,
        data_bits=8,
        parity="NONE",
        stop_bits=1,
    )(config)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_MODEL] == "ld6002c")
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    for key in ("breathing_rate", "heart_rate", "target_depth"):
        if key in config:
            entity = await sensor.new_sensor(config[key])
            cg.add(getattr(var, f"set_{key}_sensor")(entity))
    for key in ("presence", "fall_detected"):
        if key in config:
            entity = await binary_sensor.new_binary_sensor(config[key])
            cg.add(getattr(var, f"set_{key}_binary_sensor")(entity))

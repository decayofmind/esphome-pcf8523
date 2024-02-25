import esphome.config_validation as cv
import esphome.codegen as cg
from esphome import automation
from esphome.components import i2c, time
from esphome.const import CONF_ID

CODEOWNERS = ["@decayofmind"]

DEPENDENCIES = ["i2c"]


pcf8523_ns = cg.esphome_ns.namespace("pcf8523")
pcf8523Component = pcf8523_ns.class_(
    "PCF8523Component", time.RealTimeClock, i2c.I2CDevice
)
WriteAction = pcf8523_ns.class_("WriteAction", automation.Action)
ReadAction = pcf8523_ns.class_("ReadAction", automation.Action)


CONFIG_SCHEMA = time.TIME_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(pcf8523Component),
    }
).extend(i2c.i2c_device_schema(0xA3))


@automation.register_action(
    "pcf8523.write_time",
    WriteAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(pcf8523Component),
        }
    ),
)
async def pcf8523_write_time_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    "pcf8523.read_time",
    ReadAction,
    automation.maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(pcf8523Component),
        }
    ),
)
async def pcf8523_read_time_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    await time.register_time(var, config)

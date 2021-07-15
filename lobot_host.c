#include <stdint.h>

#include "lobot_host.h"
#include "lobot_types.h"

static bool _checksum(lobot_cmd_t *_cmd)
{
    if (_cmd->head.len < LEN_VOID_CMD)
    {
        return false;
    }

    chksm_t chksm = _cmd->head.id + _cmd->head.len + _cmd->head.cmd;
    for (uint8_t i = _cmd->head.len; i > LEN_VOID_CMD; i--)
    {
        chksm += ((uint8_t *)&(_cmd->head.id))[i - 1];
    }

    chksm_t *chksm_ptr = &((uint8_t *)&(_cmd->head.id))[_cmd->head.len];
    
    bool result = (*chksm_ptr == chksm);
    *chksm_ptr = chksm;
    return result;
}

static void _send_msg(lobot_cmd_t *const _cmd, SERVO_t _cmd_id, uint8_t _len, id_t _id)
{
    _cmd->head.preamble = PREAMBLE;
    _cmd->head.cmd = _cmd_id;
    _cmd->head.id  = _id;
    _cmd->head.len = _len;
    _checksum(_cmd);
    #warning bsp_send
}

static void _send_base_msg(SERVO_t _cmd_id, id_t _id)
{
    struct __attribute__((__packed__))
    {
        head_t head;
        chksm_t chksm_t;
    } cmd;
    _send_msg((lobot_cmd_t *)&cmd, _cmd_id, LEN_VOID_CMD, _id);
}

void lobot_host_move_time_write(id_t _id, angle_t _angle, time_t _time)
{
    lobot_cmd_t cmd =
    {
        .move_time.angle = _angle,
        .move_time.time  = _time,
    };
    _send_msg(&cmd, SERVO_MOVE_TIME_WRITE, SIZEOF_CMD(cmd.move_time), _id);
}

void lobot_host_move_time_read(id_t _id)
{
    _send_base_msg(SERVO_MOVE_TIME_READ, _id);
}

void lobot_host_move_time_wait_write(id_t _id, angle_t _angle, time_t _time)
{
    lobot_cmd_t cmd =
    {
        .move_time.angle = _angle,
        .move_time.time  = _time,
    };
    _send_msg(&cmd, SERVO_MOVE_TIME_WAIT_WRITE, SIZEOF_CMD(cmd.move_time), _id);
}

void lobot_host_move_time_wait_read(id_t _id)
{
    _send_base_msg(SERVO_MOVE_TIME_WAIT_READ, _id);
}

void lobot_host_move_start(id_t _id)
{
    _send_base_msg(SERVO_MOVE_START, _id);
}

void lobot_host_move_stop(id_t _id)
{
    _send_base_msg(SERVO_MOVE_STOP, _id);
}

void lobot_host_id_write(id_t _id, id_t _new)
{
    lobot_cmd_t cmd =
    {
        .id.new_id = _new,
    };
    _send_msg(&cmd, SERVO_ID_WRITE, SIZEOF_CMD(cmd.id), _id);
}

void lobot_host_id_read(void)
{
    _send_base_msg(SERVO_ID_READ, BROADCAST_ID);
}

void lobot_host_angle_offset_adjust(id_t _id, int8_t _adj)
{
    lobot_cmd_t cmd =
    {
        .angle_offset.offset = _adj,
    };
    _send_msg(&cmd, SERVO_ANGLE_OFFSET_ADJUST, SIZEOF_CMD(cmd.angle_offset), _id);
}

void lobot_host_angle_offset_write(id_t _id)
{
    _send_base_msg(SERVO_ANGLE_OFFSET_WRITE, _id);
}

void lobot_host_angle_offset_read(id_t _id)
{
    _send_base_msg(SERVO_ANGLE_OFFSET_READ, _id);
}

void lobot_host_angle_limit_write(id_t _id, angle_t _min, angle_t _max)
{
    lobot_cmd_t cmd =
    {
        .angle_limit.min = _min,
        .angle_limit.max = _max,
    };
    _send_msg(&cmd, SERVO_ANGLE_LIMIT_WRITE, SIZEOF_CMD(cmd.angle_limit), _id);
}

void lobot_host_angle_limit_read(id_t _id)
{
    _send_base_msg(SERVO_ANGLE_LIMIT_READ, _id);
}

void lobot_host_vin_limit_write(id_t _id, volt_t _min, volt_t _max)
{
    lobot_cmd_t cmd =
    {
        .vin_limit.min = _min,
        .vin_limit.max = _max,
    };
    _send_msg(&cmd, SERVO_VIN_LIMIT_WRITE, SIZEOF_CMD(cmd.vin_limit), _id);
}

void lobot_host_vin_limit_read(id_t _id)
{
    _send_base_msg(SERVO_VIN_LIMIT_READ, _id);
}

void lobot_host_temp_max_limit_write(id_t _id, temp_t _max)
{
    lobot_cmd_t cmd =
    {
        .temp.temp = _max,
    };
    _send_msg(&cmd, SERVO_TEMP_MAX_LIMIT_WRITE, SIZEOF_CMD(cmd.temp), _id);
}

void lobot_host_temp_max_limit_read(id_t _id)
{
    _send_base_msg(SERVO_TEMP_MAX_LIMIT_READ, _id);
}

void lobot_host_temp_read(id_t _id)
{
    _send_base_msg(SERVO_TEMP_READ, _id);
}

void lobot_host_vin_read(id_t _id)
{
    _send_base_msg(SERVO_VIN_READ, _id);
}

void lobot_host_pos_read(id_t _id)
{
    _send_base_msg(SERVO_POS_READ, _id);
}

void lobot_host_mode_write(id_t _id, flag_t _is_motor, speed_t _speed)
{
    lobot_cmd_t cmd =
    {
        .mode.mode = _is_motor ? MOTOR : SERVO,
        .mode.speed = _speed,
    };
    _send_msg(&cmd, SERVO_MODE_WRITE, SIZEOF_CMD(cmd.mode), _id);
}

void lobot_host_mode_read(id_t _id)
{
    _send_base_msg(SERVO_MODE_READ, _id);
}

void lobot_host_load_or_unload_write(id_t _id, flag_t _is_load)
{
    lobot_cmd_t cmd =
    {
        .load.state = _is_load ? LOAD : UNLOAD,
    };
    _send_msg(&cmd, SERVO_LOAD_OR_UNLOAD_WRITE, SIZEOF_CMD(cmd.load), _id);
}

void lobot_host_load_or_unload_read(id_t _id)
{
    _send_base_msg(SERVO_LOAD_OR_UNLOAD_READ, _id);
}

void lobot_host_led_ctrl_write(id_t _id, flag_t _led_on)
{
    lobot_cmd_t cmd =
    {
        .led_ctrl.state = _led_on ? ON : OFF,
    };
    _send_msg(&cmd, SERVO_LED_CTRL_WRITE, SIZEOF_CMD(cmd.load), _id);
}

void lobot_host_led_ctrl_read(id_t _id)
{
    _send_base_msg(SERVO_LED_CTRL_READ, _id);
}

void lobot_host_led_error_write(id_t _id, flag_t _over_temp, flag_t _over_volt, flag_t _over_load)
{
    lobot_cmd_t cmd =
    {
        .led_error =
        {
            .over_temp = _over_temp ? 1 : 0,
            .over_volt = _over_volt ? 1 : 0,
            .over_load = _over_load ? 1 : 0,
        }
    };
    _send_msg(&cmd, SERVO_LED_ERROR_WRITE, SIZEOF_CMD(cmd.led_error), _id);
}

void lobot_host_led_error_read(id_t _id)
{
    _send_base_msg(SERVO_LED_ERROR_READ, _id);
}

__attribute__((weak)) void lobot_host_move_time_read_callback(id_t _id, angle_t _angle, time_t _time)
{
}
__attribute__((weak)) void lobot_host_move_time_wait_read_callback(id_t _id, angle_t _angle, time_t _time)
{
}
__attribute__((weak)) void lobot_host_id_read_callback(id_t _id)
{
}
__attribute__((weak)) void lobot_host_angle_offset_read_callback(id_t _id, int8_t _adj)
{
}
__attribute__((weak)) void lobot_host_angle_limit_read_callback(id_t _id, angle_t _min, angle_t _max)
{
}
__attribute__((weak)) void lobot_host_vin_limit_read_callback(id_t _id, volt_t _min, volt_t _max)
{
}
__attribute__((weak)) void lobot_host_temp_max_limit_read_callback(id_t _id, temp_t _max)
{
}
__attribute__((weak)) void lobot_host_temp_read_callback(id_t _id, temp_t _temp)
{
}
__attribute__((weak)) void lobot_host_vin_read_callback(id_t _id, volt_t _volt)
{
}
__attribute__((weak)) void lobot_host_pos_read_callback(id_t _id, angle_t _angle)
{
}
__attribute__((weak)) void lobot_host_mode_read_callback(id_t _id, flag_t _is_motor, speed_t _speed)
{
}
__attribute__((weak)) void lobot_host_load_or_unload_read_callback(id_t _id, flag_t _is_load)
{
}
__attribute__((weak)) void lobot_host_led_ctrl_read_callback(id_t _id, flag_t _led_on)
{
}
__attribute__((weak)) void lobot_host_led_error_read_callback(id_t _id, flag_t _over_temp, flag_t _over_volt, flag_t _over_load)
{
}


bool bsp_lobot_host_callback(uint8_t *_data, uint8_t _len)
{
    head_t *const head = (head_t *)_data;
    lobot_cmd_t *const cmd = (lobot_cmd_t *)&((head_t *)_data)->id;
    if (false
        || head->preamble != PREAMBLE
        || _len < sizeof(head_t) + sizeof(chksm_t)
        || _len < head->len + sizeof(preamble_t) + sizeof(chksm_t)
        || !_checksum(cmd)
        ) return false;

    const id_t id = cmd->head.id;
    switch (cmd->head.cmd)
    {
        case SERVO_MOVE_TIME_READ     :
            if (cmd->head.len == SIZEOF_CMD(cmd->move_time))
                lobot_host_move_time_read_callback(id, cmd->move_time.angle, cmd->move_time.time);
            else return false;
        break;
        case SERVO_MOVE_TIME_WAIT_READ:
            if (cmd->head.len == SIZEOF_CMD(cmd->move_time))
                lobot_host_move_time_wait_read_callback(id, cmd->move_time.angle, cmd->move_time.time);
            else return false;
        break;
        case SERVO_ID_READ            :
            if (cmd->head.len == SIZEOF_CMD(cmd->id))
                lobot_host_id_read_callback(id);
            else return false;
        break;
        case SERVO_ANGLE_OFFSET_READ  :
            if (cmd->head.len == SIZEOF_CMD(cmd->angle_offset))
                lobot_host_angle_offset_read_callback(id, cmd->angle_offset.offset);
            else return false;
        break;
        case SERVO_ANGLE_LIMIT_READ   :
            if (cmd->head.len == SIZEOF_CMD(cmd->angle_limit))
                lobot_host_angle_limit_read_callback(id, cmd->angle_limit.min, cmd->angle_limit.max);
            else return false;
        break;
        case SERVO_VIN_LIMIT_READ     :
            if (cmd->head.len == SIZEOF_CMD(cmd->vin_limit))
                lobot_host_vin_limit_read_callback(id, cmd->vin_limit.min, cmd->vin_limit.max);
            else return false;
        break;
        case SERVO_TEMP_MAX_LIMIT_READ:
            if (cmd->head.len == SIZEOF_CMD(cmd->temp))
                lobot_host_temp_max_limit_read_callback(id, cmd->temp.temp);
            else return false;
        break;
        case SERVO_TEMP_READ          :
            if (cmd->head.len == SIZEOF_CMD(cmd->temp))
                lobot_host_temp_read_callback(id, cmd->temp.temp);
            else return false;
        break;
        case SERVO_VIN_READ           :
            if (cmd->head.len == SIZEOF_CMD(cmd->vin_limit))
                lobot_host_vin_read_callback(id, cmd->vin.voltage);
            else return false;
        break;
        case SERVO_POS_READ           :
            if (cmd->head.len == SIZEOF_CMD(cmd->pos_read))
                lobot_host_pos_read_callback(id, cmd->pos_read.position);
            else return false;
        break;
        case SERVO_MODE_READ          :
            if (cmd->head.len == SIZEOF_CMD(cmd->mode))
                lobot_host_mode_read_callback(id, cmd->mode.mode == MOTOR, cmd->mode.speed);
            else return false;
        break;
        case SERVO_LOAD_OR_UNLOAD_READ:
            if (cmd->head.len == SIZEOF_CMD(cmd->load))
                lobot_host_load_or_unload_read_callback(id, cmd->load.state == LOAD);
            else return false;
        break;
        case SERVO_LED_CTRL_READ      :
            if (cmd->head.len == SIZEOF_CMD(cmd->led_ctrl))
                lobot_host_led_ctrl_read_callback(id, cmd->led_ctrl.state == ON);
            else return false;
        break;
        case SERVO_LED_ERROR_READ     :
            if (cmd->head.len == SIZEOF_CMD(cmd->led_error))
                lobot_host_led_error_read_callback(id, cmd->led_error.over_temp, cmd->led_error.over_volt, cmd->led_error.over_load);
            else return false;
        break;
        default:
            return false;
    }
    return true;
}

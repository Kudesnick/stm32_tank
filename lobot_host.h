#pragma once

#include <stdint.h>

#include "lobot_types.h"

void lobot_host_move_time_write(id_t, angle_t, time_t);
void lobot_host_move_time_read(id_t);
void lobot_host_move_time_wait_write(id_t, angle_t, time_t);
void lobot_host_move_time_wait_read(id_t);
void lobot_host_move_start(id_t);
void lobot_host_move_stop(id_t);
void lobot_host_id_write(id_t, id_t _new);
void lobot_host_id_read(void);
void lobot_host_angle_offset_adjust(id_t, int8_t _adj);
void lobot_host_angle_offset_write(id_t);
void lobot_host_angle_offset_read(id_t);
void lobot_host_angle_limit_write(id_t, angle_t _min, angle_t _max);
void lobot_host_angle_limit_read(id_t);
void lobot_host_vin_limit_write(id_t, volt_t _min, volt_t _max);
void lobot_host_vin_limit_read(id_t);
void lobot_host_temp_max_limit_write(id_t, temp_t _max);
void lobot_host_temp_max_limit_read(id_t);
void lobot_host_temp_read(id_t);
void lobot_host_vin_read(id_t);
void lobot_host_pos_read(id_t);
void lobot_host_mode_write(id_t, flag_t _is_motor, speed_t);
void lobot_host_mode_read(id_t);
void lobot_host_load_or_unload_write(id_t, flag_t _is_load);
void lobot_host_load_or_unload_read(id_t);
void lobot_host_led_ctrl_write(id_t, flag_t _led_on);
void lobot_host_led_ctrl_read(id_t);
void lobot_host_led_error_write(id_t, flag_t _over_temp, flag_t _over_volt, flag_t _over_load);
void lobot_host_led_error_read(id_t);

void lobot_host_move_time_read_callback(id_t, angle_t, time_t);
void lobot_host_move_time_wait_read_callback(id_t, angle_t, time_t);
void lobot_host_id_read_callback(id_t);
void lobot_host_angle_offset_read_callback(id_t, int8_t _adj);
void lobot_host_angle_limit_read_callback(id_t, angle_t _min, angle_t _max);
void lobot_host_vin_limit_read_callback(id_t, volt_t _min, volt_t _max);
void lobot_host_temp_max_limit_read_callback(id_t, temp_t _max);
void lobot_host_temp_read_callback(id_t, temp_t);
void lobot_host_vin_read_callback(id_t, volt_t);
void lobot_host_pos_read_callback(id_t, angle_t);
void lobot_host_mode_read_callback(id_t, flag_t _is_motor, speed_t);
void lobot_host_load_or_unload_read_callback(id_t, flag_t _is_load);
void lobot_host_led_ctrl_read_callback(id_t, flag_t _led_on);
void lobot_host_led_error_read_callback(id_t, flag_t _over_temp, flag_t _over_volt, flag_t _over_load);

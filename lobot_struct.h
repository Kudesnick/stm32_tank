#include <stdint.h>

/// Commands identificators
typedef enum __attribute__((__packed__))
{
    SERVO_MOVE_TIME_WRITE       = 1 ,
    SERVO_MOVE_TIME_READ        = 2 ,
    SERVO_MOVE_TIME_WAIT_WRITE  = 7 ,
    SERVO_MOVE_TIME_WAIT_READ   = 8 ,
    SERVO_MOVE_START            = 11,
    SERVO_MOVE_STOP             = 12,
    SERVO_ID_WRITE              = 13,
    SERVO_ID_READ               = 14,
    SERVO_ANGLE_OFFSET_ADJUST   = 17,
    SERVO_ANGLE_OFFSET_WRITE    = 18,
    SERVO_ANGLE_OFFSET_READ     = 19,
    SERVO_ANGLE_LIMIT_WRITE     = 20,
    SERVO_ANGLE_LIMIT_READ      = 21,
    SERVO_VIN_LIMIT_WRITE       = 22,
    SERVO_VIN_LIMIT_READ        = 23,
    SERVO_TEMP_MAX_LIMIT_WRITE  = 24,
    SERVO_TEMP_MAX_LIMIT_READ   = 25,
    SERVO_TEMP_READ             = 26,
    SERVO_VIN_READ              = 27,
    SERVO_POS_READ              = 28,
    SERVO_OR_MOTOR_MODE_WRITE   = 29,
    SERVO_OR_MOTOR_MODE_READ    = 30,
    SERVO_LOAD_OR_UNLOAD_WRITE  = 31,
    SERVO_LOAD_OR_UNLOAD_READ   = 32,
    SERVO_LED_CTRL_WRITE        = 33,
    SERVO_LED_CTRL_READ         = 34,
    SERVO_LED_ERROR_WRITE       = 35,
    SERVO_LED_ERROR_READ        = 36,
} SERVO_t;

/// Common struct of header
typedef struct __attribute__((__packed__))
{
    uint16_t preamble; ///< Start of command detection. Must be 0x5555
    uint8_t  id;       ///< Servo personal ID. Rage from 0x00 ~ 0xFD. 0xFE - broadcast.
    uint8_t  len;      ///< Length of message without preamble and ID. 3 - is minimum value.
    SERVO_t  cmd;      ///< Comand ID
} head_t;

/**
 * @brief Preset to action or action command.
 * @details Valid do commands:
 * SERVO_MOVE_TIME_WRITE
 * SERVO_MOVE_TIME_READ (answer)
 * SERVO_MOVE_TIME_WAIT_WRITE
 * SERVO_MOVE_TIME_WAIT_READ (answer)
 */
typedef struct __attribute__((__packed__))
{
    head_t head;
    /**
     * @brief Angle value. Range 0~100
     * @details Corresponding to the servo angle of 0 ~ 240 °, that means the minimum angle of the
     * servo can bevaried is 0.24 degree
     */
    uint16_t angle;
    /**
     * @brief Time value. The range of time is 0~30000ms
     * @details When the command is sent to servo, the servo will be rotated from current angle to
     * parameter angle at uniform speed within parameter time. After the command reaches servo,
     * servo will rotate immediately.
     */
    uint16_t time;
    uint8_t chksm;
} SERVO_MOVE_TIME_t;

/**
 * @brief Servo set/get id
 * @details Valid do commands:
 * SERVO_ID_WRITE
 * SERVO_ID_READ (answer)
 */
typedef struct __attribute__((__packed__))
{
    head_t head;
    uint8_t new_id; ///< New ID. Rage from  0x00 ~ 0xFD
    uint8_t chksm;
} SERVO_ID_t;

/**
 * @brief Adjustment zero position
 * @details Valid do commands:
 * SERVO_ANGLE_OFFSET_ADJUST
 */
typedef struct __attribute__((__packed__))
{
    head_t head;
    /**
     * @brief servo deviation
     * @details range -125~ 125, The corresponding angle of -30 ° ~ 30 °, when
     * this command reach to the servo, the servo will immediately rotate to adjust the deviation.
     */
    int8_t offset;
    uint8_t chksm;
} SERVO_ANGLE_OFFSET_t;

typedef struct __attribute__((__packed__))
{
    head_t head;
    uint16_t min_angle;
    uint16_t max_angle;
    uint8_t chksm;
} SERVO_ANGLE_LIMIT_t;

typedef struct __attribute__((__packed__))
{
    head_t head;
    uint16_t min_voltage;
    uint16_t max_voltage;
    uint8_t chksm;
} SERVO_VIN_LIMIT_t;

typedef struct __attribute__((__packed__))
{
    head_t head;
    uint8_t temp;
    uint8_t chksm;
} SERVO_TEMP_t;

typedef struct __attribute__((__packed__))
{
    head_t head;
    uint16_t voltage;
    uint8_t chksm;
} SERVO_VIN_t;

typedef struct __attribute__((__packed__))
{
    head_t head;
    uint16_t position;
    uint8_t chksm;
} SERVO_POS_READ_t;

typedef struct __attribute__((__packed__))
{
    head_t head;
    enum __attribute__((__packed__))
    {
        SERVO,
        MOTOR,
    } mode;
    int16_t speed;
    uint8_t chksm;
} SERVO_OR_MOTOR_MODE_t;

typedef struct __attribute__((__packed__))
{
    head_t head;
    enum __attribute__((__packed__))
    {
        NO_LOAD,
        LOAD,
    } state;
    uint8_t chksm;
} SERVO_LOAD_t;

typedef SERVO_LOAD_t SERVO_LED_CTRL_t;

typedef struct __attribute__((__packed__))
{
    head_t head;
    struct __attribute__((__packed__))
    {
        uint8_t OVER_TEMP: 1;
        uint8_t OVER_VOLT: 1;
        uint8_t OVER_LOAD: 1;
    } state;
    uint8_t chksm;
} SERVO_LED_ERROR_t;

void servo_move_time_write(uint8_t _id, uint16_t _angle, uint16_t _time);
void servo_move_time_read(uint8_t _id);
void servo_move_time_wait_write(uint8_t _id, uint16_t _angle, uint16_t _time);
void servo_move_time_wait_read(uint8_t _id);
void servo_move_start(uint8_t _id);
void servo_move_stop(uint8_t _id);
void servo_id_write(uint8_t _id, uint8_t _new_id);
void servo_id_read(void);
void servo_angle_offset_adjust(uint8_t _id, int8_t _adj);
void servo_angle_offset_write(uint8_t _id);
void servo_angle_offset_read(uint8_t _id);
void servo_angle_limit_write(uint8_t _id, uint16_t _min_angle, uint16_t _max_angle);
void servo_angle_limit_read(uint8_t _id);
void servo_vin_limit_write(uint8_t _id, uint16_t _min_voltage, uint16_t _max_voltage);
void servo_vin_limit_read(uint8_t _id);
void servo_temp_max_limit_write(uint8_t _id, uint8_t _max_temp);
void servo_temp_max_limit_read(uint8_t _id);
void servo_temp_read(uint8_t _id);
void servo_vin_read(uint8_t _id);
void servo_pos_read(uint8_t _id);
void servo_or_motor_mode_write(uint8_t _id, bool _is_motor, int16_t _speed);
void servo_or_motor_mode_read(uint8_t _id);
void servo_load_or_unload_write(uint8_t _id, bool _is_load);
void servo_load_or_unload_read(uint8_t _id);
void servo_led_ctrl_write(uint8_t _id, bool _led_on);
void servo_led_ctrl_read(uint8_t _id);
void servo_led_error_write(uint8_t _id, bool _over_temp, bool _over_volt, bool _over_load);
void servo_led_error_read(uint8_t _id);

void servo_move_time_read_callnack(uint8_t _id, uint16_t _angle, uint16_t _time);
void servo_move_time_wait_read_callnack(uint8_t _id, uint16_t _angle, uint16_t _time);
void servo_id_read_callnack(uint8_t _id);
void servo_angle_offset_read_callnack(uint8_t _id, int8_t _adj);
void servo_angle_limit_read_callnack(uint8_t _id, uint16_t _min_angle, uint16_t _max_angle);
void servo_vin_limit_read_callnack(uint8_t _id, uint16_t _min_voltage, uint16_t _max_voltage);
void servo_temp_max_limit_read_callnack(uint8_t _id, uint8_t _max_temp);
void servo_temp_read_callnack(uint8_t _id, uint8_t _temp);
void servo_vin_read_callnack(uint8_t _id, uint16_t _voltage);
void servo_pos_read_callnack(uint8_t _id, uint16_t _angle);
void servo_or_motor_mode_read_callnack(uint8_t _id, bool _is_motor, int16_t _speed);
void servo_load_or_unload_read_callnack(uint8_t _id, bool _is_load);
void servo_led_ctrl_read_callnack(uint8_t _id, bool _led_on);
void servo_led_error_read_callnack(uint8_t _id, bool _over_temp, bool _over_volt, bool _over_load);

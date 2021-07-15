#pragma once

#include <stdint.h>
#include <stdbool.h>

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
    SERVO_MODE_WRITE            = 29,
    SERVO_MODE_READ             = 30,
    SERVO_LOAD_OR_UNLOAD_WRITE  = 31,
    SERVO_LOAD_OR_UNLOAD_READ   = 32,
    SERVO_LED_CTRL_WRITE        = 33,
    SERVO_LED_CTRL_READ         = 34,
    SERVO_LED_ERROR_WRITE       = 35,
    SERVO_LED_ERROR_READ        = 36,

    SERVO_CNT,
} SERVO_t;

typedef uint16_t preamble_t;
typedef uint8_t id_t;
typedef uint16_t angle_t;
typedef uint16_t time_t;
typedef uint16_t volt_t;
typedef uint8_t temp_t;
typedef int16_t speed_t;
typedef bool flag_t;
typedef uint8_t chksm_t;

/// Common struct of header
typedef struct __attribute__((__packed__))
{
    preamble_t preamble; ///< Start of command detection. Must be 0x5555
    id_t  id;       ///< Servo personal ID. Rage from 0x00 ~ 0xFD. 0xFE - broadcast.
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
    /**
     * @brief Angle value. Range 0~100
     * @details Corresponding to the servo angle of 0 ~ 240 °, that means the minimum angle of the
     * servo can bevaried is 0.24 degree
     */
    angle_t angle;
    /**
     * @brief Time value. The range of time is 0~30000ms
     * @details When the command is sent to servo, the servo will be rotated from current angle to
     * parameter angle at uniform speed within parameter time. After the command reaches servo,
     * servo will rotate immediately.
     */
    time_t time;
} SERVO_MOVE_TIME_t;

/**
 * @brief Servo set/get id
 * @details Valid do commands:
 * SERVO_ID_WRITE
 * SERVO_ID_READ (answer)
 */
typedef struct __attribute__((__packed__))
{
    id_t new_id; ///< New ID. Rage from  0x00 ~ 0xFD
} SERVO_ID_t;

/**
 * @brief Adjustment zero position
 * @details Valid do commands:
 * SERVO_ANGLE_OFFSET_ADJUST
 */
typedef struct __attribute__((__packed__))
{
    /**
     * @brief servo deviation
     * @details range -125~ 125, The corresponding angle of -30 ° ~ 30 °, when
     * this command reach to the servo, the servo will immediately rotate to adjust the deviation.
     */
    int8_t offset;
} SERVO_ANGLE_OFFSET_t;

typedef struct __attribute__((__packed__))
{
    angle_t min;
    angle_t max;
} SERVO_ANGLE_LIMIT_t;

typedef struct __attribute__((__packed__))
{
    volt_t min;
    volt_t max;
} SERVO_VIN_LIMIT_t;

typedef struct __attribute__((__packed__))
{
    temp_t temp;
} SERVO_TEMP_t;

typedef struct __attribute__((__packed__))
{
    volt_t voltage;
} SERVO_VIN_t;

typedef struct __attribute__((__packed__))
{
    angle_t position;
} SERVO_POS_READ_t;

typedef struct __attribute__((__packed__))
{
    enum __attribute__((__packed__))
    {
        SERVO,
        MOTOR,
    } mode;
    speed_t speed;
} SERVO_MODE_t;

typedef struct __attribute__((__packed__))
{
    enum __attribute__((__packed__))
    {
        UNLOAD,
        LOAD,
    } state;
} SERVO_LOAD_t;

typedef struct __attribute__((__packed__))
{
    enum __attribute__((__packed__))
    {
        OFF,
        ON,
    } state;
} SERVO_LED_CTRL_t;

typedef struct __attribute__((__packed__))
{
    uint8_t over_temp: 1;
    uint8_t over_volt: 1;
    uint8_t over_load: 1;
} SERVO_LED_ERROR_t;

typedef struct __attribute__((__packed__))
{
    head_t head;
    union __attribute__((__packed__))
    {
        SERVO_MOVE_TIME_t     move_time    ;
        SERVO_ID_t            id           ;
        SERVO_ANGLE_OFFSET_t  angle_offset ;
        SERVO_ANGLE_LIMIT_t   angle_limit  ;
        SERVO_VIN_LIMIT_t     vin_limit    ;
        SERVO_TEMP_t          temp         ;
        SERVO_VIN_t           vin          ;
        SERVO_POS_READ_t      pos_read     ;
        SERVO_MODE_t          mode         ;
        SERVO_LOAD_t          load         ;
        SERVO_LED_CTRL_t      led_ctrl     ;
        SERVO_LED_ERROR_t     led_error    ;

        chksm_t               chksm        ;
    };
} lobot_cmd_t;

#define PREAMBLE            ((preamble_t)0x5555)
#define BROADCAST_ID        ((id_t)(0xFE))
#define LEN_VOID_CMD        (sizeof(head_t) - sizeof(preamble_t))
#define SIZEOF_CMD(_LEN)    (sizeof(_LEN) + LEN_VOID_CMD)

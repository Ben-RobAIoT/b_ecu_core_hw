/* Author: Beniot Phan (Phan Bao Tin)
 * Project: B-ECU Core - Automotive Sensor ECU & Embedded Linux Gateway
 * File: ecu_frame_defs.h
 * Description: Zero-padding binary protocol definitions, compile-time layout
 *              assertions, endianness-safe bit-shift serializers, and CRC-16/CCITT-FALSE.
 */

#ifndef ECU_FRAME_DEFS_H
#define ECU_FRAME_DEFS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * 1. Compile-Time Assertion Macro (C11 & C++11/17 Compatible)
 * ============================================================================ */
#if defined(__cplusplus)
    #define ECU_STATIC_ASSERT(cond, msg) static_assert((cond), msg)
#else
    #define ECU_STATIC_ASSERT(cond, msg) _Static_assert((cond), msg)
#endif

/* ============================================================================
 * 2. Protocol Constants, Message Types, FSM States & Fault Bitmasks
 * ============================================================================ */
#define ECU_FRAME_SOF               ((uint16_t)0xAA55U)
#define ECU_FRAME_EOF               ((uint16_t)0x55AAU)
#define ECU_PROTOCOL_VERSION        ((uint8_t)0x01U)

#define ECU_HEADER_WIRE_SIZE        (8U)
#define ECU_TELEMETRY_PAYLOAD_SIZE  (28U)
#define ECU_TRAILER_WIRE_SIZE       (4U)  /* 2B CRC-16 + 2B EOF */
#define ECU_TELEMETRY_FRAME_SIZE    (ECU_HEADER_WIRE_SIZE + ECU_TELEMETRY_PAYLOAD_SIZE + ECU_TRAILER_WIRE_SIZE)

/* Header Flags */
#define ECU_HDR_FLAG_ACK_REQ        ((uint8_t)(1U << 0U))
#define ECU_HDR_FLAG_HIGH_PRIO      ((uint8_t)(1U << 1U))

/* Message Types */
typedef enum {
    ECU_MSG_TELEMETRY = 0x01U,
    ECU_MSG_COMMAND   = 0x02U,
    ECU_MSG_HEARTBEAT = 0x03U,
    ECU_MSG_DTC_FAULT = 0x04U
} ecu_msg_type_t;

/* System FSM States */
typedef enum {
    ECU_STATE_BOOT_INIT         = 0x00U,
    ECU_STATE_SELF_TEST         = 0x01U,
    ECU_STATE_NORMAL_RUN        = 0x02U,
    ECU_STATE_DEGRADED_RECOVERY = 0x03U,
    ECU_STATE_LOW_POWER         = 0x04U,
    ECU_STATE_SAFE_STOP         = 0x05U
} ecu_state_t;

/* Diagnostic Trouble Code (DTC) Bitmask Flags */
#define ECU_FAULT_NONE              ((uint8_t)0x00U)
#define ECU_FAULT_I2C_BUS_HANG      ((uint8_t)(1U << 0U))
#define ECU_FAULT_IMU_NACK          ((uint8_t)(1U << 1U))
#define ECU_FAULT_BME_NACK          ((uint8_t)(1U << 2U))
#define ECU_FAULT_UART_CRC_ERR      ((uint8_t)(1U << 3U))
#define ECU_FAULT_ESTOP_ACTIVE      ((uint8_t)(1U << 4U))
#define ECU_FAULT_WDG_RESET_CAUSE   ((uint8_t)(1U << 5U))
#define ECU_FAULT_RINGBUF_OVERFLOW  ((uint8_t)(1U << 6U))

/* Serializer / Deserializer Return Status */
typedef enum {
    ECU_OK              = 0,
    ECU_ERR_NULL_PTR    = -1,
    ECU_ERR_BUF_SMALL   = -2,
    ECU_ERR_BAD_SOF     = -3,
    ECU_ERR_BAD_EOF     = -4,
    ECU_ERR_BAD_VERSION = -5,
    ECU_ERR_BAD_LEN     = -6,
    ECU_ERR_BAD_CRC     = -7
} ecu_status_t;

/* ============================================================================
 * 3. Naturally Aligned Zero-Padding In-Memory Structs (NO #pragma pack(1))
 * ============================================================================ */

/* Size: 8 bytes | Alignment: 2 bytes | Padding: 0 bytes */
typedef struct {
    uint16_t sof;         /* Offset 0..1 */
    uint8_t  version;     /* Offset 2    */
    uint8_t  msg_type;    /* Offset 3    */
    uint16_t seq_num;     /* Offset 4..5 */
    uint8_t  payload_len; /* Offset 6    */
    uint8_t  hdr_flags;   /* Offset 7    */
} ecu_frame_header_t;

/* Size: 28 bytes | Alignment: 4 bytes | Padding: 0 bytes */
typedef struct {
    uint32_t timestamp_ms;       /* Offset  0..3  */
    int32_t  pressure_pa;        /* Offset  4..7  */
    int16_t  accel_x_mg;         /* Offset  8..9  */
    int16_t  accel_y_mg;         /* Offset 10..11 */
    int16_t  accel_z_mg;         /* Offset 12..13 */
    int16_t  gyro_z_ddeg_s;      /* Offset 14..15 */
    int16_t  temp_c_x100;        /* Offset 16..17 */
    uint16_t hum_rh_x100;        /* Offset 18..19 */
    uint16_t adc_throttle_raw;   /* Offset 20..21 */
    uint16_t adc_light_raw;      /* Offset 22..23 */
    uint16_t pwm_duty_permille;  /* Offset 24..25 */
    uint8_t  ecu_state;          /* Offset 26     */
    uint8_t  fault_flags;        /* Offset 27     */
} ecu_telemetry_payload_t;

/* Size: 40 bytes | Alignment: 4 bytes | Padding: 0 bytes */
typedef struct {
    ecu_frame_header_t      header;  /* Offset  0..7  */
    ecu_telemetry_payload_t payload; /* Offset  8..35 */
    uint16_t                crc16;   /* Offset 36..37 */
    uint16_t                eof;     /* Offset 38..39 */
} ecu_telemetry_frame_t;

/* Compile-Time Zero-Padding Verification */
ECU_STATIC_ASSERT(sizeof(ecu_frame_header_t) == 8U,
                  "ecu_frame_header_t must be 8 bytes with zero padding");
ECU_STATIC_ASSERT(sizeof(ecu_telemetry_payload_t) == 28U,
                  "ecu_telemetry_payload_t must be 28 bytes with zero padding");
ECU_STATIC_ASSERT(sizeof(ecu_telemetry_frame_t) == 40U,
                  "ecu_telemetry_frame_t must be 40 bytes with zero padding");
ECU_STATIC_ASSERT(offsetof(ecu_telemetry_frame_t, payload) == 8U,
                  "Unexpected padding before payload");
ECU_STATIC_ASSERT(offsetof(ecu_telemetry_frame_t, crc16) == 36U,
                  "Unexpected padding before crc16");
ECU_STATIC_ASSERT(offsetof(ecu_telemetry_frame_t, eof) == 38U,
                  "Unexpected padding before eof");

/* ============================================================================
 * 4. CRC-16/CCITT-FALSE (Poly: 0x1021, Init: 0xFFFF, XorOut: 0x0000)
 *    Unit Test Vector: ecu_crc16_ccitt((const uint8_t *)"123456789", 9U) == 0x29B1U
 * ============================================================================ */
static inline uint16_t ecu_crc16_ccitt(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFFU;
    if (data == NULL) {
        return 0U;
    }

    for (size_t i = 0U; i < len; ++i) {
        crc ^= (uint16_t)((uint16_t)data[i] << 8U);
        for (uint8_t bit = 0U; bit < 8U; ++bit) {
            if ((crc & 0x8000U) != 0U) {
                crc = (uint16_t)((uint16_t)(crc << 1U) ^ 0x1021U);
            } else {
                crc = (uint16_t)(crc << 1U);
            }
        }
    }
    return crc;
}

/* ============================================================================
 * 5. Endianness-Safe Big-Endian (Network Byte Order) Helpers
 *    Never cast raw byte buffers directly to C structs.
 * ============================================================================ */
static inline void ecu_put_be16(uint8_t *dst, uint16_t val)
{
    dst[0] = (uint8_t)((val >> 8U) & 0xFFU);
    dst[1] = (uint8_t)(val & 0xFFU);
}

static inline void ecu_put_be32(uint8_t *dst, uint32_t val)
{
    dst[0] = (uint8_t)((val >> 24U) & 0xFFU);
    dst[1] = (uint8_t)((val >> 16U) & 0xFFU);
    dst[2] = (uint8_t)((val >> 8U)  & 0xFFU);
    dst[3] = (uint8_t)(val & 0xFFU);
}

static inline uint16_t ecu_get_be16(const uint8_t *src)
{
    return (uint16_t)(((uint16_t)src[0] << 8U) | (uint16_t)src[1]);
}

static inline uint32_t ecu_get_be32(const uint8_t *src)
{
    return ((uint32_t)src[0] << 24U) |
           ((uint32_t)src[1] << 16U) |
           ((uint32_t)src[2] << 8U)  |
           ((uint32_t)src[3]);
}

/* ============================================================================
 * 6. Caller-Allocated Frame Serializer & Deserializer (Zero Heap)
 * ============================================================================ */
static inline ecu_status_t ecu_serialize_telemetry(
    const ecu_telemetry_payload_t *payload,
    uint16_t seq_num,
    uint8_t hdr_flags,
    uint8_t *out_buf,
    size_t out_buf_size,
    size_t *out_written)
{
    if ((payload == NULL) || (out_buf == NULL)) {
        return ECU_ERR_NULL_PTR;
    }
    if (out_buf_size < ECU_TELEMETRY_FRAME_SIZE) {
        return ECU_ERR_BUF_SMALL;
    }

    /* Header (Offset 0..7) */
    ecu_put_be16(&out_buf[0], ECU_FRAME_SOF);
    out_buf[2] = ECU_PROTOCOL_VERSION;
    out_buf[3] = (uint8_t)ECU_MSG_TELEMETRY;
    ecu_put_be16(&out_buf[4], seq_num);
    out_buf[6] = (uint8_t)ECU_TELEMETRY_PAYLOAD_SIZE;
    out_buf[7] = hdr_flags;

    /* Payload (Offset 8..35) - cast signed fields to unsigned before shifting to avoid UB */
    ecu_put_be32(&out_buf[8],  payload->timestamp_ms);
    ecu_put_be32(&out_buf[12], (uint32_t)payload->pressure_pa);
    ecu_put_be16(&out_buf[16], (uint16_t)payload->accel_x_mg);
    ecu_put_be16(&out_buf[18], (uint16_t)payload->accel_y_mg);
    ecu_put_be16(&out_buf[20], (uint16_t)payload->accel_z_mg);
    ecu_put_be16(&out_buf[22], (uint16_t)payload->gyro_z_ddeg_s);
    ecu_put_be16(&out_buf[24], (uint16_t)payload->temp_c_x100);
    ecu_put_be16(&out_buf[26], payload->hum_rh_x100);
    ecu_put_be16(&out_buf[28], payload->adc_throttle_raw);
    ecu_put_be16(&out_buf[30], payload->adc_light_raw);
    ecu_put_be16(&out_buf[32], payload->pwm_duty_permille);
    out_buf[34] = payload->ecu_state;
    out_buf[35] = payload->fault_flags;

    /* Trailer: CRC-16 over bytes 0..35, then EOF (Offset 36..39) */
    const uint16_t crc = ecu_crc16_ccitt(out_buf, ECU_HEADER_WIRE_SIZE + ECU_TELEMETRY_PAYLOAD_SIZE);
    ecu_put_be16(&out_buf[36], crc);
    ecu_put_be16(&out_buf[38], ECU_FRAME_EOF);

    if (out_written != NULL) {
        *out_written = ECU_TELEMETRY_FRAME_SIZE;
    }
    return ECU_OK;
}

static inline ecu_status_t ecu_deserialize_telemetry(
    const uint8_t *in_buf,
    size_t in_len,
    ecu_telemetry_frame_t *out_frame)
{
    if ((in_buf == NULL) || (out_frame == NULL)) {
        return ECU_ERR_NULL_PTR;
    }
    if (in_len < ECU_TELEMETRY_FRAME_SIZE) {
        return ECU_ERR_BUF_SMALL;
    }

    const uint16_t sof = ecu_get_be16(&in_buf[0]);
    if (sof != ECU_FRAME_SOF) {
        return ECU_ERR_BAD_SOF;
    }
    if (in_buf[2] != ECU_PROTOCOL_VERSION) {
        return ECU_ERR_BAD_VERSION;
    }
    if (in_buf[6] != (uint8_t)ECU_TELEMETRY_PAYLOAD_SIZE) {
        return ECU_ERR_BAD_LEN;
    }

    const uint16_t eof = ecu_get_be16(&in_buf[38]);
    if (eof != ECU_FRAME_EOF) {
        return ECU_ERR_BAD_EOF;
    }

    const uint16_t rx_crc   = ecu_get_be16(&in_buf[36]);
    const uint16_t calc_crc = ecu_crc16_ccitt(in_buf, ECU_HEADER_WIRE_SIZE + ECU_TELEMETRY_PAYLOAD_SIZE);
    if (rx_crc != calc_crc) {
        return ECU_ERR_BAD_CRC;
    }

    /* Populate validated frame */
    out_frame->header.sof         = sof;
    out_frame->header.version     = in_buf[2];
    out_frame->header.msg_type    = in_buf[3];
    out_frame->header.seq_num     = ecu_get_be16(&in_buf[4]);
    out_frame->header.payload_len = in_buf[6];
    out_frame->header.hdr_flags   = in_buf[7];

    out_frame->payload.timestamp_ms      = ecu_get_be32(&in_buf[8]);
    out_frame->payload.pressure_pa       = (int32_t)ecu_get_be32(&in_buf[12]);
    out_frame->payload.accel_x_mg        = (int16_t)ecu_get_be16(&in_buf[16]);
    out_frame->payload.accel_y_mg        = (int16_t)ecu_get_be16(&in_buf[18]);
    out_frame->payload.accel_z_mg        = (int16_t)ecu_get_be16(&in_buf[20]);
    out_frame->payload.gyro_z_ddeg_s     = (int16_t)ecu_get_be16(&in_buf[22]);
    out_frame->payload.temp_c_x100       = (int16_t)ecu_get_be16(&in_buf[24]);
    out_frame->payload.hum_rh_x100       = ecu_get_be16(&in_buf[26]);
    out_frame->payload.adc_throttle_raw  = ecu_get_be16(&in_buf[28]);
    out_frame->payload.adc_light_raw     = ecu_get_be16(&in_buf[30]);
    out_frame->payload.pwm_duty_permille = ecu_get_be16(&in_buf[32]);
    out_frame->payload.ecu_state         = in_buf[34];
    out_frame->payload.fault_flags       = in_buf[35];

    out_frame->crc16 = rx_crc;
    out_frame->eof   = eof;

    return ECU_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* ECU_FRAME_DEFS_H */
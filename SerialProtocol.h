/*
 * SerialProtocol.h
 *
 *  Created on: Jul 15, 2014
 *      Author: jonasbachli
 */

#ifndef SERIALPROTOCOL_H_
#define SERIALPROTOCOL_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RL_CMD_HEADER			0x3a
#define RL_RESPONSE_HEADER		0x2b

#define RL_RESPONSE_OK			0x00
#define RL_RESPONSE_ERROR		0xff

typedef enum {
    RL_CMD_PING = 0x00,
    RL_CMD_INFO = 0x01,
    RL_CMD_SET_IDLE = 0x11,
    RL_CMD_SET_READY = 0x12,
    RL_CMD_SET_REC = 0x13,
    RL_CMD_SET_CUE = 0x14,
    RL_CMD_GET_STATE = 0x21,
	RL_CMD_SET_IDLE_COL = 0x41,
	RL_CMD_SET_READY_COL = 0x42,
	RL_CMD_SET_REC_COL = 0x43,
	RL_CMD_SET_CUE_COL = 0x44,
	RL_CMD_GET_IDLE_COL = 0x51,
	RL_CMD_GET_READY_COL = 0x52,
	RL_CMD_GET_REC_COL = 0x53,
	RL_CMD_GET_CUE_COL = 0x54,
    RL_CMD_RESET = 0xff,
} RL_CMD;

typedef enum {
    RLState_idle = 0x00,
    RLState_ready = 0x01,
    RLState_rec = 0x02,
    RLState_cue = 0x03,
} RLState;

typedef struct __attribute__((packed)) _RLMsg{
    uint8_t cmd;
    uint32_t arg;
} RLMsg;

#ifdef __cplusplus
}
#endif

#endif /* SERIALPROTOCOL_H_ */

#ifndef CDH_INCLUDE_CDH_EPS_PROTOCOL_ID_H_
#define CDH_INCLUDE_CDH_EPS_PROTOCOL_ID_H_

#include "obc_assert.h"
#include <stdint.h>

/* Command IDs */
#define CMD_SUBSYS_SHUTDDOWN 0x00
#define CMD_HEARTBEAT 0x01
#define CMD_GET_TELEMETRY 0x02

/* Telemetry IDs */
#define TLE_EPS_BOARD_TEMP 0x03
#define TLE_SOLAR_PANEL_1_TEMP 0x04
#define TLE_SOLAR_PANEL_2_TEMP 0x05
#define TLE_SOLAR_PANEL_3_TEMP 0x06
#define TLE_SOLAR_PANEL_4_TEMP 0x07
#define TLE_COMMS_5V_CURRENT 0x08
#define TLE_COMMS_3V3_CURRENT 0x09
#define TLE_MAG_8V_CURRENT 0x0A
#define TLE_ADCS_5V_CURRENT 0x0B
#define TLE_ADCS_3V3_CURRENT 0x0C
#define TLE_OBC_CURRENT 0x0D
#define TLE_COMMS_5V_VOLTAGE 0x0E
#define TLE_COMMS_3V3_VOLTAGE 0x0F
#define TLE_MAG_8V_VOLTAGE 0x10
#define TLE_ADCS_5V_VOLTAGE 0x11
#define TLE_ADCS_3V3_VOLTAGE 0x12
#define TLE_OBC_3V3_VOLTAGE 0x13

/* Command Response IDs */
#define RESP_SUBSYS_SHUTDDOWN_ACK 0x14
#define RESP_HEARTBEAT_ACK 0x15

#endif /* CDH_INCLUDE_CDH_EPS_PROTOCOL_ID_H_ */
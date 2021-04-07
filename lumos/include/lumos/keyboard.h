#ifndef KEYBOARD_H
#define KEYBOARD_H

// KEYBOARD ports
#define KBD_ENC_COMMAND_PORT 0x60
#define KBD_ENC_DATA_PORT 0x60
#define KBD_CTL_COMMAND_PORT 0x64
#define KBD_CTL_STATUS_REGISTER 0x64
#define KBD_CTL_DATA_PORT 0x60

//  Status Masks
#define OP_BUF_STATUS 0x01
#define IP_BUF_STATUS 0x02
#define SYS_FLAG 0x04
#define LAST_COMMAND 0x08
#define KBD_LOCK 0x10
#define AUX_OP_BUF_STATUS 0x20
#define TIMEOUT 0x40
#define PARITY_ERROR 0x80

// keys_status masks
#define CTRL 0x01
#define SHIFT 0x02
#define ALT 0x04
#define CAPS 0x08
#define NUM_LOCK 0x10

// Commands & return codes
#define KBD_ENC_SELF_TEST 0xFF
#define KBD_ENC_SELF_TEST_SUCCESS 0xAA
#define KBD_ACK 0xFA
#define KBD_CTL_SELF_TEST 0xAA
#define KBD_CTL_SELF_TEST_SUCCESS 0x55
#define KBD_ENC_RESEND 0xFE

void init_keyboard();

#endif
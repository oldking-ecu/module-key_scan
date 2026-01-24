/**
 * @file key_scan_user.h
 * @author wwyyy (1046685883@qq.com)
 * @brief
 * @version 3.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2015-2026 oldking-ecu, All rights reserved
 *
 */
#ifndef KEY_SCAN_USER_H__
#define KEY_SCAN_USER_H__
#include "key_scan.h"

/**
 * @brief
 *
 */
typedef enum {
	KEY_SCAN_INS_KEY1 = 0,
	KEY_SCAN_INS_KEY2,
	KEY_SCAN_INS_KEY3,
	KEY_SCAN_INS_KEY4,
	KEY_SCAN_INS_KEY5,
	KEY_SCAN_INS_TOTAL_NUM
} KEY_SCAN_INSS_ENUM_TYPE;

/**
 * @brief
 *
 */
void KeyScan_UserInit(void);

#endif    // KEY_SCAN_USER_H__

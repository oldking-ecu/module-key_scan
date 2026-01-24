/**
 * @file key_scan.h
 * @author wwyyy (1046685883@qq.com)
 * @brief
 * @version 3.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2015-2026 oldking-ecu, All rights reserved
 *
 */
#ifndef KEY_SCAN_H__
#define KEY_SCAN_H__
#include "type.h"

/**
 * @brief
 *
 */
typedef enum {
	KEYMSG_SHORT = 0,         // 短按按下 单次
	KEYMSG_SHORT_RELEASE,     // 短按释放 单次
	KEYMSG_LONG_ONCE,         // 首次长按 单次
	KEYMSG_LONGING,           // 持续长按 周期
	KEYMSG_LONG_LONG_ONCE,    // 长长按 单次
	KEYMSG_LONG_RELEASE,      // 长按释放 单次
} KeyScan_MsgTypeEnum;

/**
 * @brief
 *
 */
typedef struct {
	uint16 JitterMs;             // 消抖延时
	uint16 LongMs;               // 首次长按延时
	uint16 LongPeriodMs;         // 长按持续msg周期
	uint16 LongAccelMs;          // 长按加速延时
	uint16 LongAccelPeriodMs;    // 长按加速后持续msg周期
	uint16 LongLongOnceMs;       // 超长按延时
} KeyScan_InsCfgType;

/**
 * @brief
 *
 */
typedef struct {
	uint8 Status;
	uint32 Timstamp;
	uint32 Timstamp1;
} KeyScan_InsInfType;

/**
 * @brief
 *
 */
typedef struct {
	const KeyScan_InsCfgType *InsCfgPtr;
	KeyScan_InsInfType *InsInfPtr;
	uint8 InsNum;
	void (*KeyBspInit_FuncPtr)(void);                                  // 可选
	uint8 (*IsPress_FuncPtr)(uint8 ins);                               // 必须
	uint32 (*GetSysTick_FuncPtr)(void);                                // 必须
	void (*MsgCbk_FuncPtr)(uint8 ins, KeyScan_MsgTypeEnum msgType);    // 必须
} KeyScan_CfgType;

void KeyScan_Init(KeyScan_CfgType *CfgPtr);
void KeyScan_Main(void);


#endif    // KEY_SCAN_H__

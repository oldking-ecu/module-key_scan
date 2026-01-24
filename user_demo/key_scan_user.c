/**
 * @file key_scan_user.c
 * @author wwyyy (1046685883@qq.com)
 * @brief
 * @version 3.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2015-2026 oldking-ecu, All rights reserved
 *
 */
#include "key_scan_user.h"
#include "mcu_hal_user.h"
#include "task.h"
#include "timer.h"
#include "public.h"
#include "sc8815_user.h"
#include "param.h"

#define VBUS_MAX 38189
#define VBUS_MIN 37

#define IBUS_MAX 12000
#define IBUS_MIN 46

#define KeySetNum1 Param_Info.BusVolt
#define KeySetNum2 Param_Info.BusCurrent
#define KeySetSwitch Param_Info.CurSetIdx

/**
 * @brief
 *
 */
static const KeyScan_InsCfgType KeyScan_InsCfg[KEY_SCAN_INS_TOTAL_NUM] = {
	{.JitterMs = 10, .LongMs = 500, .LongPeriodMs = 100, .LongAccelMs = 2000, .LongAccelPeriodMs = 5, .LongLongOnceMs = 5000},
	{.JitterMs = 10, .LongMs = 500, .LongPeriodMs = 100, .LongAccelMs = 2000, .LongAccelPeriodMs = 5, .LongLongOnceMs = 5000},
	{.JitterMs = 10, .LongMs = 500, .LongPeriodMs = 100, .LongAccelMs = 2000, .LongAccelPeriodMs = 5, .LongLongOnceMs = 5000},
	{.JitterMs = 10, .LongMs = 500, .LongPeriodMs = 100, .LongAccelMs = 2000, .LongAccelPeriodMs = 5, .LongLongOnceMs = 5000},
	{.JitterMs = 10, .LongMs = 500, .LongPeriodMs = 100, .LongAccelMs = 2000, .LongAccelPeriodMs = 5, .LongLongOnceMs = 5000},
};
static KeyScan_InsInfType KeyScan_InsInf[KEY_SCAN_INS_TOTAL_NUM];


static uint8 Key_GetStatus(uint8 ins)
{
	return !McuDio_GetStatus(ins + MCU_DIO_I_KEY1);
}


void KeyScan_MsgEvent(uint8 ins, KeyScan_MsgTypeEnum msgType)
{
	if (KEY_SCAN_INS_KEY3 == ins) {    //++
		if (KEYMSG_SHORT_RELEASE == msgType || KEYMSG_LONG_RELEASE == msgType) {
			if (0 == KeySetSwitch) {
				MATH_NUM_ADD_LIMIT(KeySetNum1, 1, VBUS_MAX);
			} else {
				MATH_NUM_ADD_LIMIT(KeySetNum2, 1, IBUS_MAX);
			}
			Sc8115_SetVbus_Ibus(KeySetNum1, KeySetNum2);
			Param_SaveTrig(1000);
		} else if (KEYMSG_LONGING == msgType) {
			if (0 == KeySetSwitch) {
				MATH_NUM_ADD_LIMIT(KeySetNum1, 10, VBUS_MAX);
			} else {
				MATH_NUM_ADD_LIMIT(KeySetNum2, 10, IBUS_MAX);
			}
			Sc8115_SetVbus_Ibus(KeySetNum1, KeySetNum2);
			Param_SaveTrig(1000);
		}
	} else if (KEY_SCAN_INS_KEY2 == ins) {    //--
		if (KEYMSG_SHORT_RELEASE == msgType || KEYMSG_LONG_RELEASE == msgType) {
			if (0 == KeySetSwitch) {
				MATH_NUM_SUB_LIMIT(KeySetNum1, 1, VBUS_MIN);
			} else {
				MATH_NUM_SUB_LIMIT(KeySetNum2, 1, IBUS_MIN);
			}
			Sc8115_SetVbus_Ibus(KeySetNum1, KeySetNum2);
			Param_SaveTrig(1000);
		} else if (KEYMSG_LONGING == msgType) {
			if (0 == KeySetSwitch) {
				MATH_NUM_SUB_LIMIT(KeySetNum1, 10, VBUS_MIN);
			} else {
				MATH_NUM_SUB_LIMIT(KeySetNum2, 10, IBUS_MIN);
			}
			Sc8115_SetVbus_Ibus(KeySetNum1, KeySetNum2);
			Param_SaveTrig(1000);
		}
	} else if (KEY_SCAN_INS_KEY4 == ins) {    // 切换
		if (KEYMSG_SHORT_RELEASE == msgType || KEYMSG_LONG_RELEASE == msgType) {
			KeySetSwitch = !KeySetSwitch;
			Param_SaveTrig(1000);
		}
	} else if (KEY_SCAN_INS_KEY1 == ins) {    // 启动/输出
		if (KEYMSG_SHORT_RELEASE == msgType || KEYMSG_LONG_RELEASE == msgType) {
			Param_Info.StartStop = !Param_Info.StartStop;
			McuDio_SetStatus(MCU_DIO_O_SC_PSTOP, !Param_Info.StartStop);
			Param_SaveTrig(1000);
		}
	}
}

/**
 * @brief
 *
 */
void KeyScan_UserInit(void)
{
	KeyScan_CfgType config = {0};

	config.InsCfgPtr = KeyScan_InsCfg;
	config.InsInfPtr = KeyScan_InsInf;
	config.InsNum = KEY_SCAN_INS_TOTAL_NUM;
	config.KeyBspInit_FuncPtr = NULL;           // KeyHardware_Init;//可选
	config.IsPress_FuncPtr = Key_GetStatus;     // 必须
	config.GetSysTick_FuncPtr = Tim_GetTick;    // 必须
	config.MsgCbk_FuncPtr = KeyScan_MsgEvent;   // 必须
	KeyScan_Init(&config);
	Task_Creat(TASK_NOR, KeyScan_Main, 5);
}

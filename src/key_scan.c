/**
 * @file key_scan.c
 * @author wwyyy (1046685883@qq.com)
 * @brief
 * @version 3.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2015-2026 oldking-ecu, All rights reserved
 *
 */
#include "key_scan.h"
#include "public.h"
#include "string.h"

/**
 * @brief
 *
 */
typedef struct {
	const KeyScan_CfgType Config;
} KeyScan_InfType;

/**
 * @brief
 *
 */
KeyScan_InfType KeyScan_Info;

#define CONFIG(Member) (KeyScan_Info.Config.Member)
#define INSCFG(Member) ((CONFIG(InsCfgPtr) + ins)->Member)
#define INSINF(Member) ((CONFIG(InsInfPtr) + ins)->Member)


/**
 * @brief
 *
 * @param cfgPtr:
 */
void KeyScan_Init(KeyScan_CfgType *cfgPtr)
{
	memset(&KeyScan_Info, 0, sizeof(KeyScan_Info));
	if (NULL == cfgPtr)
		return;
	memcpy((void *)&KeyScan_Info.Config, cfgPtr, sizeof(KeyScan_CfgType));
	if (CONFIG(KeyBspInit_FuncPtr))
		CONFIG(KeyBspInit_FuncPtr)();
}

/**
 * @brief
 *
 */
void KeyScan_Main(void)
{
	uint8 ins;

	for (ins = 0; ins < CONFIG(InsNum); ins++) {
		switch (INSINF(Status)) {
		case 0:    // 检测按下(消抖)
			if (CONFIG(IsPress_FuncPtr)(ins)) {
				INSINF(Timstamp) = CONFIG(GetSysTick_FuncPtr)();
				INSINF(Status)++;
			}
			break;
		case 1:    // 确认被按下
			if (CheckTimeout(INSINF(Timstamp), CONFIG(GetSysTick_FuncPtr)(), INSCFG(JitterMs))) {
				if (CONFIG(IsPress_FuncPtr)(ins)) {
					CONFIG(MsgCbk_FuncPtr)(ins, KEYMSG_SHORT);
					INSINF(Status)++;
				} else {
					INSINF(Status) = 0;
				}
			}
			break;
		case 2:    // 等待短按释放
			if (CONFIG(IsPress_FuncPtr)(ins)) {
				if (CheckTimeout(INSINF(Timstamp), CONFIG(GetSysTick_FuncPtr)(), INSCFG(LongMs))) {
					CONFIG(MsgCbk_FuncPtr)(ins, KEYMSG_LONG_ONCE);
					INSINF(Timstamp) = CONFIG(GetSysTick_FuncPtr)();
					INSINF(Timstamp1) = CONFIG(GetSysTick_FuncPtr)();
					INSINF(Status)++;
				}
			} else {
				CONFIG(MsgCbk_FuncPtr)(ins, KEYMSG_SHORT_RELEASE);
				INSINF(Status) = 0;
			}
			break;
		case 3:    // 长按并检查期间加速
			if (CONFIG(IsPress_FuncPtr)(ins)) {
				if (CheckTimeout(INSINF(Timstamp), CONFIG(GetSysTick_FuncPtr)(), INSCFG(LongPeriodMs))) {
					CONFIG(MsgCbk_FuncPtr)(ins, KEYMSG_LONGING);
					INSINF(Timstamp) = CONFIG(GetSysTick_FuncPtr)();
				}
				if (CheckTimeout(INSINF(Timstamp1), CONFIG(GetSysTick_FuncPtr)(), INSCFG(LongAccelMs))) {
					INSINF(Timstamp1) = CONFIG(GetSysTick_FuncPtr)();
					INSINF(Status)++;
				}
			} else {
				CONFIG(MsgCbk_FuncPtr)(ins, KEYMSG_LONG_RELEASE);
				INSINF(Status) = 0;
			}
			break;
		case 4:    // 长按加速中，并检查长长按
			if (CONFIG(IsPress_FuncPtr)(ins)) {
				if (CheckTimeout(INSINF(Timstamp), CONFIG(GetSysTick_FuncPtr)(), INSCFG(LongAccelPeriodMs))) {
					CONFIG(MsgCbk_FuncPtr)(ins, KEYMSG_LONGING);
					INSINF(Timstamp) = CONFIG(GetSysTick_FuncPtr)();
				}
				if (CheckTimeout(INSINF(Timstamp1), CONFIG(GetSysTick_FuncPtr)(), INSCFG(LongLongOnceMs))) {
					CONFIG(MsgCbk_FuncPtr)(ins, KEYMSG_LONG_LONG_ONCE);
					INSINF(Timstamp1) = CONFIG(GetSysTick_FuncPtr)();
					INSINF(Status)++;
				}
			} else {
				CONFIG(MsgCbk_FuncPtr)(ins, KEYMSG_LONG_RELEASE);
				INSINF(Status) = 0;
			}
			break;
		case 5:    // 等待长按释放
			if (CONFIG(IsPress_FuncPtr)(ins)) {
				if (CheckTimeout(INSINF(Timstamp), CONFIG(GetSysTick_FuncPtr)(), INSCFG(LongAccelPeriodMs))) {
					CONFIG(MsgCbk_FuncPtr)(ins, KEYMSG_LONGING);
					INSINF(Timstamp) = CONFIG(GetSysTick_FuncPtr)();
				}
			} else {
				CONFIG(MsgCbk_FuncPtr)(ins, KEYMSG_LONG_RELEASE);
				INSINF(Status) = 0;
			}
			break;
		default:
			INSINF(Status) = 0;
			break;
		}
	}
}

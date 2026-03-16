/*!
 * FileName: SignalController.cpp
 *
 * Author:   ZhangChaoZe
 * Date:     2026-1-1
 * Code:     UTF-8 without BOM
 * LineFeed: Unix LF
 *
 * Describe: 信号-槽转发器
 */

#include <QMetaType>

#include "SignalController.h"

SignalController *SignalController::instances = NULL; //单实例句柄初始化

void SignalController::setConnectInfo(struct ConnectInfo *connectList, int item_num)
{
	int i = 0;
	for (i = 0; i < item_num; i++)
	{
		connect(connectList[i].sender, connectList[i].signal_str, connectList[i].reciever, connectList[i].sig_slot_str, connectList[i].connect_type);
	}
}

void SignalController::registerMetaALLType()
{

}

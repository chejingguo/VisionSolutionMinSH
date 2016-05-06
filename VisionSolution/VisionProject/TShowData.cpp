#include "stdafx.h"
#include "TShowData.h"


TShowData::TShowData()
{
}


TShowData::~TShowData()
{
}

string TShowData::sWorkMode = "准备";

string TShowData::sConnectIP = "未连接";
int    TShowData::iPort = 0;
bool   TShowData::bConnectFlag=false;
/********************************************************************
	created:	2014/02/13
	created:	13:2:2014   18:44
	filename: 	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\Classes\FKCommon.h
	file path:	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\Classes
	file base:	FKCommon
	file ext:	h
	author:		FreeKnight
	
	purpose:	
*********************************************************************/

#pragma once

//--------------------------------------------------------------------
#include "cocos2d.h"
#include "CCEGLView.h"
//--------------------------------------------------------------------
USING_NS_CC;
//--------------------------------------------------------------------
#define FKLOG_STRING			0x12345678
#define FKLOG(format, ...)			FKLog(format, ##__VA_ARGS__)
#ifndef MAX_LOG_LEN
#define MAX_LOG_LEN         (cocos2d::kMaxLogLen + 1)
#endif

#ifdef CCLOG
#undef CCLOG
#define CCLOG FKLOG
#endif
//--------------------------------------------------------------------
void SendLogToWindow(const char *log);
void FKLog(const char * pszFormat, ...);
//--------------------------------------------------------------------
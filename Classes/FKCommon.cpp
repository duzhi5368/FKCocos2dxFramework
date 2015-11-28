/********************************************************************
	created:	2014/02/13
	created:	13:2:2014   18:53
	filename: 	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\Classes\FKCommon.cpp
	file path:	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\Classes
	file base:	FKCommon
	file ext:	cpp
	author:		FreeKnight
	
	purpose:	
*********************************************************************/

#include "FKCommon.h"

//--------------------------------------------------------------------
void SendLogToWindow(const char *log)
{
	// Send data as a message
	COPYDATASTRUCT myCDS;
	myCDS.dwData = FKLOG_STRING;
	myCDS.cbData = (DWORD)strlen(log) + 1;
	myCDS.lpData = (PVOID)log;
	HWND hwnd = CCEGLView::sharedOpenGLView()->getHWnd();
	SendMessage(hwnd,
		WM_COPYDATA,
		(WPARAM)(HWND)hwnd,
		(LPARAM)(LPVOID)&myCDS);
}
//--------------------------------------------------------------------
void FKLog(const char * pszFormat, ...)
{
	char szBuf[MAX_LOG_LEN - 20] = {0};
	char szShowBuf[MAX_LOG_LEN] = { 0 };

	struct tm *tm;  
	time_t timep;  
	time(&timep);  
	tm = localtime(&timep);  
	int hour=tm->tm_hour;  
	int minute=tm->tm_min;  
	int second=tm->tm_sec;
	char szTime[10] = { 0 };
	sprintf( szTime, "%02d:%02d:%02d",  hour, minute, second );

	va_list ap;
	va_start(ap, pszFormat);
	vsnprintf_s(szBuf, MAX_LOG_LEN - 20, MAX_LOG_LEN - 20, pszFormat, ap);
	va_end(ap);

	sprintf_s( szShowBuf, "%s - %s", szTime, szBuf );

	SendLogToWindow(szShowBuf);

	OutputDebugStringA(szShowBuf);
	OutputDebugStringA("\n");

	puts(szShowBuf);
}
//--------------------------------------------------------------------
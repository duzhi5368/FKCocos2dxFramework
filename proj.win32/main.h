/********************************************************************
	created:	2014/02/12
	created:	12:2:2014   16:27
	filename: 	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\proj.win32\main.h
	file path:	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\proj.win32
	file base:	main
	file ext:	h
	author:		FreeKnight
	
	purpose:	
*********************************************************************/

#pragma once

//--------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//--------------------------------------------------------------------
// Windows Header Files:
#include <windows.h>
//--------------------------------------------------------------------
// C RunTime Header Files
#include "CCStdC.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
//--------------------------------------------------------------------
// resource
#include "resource.h"
//--------------------------------------------------------------------
// STL
#include <string>
#include <vector>
//--------------------------------------------------------------------
// cocos2d head
#include "cocos2d.h"
//--------------------------------------------------------------------
// self code
#include "AppDelegate.h"
//--------------------------------------------------------------------
using namespace std;
using namespace cocos2d;
//--------------------------------------------------------------------
class CFKCocos2dxAppWrapper : public CCObject
{
public:
	static int	CreateAndRun();
public:
	AppDelegate*	GetApp(){
		return m_pApp;
	}
	HWND GetWindowHandle()
	{
		return m_hWnd;
	}
private:
	AppDelegate *			m_pApp;
	HWND						m_hWnd;
private:
	static CFKCocos2dxAppWrapper*		s_WrapperInstance;
	static CFKCocos2dxAppWrapper*		GetWrapperInstance();
	static void											PurgeSharedInstance();
private:
	CFKCocos2dxAppWrapper();
private:
	CFKAppConfig			m_tagAppConfig;
	FILE*							m_pWriteDebugLogFile;
	BOOL						m_bIsExit;
	float							m_fCurSpeed;
private:
	int							Run();
	void							LoadProjectConfig();
	void							CreateViewMenu();
	void							UpdateViewMenu();
	void							Relaunch();
private:
	void							WriteDebugLog( const char* p_szLog );
private:
	void							OnFileCreateProjectShortcut();
	void							OnFileProjectConfig();
	void							OnFileRelaunch();
	void							OnFileExit();
	void							OnHelpAbout();
	void							OnPauseGame();
	void							OnResumeGame();
	void							OnIncreaseSpeed();
	void							OnReduceSpeed();
	void							OnChangeSpeed( float p_fValue );
	void							OnShowAppInfo();
private:
	void							OnViewChangeFrameSize( int p_nViewMenuID );
	void							OnViewChangeOrientation( int p_nViewMenuID );
	void							OnViewChangeZoom( int p_nScaleMode );
private:
	static LRESULT						WindowProc(UINT message, WPARAM wParam, LPARAM lParam, BOOL *pProcessed);
	static INT_PTR CALLBACK		AboutDialogCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};
//--------------------------------------------------------------------
class CWrapperConfigDialog
{
public:
	static bool ShowModal(HWND hwnd, CFKAppConfig *p_pAppConfig);
public:
	~CWrapperConfigDialog();
private:
	CWrapperConfigDialog( HWND p_hWnd );
	
	static CWrapperConfigDialog* GetInstance(){
		return s_DialogInstance;
	}
	static CWrapperConfigDialog*	s_DialogInstance;
private:
	bool ShowDialog(CFKAppConfig *p_pAppConfig);
	bool CheckConfig();
private:
	void OnInitDialog(HWND p_hWndDialog);
	void OnScreenSizeChanged();
	void OnScreenDirectionChanged();
	void OnOK();
private:
	static INT_PTR CALLBACK DialogCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL DirectoryExists(const string p_szPath);
	static BOOL FileExists(const string p_szPath);
	static string GetCurAppPath();
private:
	CFKAppConfig							m_tagAppConfig;
	HWND										m_hWnd;
	HWND										m_hWndDialog;
	bool											m_bIsSaveDialog;
};
//--------------------------------------------------------------------
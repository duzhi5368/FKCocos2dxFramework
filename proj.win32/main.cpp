/********************************************************************
	created:	2014/02/12
	created:	12:2:2014   16:38
	filename: 	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\proj.win32\main.cpp
	file path:	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\proj.win32
	file base:	main
	file ext:	cpp
	author:		FreeKnight
	
	purpose:	
*********************************************************************/

#pragma comment(lib, "comctl32.lib")
// #pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='X86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif
//--------------------------------------------------------------------

#include "main.h"

#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sstream>

#include <Commdlg.h>
#include <Shlobj.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shellapi.h>

#include <Windowsx.h>
#include <Shlobj.h>
#include <psapi.h>
#pragma comment(lib,"psapi.lib")

#include "CCEGLView.h"
#include "SimpleAudioEngine.h"

#include "FKCommon.h"
//--------------------------------------------------------------------
USING_NS_CC;
//--------------------------------------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	return CFKCocos2dxAppWrapper::CreateAndRun();
}
//--------------------------------------------------------------------
// CFKCocos2dxAppWrapper
//--------------------------------------------------------------------
CFKCocos2dxAppWrapper*		CFKCocos2dxAppWrapper::s_WrapperInstance = NULL;
//--------------------------------------------------------------------
int CFKCocos2dxAppWrapper::CreateAndRun()
{
	CFKCocos2dxAppWrapper* pInstance = CFKCocos2dxAppWrapper::GetWrapperInstance();
	if( pInstance == NULL ) return -1;
	int nRet = pInstance->Run();
	CFKCocos2dxAppWrapper::PurgeSharedInstance();
	return nRet;
}
//--------------------------------------------------------------------
CFKCocos2dxAppWrapper* CFKCocos2dxAppWrapper::GetWrapperInstance()
{
	if( s_WrapperInstance == NULL )
	{
		s_WrapperInstance = new CFKCocos2dxAppWrapper();
	}
	return s_WrapperInstance;
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::PurgeSharedInstance()
{
	if( s_WrapperInstance )
	{
		delete s_WrapperInstance;
		s_WrapperInstance = NULL;
	}
}
//--------------------------------------------------------------------
CFKCocos2dxAppWrapper::CFKCocos2dxAppWrapper()
	: m_pApp( NULL )
	, m_hWnd( NULL )
	, m_bIsExit( TRUE )
	, m_pWriteDebugLogFile( NULL )
	, m_fCurSpeed( 1.0f )
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
}
//--------------------------------------------------------------------
int CFKCocos2dxAppWrapper::Run()
{
	bool bRet = 0;

	// first ,load project config
	LoadProjectConfig();

	// create consle
	HWND hwndConsole = NULL;
	if (m_tagAppConfig.IsShowConsole())
	{
		AllocConsole();
		freopen("CONOUT$", "wt", stdout);
		freopen("CONOUT$", "wt", stderr);

		// disable close console
		hwndConsole = GetConsoleWindow();
		if (hwndConsole != NULL)
		{
			HMENU hMenu = GetSystemMenu(hwndConsole, FALSE);
			if (hMenu != NULL) DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);

			ShowWindow(hwndConsole, SW_SHOW);
			BringWindowToTop(hwndConsole);
		}
	}

	// create log file
	if (m_tagAppConfig.IsWriteDebugLogToFile())
	{
		const string debugLogFilePath = m_tagAppConfig.GetDebugLogFilePath();
		m_pWriteDebugLogFile = fopen(debugLogFilePath.c_str(), "w");
		if ( m_pWriteDebugLogFile == NULL )
		{
			FKLOG("Cannot create debug log file %s", debugLogFilePath.c_str());
		}
	}

	// MainLoop
	do
	{
		m_bIsExit = TRUE;

		// create app
		m_pApp = new AppDelegate();
		if( m_pApp == NULL )
		{
			bRet = false;
			break;
		}

		// set some environments
		m_pApp->SetProjectConfig( m_tagAppConfig );
		CCFileUtils::sharedFileUtils()->addSearchPath( m_tagAppConfig.GetProjectDir().c_str() );

		// create openGL view
		CCEGLView* eglView = CCEGLView::sharedOpenGLView();
		eglView->setMenuResource(MAKEINTRESOURCE(IDC_LUAHOSTWIN32));
		eglView->setWndProc (WindowProc );
		eglView->setFrameSize( m_tagAppConfig.GetFrameSize().width, m_tagAppConfig.GetFrameSize().height );
		eglView->setFrameZoomFactor( m_tagAppConfig.GetFrameScale() );

		// set window actived
		m_hWnd = eglView->getHWnd();
		BringWindowToTop(m_hWnd);
		SetWindowTextA(m_hWnd, "FKCocos2dxAppWrapper Project");

		// center window position
		eglView->centerWindow();

		// set icon
		HICON icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_LUAHOSTWIN32));
		SendMessage( m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
		if (hwndConsole)
		{
			SendMessage(hwndConsole, WM_SETICON, ICON_BIG, (LPARAM)icon);
		}

		// set menu
		CreateViewMenu();
		UpdateViewMenu();

		// run games 
		m_pApp->run();

		// clean up
		CocosDenshion::SimpleAudioEngine::end();

		delete m_pApp;
		m_pApp = NULL;

	}while( !m_bIsExit );

	// ready exit
	FreeConsole();
	if( m_pWriteDebugLogFile != NULL )
	{
		fclose(m_pWriteDebugLogFile);
	}

	return bRet;
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::LoadProjectConfig()
{
	vector<string> args;
	for (int i = 0; i < __argc; ++i)
	{
		wstring ws(__wargv[i]);
		string s;
		s.assign(ws.begin(), ws.end());
		args.push_back(s);
	}
	m_tagAppConfig.ParseCommandLine(args);
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::CreateViewMenu()
{
	HMENU menu = GetMenu(m_hWnd);
	HMENU viewMenu = GetSubMenu(menu, 1);

	for (int i = CSimulatorConfig::GetInstance()->GetScreenSizeCount() - 1; i >= 0; --i)
	{
		SSimulatorScreenSize size = CSimulatorConfig::GetInstance()->GetScreenSize(i);
		wstring menuName;
		menuName.assign(size.m_szTitle.begin(), size.m_szTitle.end());

		MENUITEMINFO item;
		ZeroMemory(&item, sizeof(item));
		item.cbSize = sizeof(item);
		item.fMask = MIIM_FTYPE | MIIM_ID | MIIM_STRING;
		item.fType = MFT_STRING;
		item.wID = ID_VIEW_SIZE + i;
		item.dwTypeData = (LPTSTR)menuName.c_str();
		item.cch = menuName.length();

		InsertMenuItem(viewMenu, 0, TRUE, &item);
	}
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::UpdateViewMenu()
{
	HMENU menu = GetMenu(m_hWnd);
	HMENU viewMenu = GetSubMenu(menu, 1);

	if (m_tagAppConfig.IsLandscapeFrame())
	{
		CheckMenuItem(viewMenu, ID_VIEW_PORTRAIT, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(viewMenu, ID_VIEW_LANDSCAPE, MF_BYCOMMAND | MF_CHECKED);
	}
	else
	{
		CheckMenuItem(viewMenu, ID_VIEW_PORTRAIT, MF_BYCOMMAND | MF_CHECKED);
		CheckMenuItem(viewMenu, ID_VIEW_LANDSCAPE, MF_BYCOMMAND | MF_UNCHECKED);
	}

	if (m_tagAppConfig.GetFrameScale() < 1.0f)
	{
		CheckMenuItem(viewMenu, ID_VIEW_RESET_ZOOM, MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(viewMenu, ID_VIEW_ZOOM_OUT, MF_BYCOMMAND | MF_CHECKED);
	}
	else
	{
		CheckMenuItem(viewMenu, ID_VIEW_RESET_ZOOM, MF_BYCOMMAND | MF_CHECKED);
		CheckMenuItem(viewMenu, ID_VIEW_ZOOM_OUT, MF_BYCOMMAND | MF_UNCHECKED);
	}

	int current = CSimulatorConfig::GetInstance()->CheckScreenSize(m_tagAppConfig.GetFrameSize());
	for (int i = CSimulatorConfig::GetInstance()->GetScreenSizeCount() - 1; i >= 0; --i)
	{
		CheckMenuItem(viewMenu, i, MF_BYPOSITION | (i == current ? MF_CHECKED : MF_UNCHECKED));
	}

	HMENU controlMenu = GetSubMenu(menu, 2);
	if( CCDirector::sharedDirector()->isPaused() ) 
	{
		CheckMenuItem(controlMenu, ID_COUNTINUE,  MF_BYCOMMAND | MF_UNCHECKED);
		CheckMenuItem(controlMenu, ID_PAUSE,  MF_BYCOMMAND | MF_CHECKED);
	}
	else
	{
		CheckMenuItem(controlMenu, ID_COUNTINUE,  MF_BYCOMMAND | MF_CHECKED);
		CheckMenuItem(controlMenu, ID_PAUSE,  MF_BYCOMMAND | MF_UNCHECKED);
	}

	CheckMenuItem(controlMenu, ID_4X,  MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(controlMenu, ID_2X,  MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(controlMenu, ID_1X,  MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(controlMenu, ID_05X,  MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(controlMenu, ID_025X,  MF_BYCOMMAND | MF_UNCHECKED);
	if( m_fCurSpeed == 4.0f )
	{
		CheckMenuItem(controlMenu, ID_4X,  MF_BYCOMMAND | MF_CHECKED);
	}
	else if( m_fCurSpeed == 2.0f )
	{
		CheckMenuItem(controlMenu, ID_2X,  MF_BYCOMMAND | MF_CHECKED);
	}
	else if( m_fCurSpeed == 1.0f )
	{
		CheckMenuItem(controlMenu, ID_1X,  MF_BYCOMMAND | MF_CHECKED);
	}
	else if( m_fCurSpeed == 0.5f )
	{
		CheckMenuItem(controlMenu, ID_05X,  MF_BYCOMMAND | MF_CHECKED);
	}
	else if( m_fCurSpeed == 0.25f )
	{
		CheckMenuItem(controlMenu, ID_025X,  MF_BYCOMMAND | MF_CHECKED);
	}
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::Relaunch()
{
	string commandLine = m_tagAppConfig.MakeCommandLine();
	TCHAR moduleName[MAX_PATH];
	ZeroMemory(moduleName, sizeof(moduleName));
	GetModuleFileName(NULL, moduleName, MAX_PATH);

	wstring ws;
	ws.append(L"\"");
	ws.append(moduleName);
	ws.append(L"\" ");
	ws.append(commandLine.begin(), commandLine.end());

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	lstrcpyW(moduleName, ws.c_str());
	if (CreateProcess(NULL, moduleName, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{
		ExitProcess(0);
	}
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::WriteDebugLog( const char* p_szLog )
{
	if (!m_pWriteDebugLogFile) return;

	fputs(p_szLog, m_pWriteDebugLogFile);
	fputc('\n', m_pWriteDebugLogFile);
	fflush(m_pWriteDebugLogFile);
}
//--------------------------------------------------------------------
LRESULT CFKCocos2dxAppWrapper::WindowProc(UINT message, WPARAM wParam, LPARAM lParam, BOOL *pProcessed)
{
	int wmId, wmEvent;
	CFKCocos2dxAppWrapper *pInstance = CFKCocos2dxAppWrapper::GetWrapperInstance();
	HWND hWnd = pInstance->GetWindowHandle();

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case ID_FILE_CREATE_PROJECT_SHORTCUT:
			pInstance->OnFileCreateProjectShortcut();
			break;

		case ID_FILE_PROJECT_CONFIG:
			pInstance->OnFileProjectConfig();
			break;

		case ID_FILE_RELAUNCH:
			pInstance->OnFileRelaunch();
			break;

		case ID_FILE_EXIT:
			pInstance->OnFileExit();
			break;

		case ID_VIEW_PORTRAIT:
		case ID_VIEW_LANDSCAPE:
			pInstance->OnViewChangeOrientation(wmId);
			break;

		case ID_VIEW_RESET_ZOOM:
		case ID_VIEW_ZOOM_OUT:
			pInstance->OnViewChangeZoom(wmId);
			break;

		case ID_HELP_ABOUT:
			pInstance->OnHelpAbout();
			break;

		case ID_PAUSE:
			pInstance->OnPauseGame();
			break;

		case ID_COUNTINUE:
			pInstance->OnResumeGame();
			break;

		case ID_4X:
			pInstance->OnChangeSpeed( 4.0f );
			break;

		case ID_2X:
			pInstance->OnChangeSpeed( 2.0f );
			break;

		case ID_1X:
			pInstance->OnChangeSpeed( 1.0f );
			break;

		case ID_05X:
			pInstance->OnChangeSpeed( 0.5f );
			break;

		case ID_025X:
			pInstance->OnChangeSpeed( 0.25f );
			break;

		default:
			if (wmId >= ID_VIEW_SIZE && wmId <= ID_VIEW_SIZE + CSimulatorConfig::GetInstance()->GetScreenSizeCount() - 1)
			{
				pInstance->OnViewChangeFrameSize(wmId);
				break;
			}
			return 0;
		}
		break;

	case WM_KEYDOWN:
		if( wParam == VK_F1 )
		{
			if( GetKeyState(VK_CONTROL) & 0x8000 )
			{
				// Ctrl + F1
				pInstance->OnHelpAbout();
			}
			else
			{
				// F1
				if( CCDirector::sharedDirector()->isPaused() )
				{
					pInstance->OnResumeGame();
				}
				else
				{
					pInstance->OnPauseGame();
				}
			}
		}
		else if( wParam == VK_F2 )
		{
			pInstance->OnIncreaseSpeed();
		}
		else if( wParam == VK_F3 )
		{
			pInstance->OnReduceSpeed();
		}
		else if( wParam == VK_F4 )
		{
			pInstance->OnShowAppInfo();
		}
		else if (wParam == VK_F5)
		{
			pInstance->OnFileRelaunch();
		}
		else if( wParam == VK_F6 )
		{
			pInstance->OnFileProjectConfig();
		}
		break;

	case WM_COPYDATA:
		{
			PCOPYDATASTRUCT pMyCDS = (PCOPYDATASTRUCT) lParam;
			if (pMyCDS->dwData == FKLOG_STRING)
			{
				const char *szBuf = (const char*)(pMyCDS->lpData);
				GetWrapperInstance()->WriteDebugLog(szBuf);
				break;
			}
		}

	default:
		return 0;
	}

	*pProcessed = TRUE;
	return 0;
}
//--------------------------------------------------------------------
INT_PTR CALLBACK CFKCocos2dxAppWrapper::AboutDialogCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnFileCreateProjectShortcut()
{
	WCHAR shortcutPathBuff[MAX_PATH + 1] = {0};

	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner   = m_hWnd;
	ofn.lpstrFilter = L"快捷方式 (*.lnk)\0*.lnk\0";
	ofn.lpstrTitle  = L"创建项目快捷方式";
	ofn.Flags       = OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	ofn.lpstrFile   = shortcutPathBuff;
	ofn.nMaxFile    = MAX_PATH;

	if (!GetSaveFileName(&ofn)) return;

	// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
	// has already been called.
	IShellLink* psl;
	HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);

	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;

		// args
		string args = m_tagAppConfig.MakeCommandLine();

		// Set the path to the shortcut target and add the description.
		psl->SetPath(__wargv[0]);
		wstring wargs;
		wargs.assign(args.begin(), args.end());
		psl->SetArguments(wargs.c_str());
		psl->SetDescription(L"FKCocos2dxApp封装快捷方式");

		// Query IShellLink for the IPersistFile interface, used for saving the
		// shortcut in persistent storage.
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

		if (SUCCEEDED(hres))
		{
			// Save the link by calling IPersistFile::Save.
			size_t len = wcslen(shortcutPathBuff);
			if (_wcsicmp(shortcutPathBuff + len - 4, L".lnk") != 0)
			{
				wcscat_s(shortcutPathBuff, L".lnk");
			}
			hres = ppf->Save(shortcutPathBuff, TRUE);
			ppf->Release();
		}
		psl->Release();
	}
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnFileProjectConfig()
{
	CFKAppConfig project;
	if (!m_tagAppConfig.IsWelcome())
	{
		project = m_tagAppConfig;
	}
	if (CWrapperConfigDialog::ShowModal(m_hWnd, &project))
	{
		m_tagAppConfig = project;
		Relaunch();
	}
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnFileRelaunch()
{
	Relaunch();
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnFileExit()
{
	DestroyWindow(m_hWnd);
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnHelpAbout()
{
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWnd, AboutDialogCallback);
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnPauseGame()
{
	if( !CCDirector::sharedDirector()->isPaused() )
	{
		CCDirector::sharedDirector()->pause();
	}
	UpdateViewMenu();
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnResumeGame()
{
	if( CCDirector::sharedDirector()->isPaused() )
	{
		CCDirector::sharedDirector()->resume();
	}
	UpdateViewMenu();
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnIncreaseSpeed()
{
	if( m_fCurSpeed < 4.0f )
	{
		m_fCurSpeed *= 2;
	}
	CCDirector::sharedDirector()->getScheduler()->setTimeScale(m_fCurSpeed);
	UpdateViewMenu();
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnReduceSpeed()
{
	if( m_fCurSpeed > 0.25f )
	{
		m_fCurSpeed /= 2;
	}
	CCDirector::sharedDirector()->getScheduler()->setTimeScale(m_fCurSpeed);
	UpdateViewMenu();
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnChangeSpeed( float p_fValue )
{
	if( p_fValue == 4.0f || p_fValue == 2.0f || p_fValue == 1.0f || p_fValue == 0.5f || p_fValue == 0.25f )
	{
		m_fCurSpeed = p_fValue;
		CCDirector::sharedDirector()->getScheduler()->setTimeScale(m_fCurSpeed);
		UpdateViewMenu();
	}
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnShowAppInfo()
{
	CCLOG("----------------------------------------");
	CCTextureCache::sharedTextureCache()->dumpCachedTextureInfo();
	FKLOG("----------------------------------------");
	HANDLE handle=GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle,&pmc,sizeof(pmc));
	FKLOG("内存使用        : %d K", pmc.WorkingSetSize/1000 );
	FKLOG("内存峰值使用    : %d K", pmc.PeakWorkingSetSize/1000);
	FKLOG("虚拟内存使用    : %d K", pmc.PagefileUsage/1000 );
	FKLOG("虚拟内存峰值使用: %d K", pmc.PeakPagefileUsage/1000);
	FKLOG("----------------------------------------");
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnViewChangeFrameSize( int p_nViewMenuID )
{
	int index = p_nViewMenuID - ID_VIEW_SIZE;

	if (index >= 0 && index < CSimulatorConfig::GetInstance()->GetScreenSizeCount())
	{
		SSimulatorScreenSize size = CSimulatorConfig::GetInstance()->GetScreenSize(index);
		bool isLandscape = m_tagAppConfig.IsLandscapeFrame();
		m_tagAppConfig.SetFrameSize(CCSize(size.m_nWidth, size.m_nHeight));
		if (isLandscape)
		{
			m_tagAppConfig.ChangeFrameOrientationToLandscape();
		}
		else
		{
			m_tagAppConfig.ChangeFrameOrientationToPortait();
		}
		m_tagAppConfig.SetFrameScale(1.0f);
		Relaunch();
	}
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnViewChangeOrientation( int p_nViewMenuID )
{
	bool isLandscape = m_tagAppConfig.IsLandscapeFrame();
	bool isNeedRelaunch = false;
	if (p_nViewMenuID == ID_VIEW_PORTRAIT && isLandscape)
	{
		m_tagAppConfig.ChangeFrameOrientationToPortait();
		isNeedRelaunch = true;
	}
	else if (p_nViewMenuID == ID_VIEW_LANDSCAPE && !isLandscape)
	{
		m_tagAppConfig.ChangeFrameOrientationToLandscape();
		isNeedRelaunch = true;
	}

	if (isNeedRelaunch) Relaunch();
}
//--------------------------------------------------------------------
void CFKCocos2dxAppWrapper::OnViewChangeZoom( int p_nScaleMode )
{
	float scale = 1.0f;
	if (p_nScaleMode == ID_VIEW_ZOOM_OUT)
	{
		scale = 0.5f;
	}

	m_tagAppConfig.SetFrameScale(scale);
	CCEGLView::sharedOpenGLView()->setFrameZoomFactor(scale);
	CCEGLView::sharedOpenGLView()->resize(m_tagAppConfig.GetFrameSize().width * scale,
		m_tagAppConfig.GetFrameSize().height * scale);
	UpdateViewMenu();
}
//--------------------------------------------------------------------
// CWrapperConfigDialog
//--------------------------------------------------------------------
CWrapperConfigDialog *CWrapperConfigDialog::s_DialogInstance = NULL;
//--------------------------------------------------------------------
CWrapperConfigDialog::~CWrapperConfigDialog()
{
	s_DialogInstance = NULL;
}
//--------------------------------------------------------------------
CWrapperConfigDialog::CWrapperConfigDialog( HWND p_hWnd )
	: m_hWnd( p_hWnd )
	, m_hWndDialog( NULL )
	, m_bIsSaveDialog( false )
{
	assert(s_DialogInstance == NULL);
	s_DialogInstance = this;
}
//--------------------------------------------------------------------
bool CWrapperConfigDialog::ShowModal(HWND hwnd, CFKAppConfig *p_pAppConfig )
{
	CWrapperConfigDialog* pDialog = new CWrapperConfigDialog( hwnd );
	bool bRet = pDialog->ShowDialog( p_pAppConfig );
	delete pDialog;
	return bRet;
}
//--------------------------------------------------------------------
bool CWrapperConfigDialog::ShowDialog(CFKAppConfig *p_pAppConfig)
{
	m_tagAppConfig = *p_pAppConfig;
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROJECT_CONFIG), m_hWnd, DialogCallback);
	if (m_bIsSaveDialog)
	{
		*p_pAppConfig = m_tagAppConfig;
	}
	return m_bIsSaveDialog;
}
//--------------------------------------------------------------------
bool CWrapperConfigDialog::CheckConfig()
{
	bool isOK = false;
	char buff[MAX_PATH + 1] = {0};

	do
	{
		// check project dir and script file
		m_tagAppConfig.SetProjectDir( CWrapperConfigDialog::GetCurAppPath().c_str() );

		if (!DirectoryExists(m_tagAppConfig.GetProjectDir().c_str()))
		{
			MessageBox(m_hWndDialog, L"Invalid Project Directory, please check it", L"Error", MB_OK);
			SetFocus(GetDlgItem(m_hWndDialog, IDC_EDIT_PROJECT_DIR));
			break;
		}

		// check screen size
		HWND list = GetDlgItem(m_hWndDialog, IDC_COMBO_SCREEN_SIZE);
		int index = ComboBox_GetCurSel(list);
		CSimulatorConfig *defaults = CSimulatorConfig::GetInstance();

		int w, h;
		if (index < defaults->GetScreenSizeCount())
		{
			const SSimulatorScreenSize &size = defaults->GetScreenSize(index);
			w = size.m_nWidth;
			h = size.m_nHeight;

			if (IsDlgButtonChecked(m_hWndDialog, IDC_RADIO_LANDSCAPE) == BST_CHECKED)
			{
				int w2 = w;
				w = h;
				h = w2;
			}
		}
		else
		{
			GetDlgItemTextA(m_hWndDialog, IDC_EDIT_SCREEN_WIDTH, buff, MAX_PATH);
			w = atoi(buff);
			GetDlgItemTextA(m_hWndDialog, IDC_EDIT_SCREEN_HEIGHT, buff, MAX_PATH);
			h = atoi(buff);

			if (w <= 0)
			{

				MessageBox(m_hWndDialog, L"Invalid Screen Width, please check it", L"Error", MB_OK);
				SetFocus(GetDlgItem(m_hWndDialog, IDC_EDIT_SCREEN_WIDTH));
				break;
			}
			else if (h <= 0)
			{
				MessageBox(m_hWndDialog, L"Invalid Screen Height, please check it", L"Error", MB_OK);
				SetFocus(GetDlgItem(m_hWndDialog, IDC_EDIT_SCREEN_HEIGHT));
				break;
			}
		}

		// ok
		m_tagAppConfig.SetFrameSize(CCSize(w, h));
		m_tagAppConfig.SetShowConsole(IsDlgButtonChecked(m_hWndDialog, IDC_CHECK_SHOW_DEBUG_CONSOLE) == TRUE);
		m_tagAppConfig.SetWriteDebugLogToFile(IsDlgButtonChecked(m_hWndDialog, IDC_CHECK_WRITE_DEBUG_LOG_TO_FILE) == TRUE);
		m_tagAppConfig.SetLoadPrecompiledFramework(IsDlgButtonChecked(m_hWndDialog, IDC_CHECK_LOAD_PRECOMPILED_FRAMEWORK) == TRUE);
		isOK = true;
	} while (false);

	return isOK;
}
//--------------------------------------------------------------------
void CWrapperConfigDialog::OnInitDialog(HWND p_hWndDialog)
{
	CSimulatorConfig *defaults = CSimulatorConfig::GetInstance();
	m_hWndDialog = p_hWndDialog;
	HWND list = GetDlgItem(m_hWndDialog, IDC_COMBO_SCREEN_SIZE);

	BOOL isLandscape = FALSE;
	int currentSizeIndex = defaults->CheckScreenSize(m_tagAppConfig.GetFrameSize());
	if (currentSizeIndex < 0)
	{
		currentSizeIndex = 0;
	}
	else
	{
		isLandscape = m_tagAppConfig.IsLandscapeFrame();
	}

	for (int i = 0; i < defaults->GetScreenSizeCount(); ++i)
	{
		const SSimulatorScreenSize &size = defaults->GetScreenSize(i);
		wstring title;
		title.assign(size.m_szTitle.begin(), size.m_szTitle.end());
		ComboBox_AddString(list, title.c_str());

		if (i == currentSizeIndex)
		{
			char buff[32];
			sprintf_s(buff, "%d", isLandscape ? size.m_nHeight : size.m_nWidth);
			SetDlgItemTextA(m_hWndDialog, IDC_EDIT_SCREEN_WIDTH, buff);
			sprintf_s(buff, "%d", isLandscape ? size.m_nWidth : size.m_nHeight);
			SetDlgItemTextA(m_hWndDialog, IDC_EDIT_SCREEN_HEIGHT, buff);
		}
	}
	ComboBox_AddString(list, L"Custom Size");
	ComboBox_SetCurSel(list, currentSizeIndex);

	Edit_LimitText(GetDlgItem(m_hWndDialog, IDC_EDIT_SCREEN_WIDTH), 4);
	Edit_LimitText(GetDlgItem(m_hWndDialog, IDC_EDIT_SCREEN_HEIGHT), 4);

	HWND direction = GetDlgItem(m_hWndDialog, IDC_RADIO_PORTRAIT);
	CheckRadioButton(m_hWndDialog, IDC_RADIO_PORTRAIT, IDC_RADIO_LANDSCAPE, isLandscape ? IDC_RADIO_LANDSCAPE : IDC_RADIO_PORTRAIT);

	Button_SetCheck(GetDlgItem(m_hWndDialog, IDC_CHECK_SHOW_DEBUG_CONSOLE), m_tagAppConfig.IsShowConsole());
	Button_SetCheck(GetDlgItem(m_hWndDialog, IDC_CHECK_WRITE_DEBUG_LOG_TO_FILE), m_tagAppConfig.IsWriteDebugLogToFile());
	Button_SetCheck(GetDlgItem(m_hWndDialog, IDC_CHECK_LOAD_PRECOMPILED_FRAMEWORK), m_tagAppConfig.IsLoadPrecompiledFramework());

	// center a dialog box within its owner window
	RECT rc, rcOwner, rcDlg;
	GetWindowRect(m_hWnd, &rcOwner); 
	GetWindowRect(m_hWndDialog, &rcDlg); 
	CopyRect(&rc, &rcOwner); 

	OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
	OffsetRect(&rc, -rc.left, -rc.top); 
	OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 

	SetWindowPos(m_hWndDialog,
		HWND_TOP,
		rcOwner.left + (rc.right / 2),
		rcOwner.top + (rc.bottom / 2),
		0, 0,          // Ignores size arguments. 
		SWP_NOSIZE);
}
//--------------------------------------------------------------------
void CWrapperConfigDialog::OnScreenSizeChanged()
{
	HWND list = GetDlgItem(m_hWndDialog, IDC_COMBO_SCREEN_SIZE);
	int index = ComboBox_GetCurSel(list);
	CSimulatorConfig *defaults = CSimulatorConfig::GetInstance();

	int w, h;
	if (index < defaults->GetScreenSizeCount())
	{
		const SSimulatorScreenSize &size = defaults->GetScreenSize(index);
		w = size.m_nWidth;
		h = size.m_nHeight;

		if (IsDlgButtonChecked(m_hWndDialog, IDC_RADIO_LANDSCAPE) == BST_CHECKED)
		{
			int w2 = w;
			w = h;
			h = w2;
		}
	}
	else
	{
		w = -1; h = -1;
	}

	if (w > 0)
	{
		char buff[32];
		sprintf_s(buff, "%d", w);
		SetDlgItemTextA(m_hWndDialog, IDC_EDIT_SCREEN_WIDTH, buff);
		sprintf_s(buff, "%d", h);
		SetDlgItemTextA(m_hWndDialog, IDC_EDIT_SCREEN_HEIGHT, buff);

		Edit_Enable(GetDlgItem(m_hWndDialog, IDC_EDIT_SCREEN_WIDTH), FALSE);
		Edit_Enable(GetDlgItem(m_hWndDialog, IDC_EDIT_SCREEN_HEIGHT), FALSE);
	}
	else
	{
		Edit_Enable(GetDlgItem(m_hWndDialog, IDC_EDIT_SCREEN_WIDTH), TRUE);
		Edit_Enable(GetDlgItem(m_hWndDialog, IDC_EDIT_SCREEN_HEIGHT), TRUE);
		SetFocus(GetDlgItem(m_hWndDialog, IDC_EDIT_SCREEN_WIDTH));
	}
}
//--------------------------------------------------------------------
void CWrapperConfigDialog::OnScreenDirectionChanged()
{
	char buff[32] = {0};
	GetDlgItemTextA(m_hWndDialog, IDC_EDIT_SCREEN_WIDTH, buff, 30);
	int w = atoi(buff);
	GetDlgItemTextA(m_hWndDialog, IDC_EDIT_SCREEN_HEIGHT, buff, 30);
	int h = atoi(buff);

	bool isLandscape = IsDlgButtonChecked(m_hWndDialog, IDC_RADIO_LANDSCAPE) == BST_CHECKED;
	if ((isLandscape && w < h) || (!isLandscape && w > h))
	{
		sprintf_s(buff, "%d", h);
		SetDlgItemTextA(m_hWndDialog, IDC_EDIT_SCREEN_WIDTH, buff);
		sprintf_s(buff, "%d", w);
		SetDlgItemTextA(m_hWndDialog, IDC_EDIT_SCREEN_HEIGHT, buff);
	}
}
//--------------------------------------------------------------------
void CWrapperConfigDialog::OnOK()
{
	if (CheckConfig())
	{
		m_bIsSaveDialog = true;
	}
}
//--------------------------------------------------------------------
INT_PTR CALLBACK CWrapperConfigDialog::DialogCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		GetInstance()->OnInitDialog(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_COMBO_SCREEN_SIZE:
			if (HIWORD(wParam) == CBN_SELENDOK)
			{
				GetInstance()->OnScreenSizeChanged();
				return (INT_PTR)TRUE;
			}
			break;

		case IDC_RADIO_PORTRAIT:
		case IDC_RADIO_LANDSCAPE:
			if (HIWORD(wParam) == BN_CLICKED || HIWORD(wParam) == BN_DBLCLK)
			{
				GetInstance()->OnScreenDirectionChanged();
				return (INT_PTR)TRUE;
			}
			break;

		case IDOK:
			GetInstance()->OnOK();
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;

		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	return (INT_PTR)FALSE;
}
//--------------------------------------------------------------------
BOOL CWrapperConfigDialog::DirectoryExists(const string p_szPath)
{
	DWORD dwAttrib = GetFileAttributesA(p_szPath.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
//--------------------------------------------------------------------
BOOL CWrapperConfigDialog::FileExists(const string p_szPath)
{
	DWORD dwAttrib = GetFileAttributesA(p_szPath.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
//--------------------------------------------------------------------
string CWrapperConfigDialog::GetCurAppPath()
{
	char szFilePath[MAX_PATH + 1]={0};
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	(strrchr(szFilePath, _T('\\')))[1] = 0;
	return string(szFilePath);
}
//--------------------------------------------------------------------
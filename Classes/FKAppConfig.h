/********************************************************************
	created:	2014/02/12
	created:	12:2:2014   18:31
	filename: 	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\Classes\FKAppConfig.h
	file path:	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\Classes
	file base:	FKAppConfig
	file ext:	h
	author:		FreeKnight
	
	purpose:	
*********************************************************************/

#pragma once

//--------------------------------------------------------------------
// STL
#include <string>
#include <vector>
#include "cocos2d.h"
#include "FKCommon.h"
//--------------------------------------------------------------------
using namespace std;
using namespace cocos2d;
//--------------------------------------------------------------------
class CFKAppConfig
{
public:
	CFKAppConfig();
public:
	const bool IsShowConsole();
	void SetShowConsole( bool p_bIsShowConsole );

	const bool IsWriteDebugLogToFile();
	void SetWriteDebugLogToFile( bool p_IsWriteDebugLogToFile);
	const string GetDebugLogFilePath();

	bool IsLoadPrecompiledFramework();
	void SetLoadPrecompiledFramework(bool p_bIsLoad);

	const string GetProjectDir();
	void SetProjectDir( const string p_szProjectDir );

	const CCSize GetFrameSize();
	void SetFrameSize( CCSize p_tagFrameSize );

	const float GetFrameScale();
	void SetFrameScale( float p_fFrameScale );

	bool IsLandscapeFrame();
	void ChangeFrameOrientation();
	void ChangeFrameOrientationToPortait();
	void ChangeFrameOrientationToLandscape();

	bool IsWelcome();
	void ResetToWelcome();
public:
	void ParseCommandLine( vector<string>& p_vecArgs );
	const string MakeCommandLine();
public:
	void Dump();
private:
	bool			m_bIsWelcome;
	bool			m_bIsShowConsole;
	bool			m_bIsWriteDebugLogToFile;
	 bool			m_bIsLoadPrecompiledFramework;
	string		m_szProjectDir;
	CCSize		m_tagFrameSize;
	float			m_fFrameScale;
};
//--------------------------------------------------------------------
typedef struct _SimulatorScreenSize {
	string m_szTitle;
	int m_nWidth;
	int m_nHeight;

	_SimulatorScreenSize(const string p_szTitle, int p_nWidth, int p_nHeight)
	{
		m_szTitle  = p_szTitle;
		m_nWidth  = p_nWidth;
		m_nHeight = p_nHeight;
	}
} SSimulatorScreenSize;
//--------------------------------------------------------------------
typedef vector<SSimulatorScreenSize> ScreenSizeArray;
typedef ScreenSizeArray::iterator ScreenSizeArrayIterator;
//--------------------------------------------------------------------
#if defined(_WINDOWS)
#define DIRECTORY_SEPARATOR "\\"
#define DIRECTORY_SEPARATOR_CHAR '\\'
#else
#define DIRECTORY_SEPARATOR "/"
#define DIRECTORY_SEPARATOR_CHAR '/'
#endif
//--------------------------------------------------------------------
class CSimulatorConfig
{
public:
	static CSimulatorConfig *GetInstance();

	int GetScreenSizeCount();
	const SSimulatorScreenSize GetScreenSize(int index);
	int CheckScreenSize(const CCSize& size);

	void SetRootPath(const char *path);
	const string GetRootPath();

	const string GetPrecompiledFrameworkPath();

	static void MakeNormalizePath(string *path, const char *directorySeparator = NULL);
private:
	CSimulatorConfig();
private:
	static CSimulatorConfig *s_sharedInstance;
private:
	ScreenSizeArray m_screenSizeArray;
	string m_szRootPath;
};
//--------------------------------------------------------------------
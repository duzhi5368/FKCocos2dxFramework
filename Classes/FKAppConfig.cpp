/********************************************************************
	created:	2014/02/12
	created:	12:2:2014   18:32
	filename: 	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\Classes\FKAppConfig.cpp
	file path:	E:\Workspace\cocos2d_x_215b_cocostudio0400\cocos2d-x_v2.1.5b\cocos2d-x\projects\AppWrapper\Classes
	file base:	FKAppConfig
	file ext:	cpp
	author:		FreeKnight
	
	purpose:	
*********************************************************************/

#include "FKAppConfig.h"

//--------------------------------------------------------------------
CFKAppConfig::CFKAppConfig()
	: m_bIsShowConsole( true )
	, m_bIsWriteDebugLogToFile( true )
	, m_bIsWelcome( true )
{
	SetFrameSize( CCSize(960, 640) );
	SetFrameScale( 1.0f );
	SetLoadPrecompiledFramework( true );
}
//--------------------------------------------------------------------
const bool CFKAppConfig::IsShowConsole()
{
	return m_bIsShowConsole;
}
//--------------------------------------------------------------------
void CFKAppConfig::SetShowConsole( bool p_bIsShowConsole )
{
	m_bIsShowConsole = p_bIsShowConsole;
}
//--------------------------------------------------------------------
const bool CFKAppConfig::IsWriteDebugLogToFile()
{
	return m_bIsWriteDebugLogToFile;
}
//--------------------------------------------------------------------
void CFKAppConfig::SetWriteDebugLogToFile( bool p_IsWriteDebugLogToFile )
{
	m_bIsWriteDebugLogToFile = p_IsWriteDebugLogToFile;
}
//--------------------------------------------------------------------
const string CFKAppConfig::GetDebugLogFilePath()
{
	string path(GetProjectDir());

	struct tm *tm;  
	time_t timep;  
	time(&timep);  
	tm = localtime(&timep);  
	int year = tm->tm_year + 1900;  
	int month = tm->tm_mon + 1;  
	int day = tm->tm_mday;  
	int hour=tm->tm_hour;  
	int minute=tm->tm_min;  
	int second=tm->tm_sec;

	char szFileName[MAX_PATH] = { 0 };
	sprintf( szFileName, "%4d%02d%02d_%02d%02d%02d_FKLog.txt",  year, month, day, hour, minute, second );

	path.append(szFileName);
	return path;
}
//--------------------------------------------------------------------
bool CFKAppConfig::IsLoadPrecompiledFramework()
{
	return m_bIsLoadPrecompiledFramework;
}
//--------------------------------------------------------------------
void CFKAppConfig::SetLoadPrecompiledFramework(bool p_bIsLoad)
{
	m_bIsLoadPrecompiledFramework = p_bIsLoad;
}
//--------------------------------------------------------------------
const string CFKAppConfig::GetProjectDir()
{
	return m_szProjectDir;
}
//--------------------------------------------------------------------
void CFKAppConfig::SetProjectDir( const string p_szProjectDir )
{
	m_szProjectDir = p_szProjectDir;
}
//--------------------------------------------------------------------
const CCSize CFKAppConfig::GetFrameSize()
{
	return m_tagFrameSize;
}
//--------------------------------------------------------------------
void CFKAppConfig::SetFrameSize( CCSize p_tagFrameSize )
{
	if (p_tagFrameSize.width <= 0 || p_tagFrameSize.width > 2048 || p_tagFrameSize.height <= 0 || p_tagFrameSize.height > 2048) return;
	m_tagFrameSize = p_tagFrameSize;
}
//--------------------------------------------------------------------
const float CFKAppConfig::GetFrameScale()
{
	return m_fFrameScale;
}
//--------------------------------------------------------------------
void CFKAppConfig::SetFrameScale( float p_fFrameScale )
{
	CCAssert(p_fFrameScale > 0, "Invalid frameScale");
	m_fFrameScale = p_fFrameScale;
}
//--------------------------------------------------------------------
bool CFKAppConfig::IsLandscapeFrame()
{
	return m_tagFrameSize.width > m_tagFrameSize.height;
}
//--------------------------------------------------------------------
void CFKAppConfig::ChangeFrameOrientation()
{
	float w = m_tagFrameSize.width;
	m_tagFrameSize.width = m_tagFrameSize.height;
	m_tagFrameSize.height = w;
}
//--------------------------------------------------------------------
void CFKAppConfig::ChangeFrameOrientationToPortait()
{
	if (IsLandscapeFrame()) ChangeFrameOrientation();
}
//--------------------------------------------------------------------
void CFKAppConfig::ChangeFrameOrientationToLandscape()
{
	if (!IsLandscapeFrame()) ChangeFrameOrientation();
}
//--------------------------------------------------------------------
bool CFKAppConfig::IsWelcome()
{
	return m_bIsWelcome;
}
//--------------------------------------------------------------------
void CFKAppConfig::ResetToWelcome()
{
	m_bIsWelcome = true;
	m_bIsShowConsole = false;
	m_bIsWriteDebugLogToFile = false;

	SetFrameSize( CCSize(960, 640) );
	SetFrameScale( 1.0f );
}
//--------------------------------------------------------------------
void CFKAppConfig::ParseCommandLine( vector<string>& p_vecArgs )
{
	assert( p_vecArgs.size() >= 1 );

	// get app workdir
	string szAppPath = p_vecArgs[0];
	char szPath[MAX_PATH + 1] = {0};
	strcpy( szPath, szAppPath.c_str() );
	(strrchr(szPath, '\\'))[1] = 0;
	SetProjectDir( szPath );

	for (vector<string>::iterator it = p_vecArgs.begin(); it != p_vecArgs.end(); ++it)
	{
		const string& arg = *it;

		/*		if (arg.compare("-workdir") == 0)
		{
		++it;
		SetProjectDir(*it);
		}
		else*/ if (arg.compare("-size") == 0)
		{
			++it;
			const string& sizeStr(*it);
			size_t pos = sizeStr.find('x');
			int width = 0;
			int height = 0;
			if (pos != sizeStr.npos && pos > 0)
			{
				string widthStr, heightStr;
				widthStr.assign(sizeStr, 0, pos);
				heightStr.assign(sizeStr, pos + 1, sizeStr.length() - pos);
				width = atoi(widthStr.c_str());
				height = atoi(heightStr.c_str());
				SetFrameSize(CCSize(width, height));
			}
		}
		else if (arg.compare("-scale") == 0)
		{
			++it;
			float scale = atof((*it).c_str());
			SetFrameScale(scale);
		}
		else if (arg.compare("-write-debug-log") == 0)
		{
			SetWriteDebugLogToFile(true);
		}
		else if (arg.compare("-disable-write-debug-log") == 0)
		{
			SetWriteDebugLogToFile(false);
		}
		else if (arg.compare("-console") == 0)
		{
			SetShowConsole(true);
		}
		else if (arg.compare("-disable-console") == 0)
		{
			SetShowConsole(false);
		}
		else if (arg.compare("-load-framework") == 0)
		{
			SetLoadPrecompiledFramework(true);
		}
		else if (arg.compare("-disable-load-framework") == 0)
		{
			SetLoadPrecompiledFramework(false);
		}
	}

	Dump();
}
//--------------------------------------------------------------------
const string CFKAppConfig::MakeCommandLine()
{
	stringstream buff;

	//string path = GetProjectDir();
	//if (path.length())
	//{
	//	buff << "-workdir ";
	//	buff << path;
	//}

	buff << " -size ";
	buff << (int)GetFrameSize().width;
	buff << "x";
	buff << (int)GetFrameSize().height;

	if (GetFrameScale() < 1.0f)
	{
		buff << " -scale ";
		buff.precision(2);
		buff << GetFrameScale();
	}

	if (IsWriteDebugLogToFile())
	{
		buff << " -write-debug-log";
	}
	else
	{
		buff << " -disable-write-debug-log";
	}

	if (IsShowConsole())
	{
		buff << " -console";
	}
	else
	{
		buff << " -disable-console";
	}

	if (IsLoadPrecompiledFramework())
	{
		buff << " -load-framework";
	}
	else
	{
		buff << " -disable-load-framework";
	}

	return buff.str();
}
//--------------------------------------------------------------------
void CFKAppConfig::Dump()
{
	FKLOG("----------------------------------------");
	FKLOG("配置信息:");
	FKLOG("----------------------------------------");
	FKLOG("  项目路径:         %s", m_szProjectDir.c_str());
	FKLOG("  屏幕分辨率大小:   %0.0f x %0.0f", m_tagFrameSize.width, m_tagFrameSize.height);
	FKLOG("  屏幕分辨率缩放比: %0.2f", m_fFrameScale);
	FKLOG("  是否显示控制台:   %s", m_bIsShowConsole ? "是" : "否");
	FKLOG("  是否记录调试日志: %s", m_bIsWriteDebugLogToFile ? "是" : "否");
	FKLOG("  是否显示调试信息: %s", m_bIsLoadPrecompiledFramework ? "是" : "否");
	FKLOG("----------------------------------------");
}
//--------------------------------------------------------------------
// CSimulatorConfig
//--------------------------------------------------------------------
CSimulatorConfig *CSimulatorConfig::s_sharedInstance = NULL;
//--------------------------------------------------------------------
CSimulatorConfig *CSimulatorConfig::GetInstance()
{
	if (!s_sharedInstance)
	{
		s_sharedInstance = new CSimulatorConfig();
	}
	return s_sharedInstance;
}
//--------------------------------------------------------------------
CSimulatorConfig::CSimulatorConfig()
{
	m_screenSizeArray.push_back(SSimulatorScreenSize("iPhone 3Gs (320x480)", 320, 480));
	m_screenSizeArray.push_back(SSimulatorScreenSize("iPhone 4 (640x960)", 640, 960));
	m_screenSizeArray.push_back(SSimulatorScreenSize("iPhone 5 (640x1136)", 640, 1136));
	m_screenSizeArray.push_back(SSimulatorScreenSize("iPad (768x1024)", 768, 1024));
	m_screenSizeArray.push_back(SSimulatorScreenSize("iPad Retina (1536x2048)", 1536, 2048));
	m_screenSizeArray.push_back(SSimulatorScreenSize("Android (480x800)", 480, 800));
	m_screenSizeArray.push_back(SSimulatorScreenSize("Android (480x854)", 480, 854));
	m_screenSizeArray.push_back(SSimulatorScreenSize("Android (540x960)", 540, 960));
	m_screenSizeArray.push_back(SSimulatorScreenSize("Android (600x1024)", 600, 1024));
	m_screenSizeArray.push_back(SSimulatorScreenSize("Android (720x1280)", 720, 1280));
	m_screenSizeArray.push_back(SSimulatorScreenSize("Android (800x1280)", 800, 1280));
	m_screenSizeArray.push_back(SSimulatorScreenSize("Android (1080x1920)", 1080, 1920));
}
//--------------------------------------------------------------------
int CSimulatorConfig::GetScreenSizeCount()
{
	return (int)m_screenSizeArray.size();
}
//--------------------------------------------------------------------
const SSimulatorScreenSize CSimulatorConfig::GetScreenSize(int index)
{
	return m_screenSizeArray.at(index);
}
//--------------------------------------------------------------------
int CSimulatorConfig::CheckScreenSize(const CCSize& size)
{
	int width = size.width;
	int height = size.height;

	if (width > height)
	{
		int w = width;
		width = height;
		height = w;
	}

	int count = (int)m_screenSizeArray.size();
	for (int i = 0; i < count; ++i)
	{
		const SSimulatorScreenSize &size = m_screenSizeArray[i];
		if (size.m_nWidth == width && size.m_nHeight == height)
		{
			return i;
		}
	}

	return -1;
}
//--------------------------------------------------------------------
void CSimulatorConfig::SetRootPath(const char *path)
{
	if (path)
	{
		m_szRootPath = path;
		MakeNormalizePath(&m_szRootPath);
		if (m_szRootPath[m_szRootPath.length() - 1] != DIRECTORY_SEPARATOR_CHAR)
		{
			m_szRootPath.append(DIRECTORY_SEPARATOR);
		}
	}
}
//--------------------------------------------------------------------
const string CSimulatorConfig::GetRootPath()
{
	return m_szRootPath;
}
//--------------------------------------------------------------------
// load framework
const string CSimulatorConfig::GetPrecompiledFrameworkPath()
{
	string path = m_szRootPath;
	path.append("lib");
	path.append(DIRECTORY_SEPARATOR);
	path.append("framework_precompiled");
	path.append(DIRECTORY_SEPARATOR);
	path.append("framework_precompiled.zip");
	return path;
}
//--------------------------------------------------------------------
// helper
void CSimulatorConfig::MakeNormalizePath(string *path, const char *directorySeparator/* = NULL*/)
{
	if (!directorySeparator) directorySeparator = DIRECTORY_SEPARATOR;
	size_t pos = std::string::npos;
	while ((pos = path->find_first_of("/\\", pos + 1)) != std::string::npos)
	{
		path->replace(pos, 1, directorySeparator);
	}
}
//--------------------------------------------------------------------
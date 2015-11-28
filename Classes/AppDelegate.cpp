#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
//--------------------------------------------------------------------
USING_NS_CC;
using namespace CocosDenshion;
//--------------------------------------------------------------------
AppDelegate::AppDelegate() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
}
//--------------------------------------------------------------------
AppDelegate::~AppDelegate() 
{
	SimpleAudioEngine::sharedEngine()->end();
}
//--------------------------------------------------------------------
bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    CCDirector* pDirector = CCDirector::sharedDirector();
	pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
	pDirector->setProjection(kCCDirectorProjection2D);
	
    // turn on display FPS
	if( m_tagProjectConfig.IsLoadPrecompiledFramework() )
	{
		pDirector->setDisplayStats(true);
	}
	else
	{
		pDirector->setDisplayStats( false );
	}

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    CCScene *pScene = HelloWorld::scene();

	// some info here
	m_tagProjectConfig.Dump();
	FKLOG("进入游戏场景");
	FKLOG("----------------------------------------");

    // run
    pDirector->runWithScene(pScene);

    return true;
}
//--------------------------------------------------------------------
// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	CCDirector::sharedDirector()->stopAnimation();
	CCDirector::sharedDirector()->pause();
	SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}
//--------------------------------------------------------------------
// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	CCDirector::sharedDirector()->startAnimation();
	CCDirector::sharedDirector()->resume();
	SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}
//--------------------------------------------------------------------
void AppDelegate::SetProjectConfig( const CFKAppConfig& config )
{
	m_tagProjectConfig = config;
}
//--------------------------------------------------------------------
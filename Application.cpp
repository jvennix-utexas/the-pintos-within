#include "TheEngine.h"

#define APPLICATION_NAME "THE EPIC GAME"

void initializeGame()
{
	Ogre::String resourcesCfg;
    Ogre::String pluginsCfg;

    #ifdef _DEBUG
	    resourcesCfg = "resources_d.cfg";
	    pluginsCfg = "plugins_d.cfg";
	#else
	    resourcesCfg = "resources.cfg";
	    pluginsCfg = "plugins.cfg";
	#endif

	GraphicsManager::instance()->initialize(APPLICATION_NAME, pluginsCfg);
    ResourcesManager::instance()->initialize(resourcesCfg, pluginsCfg);
    InputManager::instance()->initialize();
    // GUIManager::instance()->initialize(APPLICATION_NAME);
    PhysicsManager::instance()->initialize();
    SceneManager::instance()->initialize();
    AudioManager::instance()->initialize();
}

void createGameContents()
{

}

void startGame()
{

}


/****************************************************************************
*                                                                           *
*                         Where the game starts                             *
*                                                                           *
*****************************************************************************/

int main(int argc, char *argv[])
{
    initializeGame();
    createGameContents();
    startGame();
    return 0;
}

/****************************************************************************
*                                                                           *
*                                                                           *
*                                                                           *
*****************************************************************************/

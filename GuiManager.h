#ifndef __GuiManager_h_
#define __GuiManager_h_

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>
#include <OISMouse.h>
#include <OgreRoot.h>
#include <OgreFrameListener.h>
#include "Singleton.h"
#include "GraphicsManager.h"
#include "GameState.h"
#include "PlayerCharacter.h"

class Gui;
class GuiManager:public Singleton<GuiManager>{
  public:
    GuiManager();
    virtual ~GuiManager();
    void EnableStart();
    void Update(const Ogre::FrameEvent& event);
    void Initialize(std::string applicationName);
    bool IsExpectingMouse();
    bool IsExpectingKeyboard();
    bool HostGame(const CEGUI::EventArgs& e);
    bool JoinGame(const CEGUI::EventArgs& e);
    bool Exit(const CEGUI::EventArgs& e);
    bool Start(const CEGUI::EventArgs& e);
    bool Connect(const CEGUI::EventArgs& e);
    static CEGUI::MouseButton TranslateButton(OIS::MouseButtonID buttonId);
    void Reinitialize();
  private:
    CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonId);
    CEGUI::OgreRenderer* _renderer;
    bool _isDisplayed;
    Gui* _hud;
    Gui* _mainMenu;
    Gui* _joinGameMenu;
    Gui* _createGameMenu;
    Gui* _lobby;
    Gui* _waitingPrompt;
    Gui* _current;
};
class Gui{
  public:
    Gui();
    void Display();
  protected:
    Gui(std::string layoutFileName);
    CEGUI::Window* _root;
};
class Hud:public Gui{
  public:
    Hud();
    void UpdateHealth(float percentHealth);
    void UpdateFuel(float percentFuel);
    void UpdateAmmoCount(int ammoCount);
    void UpdateMagCount(int magCount);
  private:
    CEGUI::ProgressBar* _healthBar;
    CEGUI::ProgressBar* _fuelBar;
    CEGUI::Window* _ammoCount;
    CEGUI::Window* _magCount;
};
class MainMenu:public Gui{
  public:
    MainMenu();
  private:
    CEGUI::PushButton* _hostGame;
    CEGUI::PushButton* _joinGame;
    CEGUI::PushButton* _exit;
};
class JoinGameMenu:public Gui{
  public:
    JoinGameMenu();
};
class CreateGameMenu:public Gui{
  public:
    CreateGameMenu();
    //const char* ReadHost();
  private:
    CEGUI::Editbox* _name;
    CEGUI::Editbox* _timeLimit;
    CEGUI::Editbox* _maxPlayers;
    CEGUI::Combobox* _teamOrganization;
    CEGUI::Combobox* _gameType;
    CEGUI::PushButton* _continue;
};
class Lobby:public Gui{
  public:
    Lobby();
};
class WaitingPrompt:public Gui{
  public:
    WaitingPrompt();
    void EnableStart();
    void RemoveStart();
  private:
    CEGUI::PushButton* _start;
};
#endif

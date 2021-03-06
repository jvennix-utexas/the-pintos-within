#ifndef __GuiManager_h_
#define __GuiManager_h_

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>
#include <OISMouse.h>
#include <OgreRoot.h>
#include <OgreFrameListener.h>
#include <memory>
#include <vector>
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
    void Start();
    bool IsExpectingMouse();
    bool IsExpectingKeyboard();
    bool CreateGame(const CEGUI::EventArgs& e);
    bool HostGame(const CEGUI::EventArgs& e);
    bool JoinGame(const CEGUI::EventArgs& e);
    bool Exit(const CEGUI::EventArgs& e);
    bool Start(const CEGUI::EventArgs& e);
    bool Back(const CEGUI::EventArgs& e);
    bool Connect(const char* host);
    bool ConnectToNamedHost(const CEGUI::EventArgs& e);
    bool ConnectToSelectedHost(const CEGUI::EventArgs& e);
    static CEGUI::MouseButton TranslateButton(OIS::MouseButtonID buttonId);
    void Reinitialize();
    bool BackToMainMenu(const CEGUI::EventArgs& e);
    void ToggleConsole();
    bool IsConsoleVisible();
    std::string GetName();
    void DisplayMessage(std::string message);
    void HideMessage();
  private:
    CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonId);
    CEGUI::OgreRenderer* _renderer;
    bool _isDisplayed;
    bool _isInitialized;
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
    virtual bool DisplayPrompts(const CEGUI::EventArgs& e);
  protected:
    Gui(std::string layoutFileName);
    CEGUI::Window* _root;
    std::shared_ptr<char> getText(CEGUI::Window* element);
    void displayPrompts(const CEGUI::EventArgs& e,std::vector<std::pair<CEGUI::Editbox*,const char*>> prompts);
    void displayPrompt(const CEGUI::EventArgs& e,std::pair<CEGUI::Editbox*,const char*> prompt);
};
class Hud:public Gui{
  public:
    Hud();
    void UpdateHealth(float percentHealth);
    void UpdateFuel(float percentFuel);
    void UpdateAmmoCount(int ammoCount);
    void UpdateMagCount(int magCount);
    void UpdateWeaponName(int weaponId);
    void UpdateScore(int score,uint32_t gameMode,uint32_t teamMode);
    void UpdateTimeRemaining(int timeRemaining);
    void ToggleConsole();
    bool IsConsoleVisible();
    void SetConsoleText(std::string str);
    void UpdateConsole();
    bool ChatSubmitted(const CEGUI::EventArgs& e);
    void DisplayMessage(std::string message);
    void HideMessage();
  private:
    CEGUI::ProgressBar* _healthBar;
    CEGUI::ProgressBar* _fuelBar;
    CEGUI::Window* _ammoCount;
    CEGUI::Window* _magCount;
    CEGUI::Window* _console;
    CEGUI::Window* _weaponName;
    CEGUI::Window* _score;
    CEGUI::Window* _timeRemaining;
    CEGUI::Window* _message;
    CEGUI::Editbox* _consoleInput;
    CEGUI::MultiLineEditbox* _consoleText;
    int _consoleSize;
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
    const char* ReadSelectedHost();
    const char* ReadNamedHost();
    const char* ReadName();
    bool DisplayPrompts(const CEGUI::EventArgs& e);
    void UpdateGames();
  private:
    CEGUI::Editbox* _name;
    CEGUI::Listbox* _hosts;
    //CEGUI::PushButton* _hostsJoin;
    CEGUI::Editbox* _host;
    CEGUI::PushButton* _hostJoin;
};
class CreateGameMenu:public Gui{
  public:
    CreateGameMenu();
    bool DisplayPrompts(const CEGUI::EventArgs& e);
    const char* ReadName();
    int ReadTeamOrganization();
    int ReadGameType();
    int ReadMap();
    float ReadTimeLimit();
  private:
    CEGUI::Editbox* _name;
    CEGUI::Editbox* _timeLimit;
    CEGUI::Editbox* _maxPlayers;
    CEGUI::Combobox* _teamOrganization;
    CEGUI::Combobox* _gameType;
    CEGUI::Combobox* _map;
    CEGUI::PushButton* _continue;
    int readComboBox(CEGUI::Combobox* box);
};
class Lobby:public Gui{
  public:
    Lobby();
    void EnableStart();
    void DisableStart();
    void AddStart();
    void RemoveStart();
    void UpdatePlayers();
    void SetConsoleText(std::string str);
    void UpdateConsole();
    bool ChatSubmitted(const CEGUI::EventArgs& e);
    void UpdateGameDescription();
  private:
    CEGUI::MultiLineEditbox* _chatBuffer;
    CEGUI::Editbox* _chatInput;
    CEGUI::PushButton* _back;
    CEGUI::PushButton* _start;
    CEGUI::PushButton* _chatSend;
    CEGUI::Listbox* _players;
    CEGUI::Window* _gameDescription;
    int _chatBufferSize;
};
class WaitingPrompt:public Gui{
  public:
    WaitingPrompt();
};
#endif

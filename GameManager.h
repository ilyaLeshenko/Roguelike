#pragma once
#include <SFML/Graphics.hpp>
#include "LevelManager.h"
#include "RoomManager.h"
#include "Player.h"
#include "MainMenu.h"
#include "Lobby.h"
#include "Shop.h"
#include <vector>

class GameManager {
public:
  enum class GameState {
    MAIN_MENU,
    LOBBY,
    INGAME,
    PAUSED,
    SHOP
  };

  static GameManager& getInstance();

  void initialize(sf::RenderWindow& window);
  void update(float deltaTime);
  void render(sf::RenderWindow& window);

  void startGame();
  void startRun();
  void returnToLobby();
  void togglePause();
  void enterShop();

  GameState getCurrentState() const { return currentState; }
  MainMenu getMainMenu() const { return mainMenu; }

private:
  GameManager();  // Теперь объявлен, но не как = default
  ~GameManager() = default;

  void loadNextRoom();
  void drawUI(const sf::View& view);

  GameState currentState = GameState::MAIN_MENU;
  std::unique_ptr<Player> player;
  std::unique_ptr<LevelManager> levelManager;
  std::unique_ptr<RoomManager> roomManager;
  Shop shop;
  bool shopLoaded = false;
  MainMenu mainMenu;
  Lobby lobby;
  sf::Font font;
  sf::Text hudText;
  sf::View currentView;
  sf::Cursor customCursor;
  sf::RenderWindow* windowPtr = nullptr;

  std::vector<std::string> roomList = { "room1.txt", "room2.txt", "room3.txt" };
  size_t currentRoomIndex = 0;
  bool allRoomsCompleted = false;
};
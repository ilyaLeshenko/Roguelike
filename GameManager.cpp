#include "GameManager.h"
#include "PlayerCamera.h"
#include <iostream>
#include "MonsterFactory.h"

GameManager::GameManager()
  : currentState(GameState::MAIN_MENU),
  shopLoaded(false),
  currentRoomIndex(0),
  allRoomsCompleted(false),
  windowPtr(nullptr) {

}

void GameManager::returnToLobby() {
  currentState = GameState::LOBBY;

  player->reset();

  const sf::Vector2f p = lobby.getSpawnPoint();
  player->x = p.x;
  player->y = p.y;
  player->sprite.setPosition(p);

  player->setLevelManager(&lobby.getLevelManager());
  roomManager->init(player.get(), &lobby.getLevelManager());
  if (allRoomsCompleted) {
    std::cout << "All rooms completed! Congratulations!" << std::endl;
    allRoomsCompleted = false;
  }
  roomManager->resetWaves();
  currentRoomIndex = 0;
}

void GameManager::togglePause() {
  if (currentState == GameState::INGAME) {
    currentState = GameState::PAUSED;
    std::cout << "Game PAUSED" << std::endl;
  }
  else if (currentState == GameState::PAUSED) {
    currentState = GameState::INGAME;
    std::cout << "Game RESUMED" << std::endl;
  }
}

GameManager& GameManager::getInstance() {
  static GameManager instance;
  return instance;
}

void GameManager::initialize(sf::RenderWindow& window) {
  windowPtr = &window;
  if (!font.loadFromFile("font/minecraft_0.ttf")) {
    std::cerr << "Failed to load font!" << std::endl;
  }
  sf::Image cursorImage;
  cursorImage.loadFromFile("image/aim.png");
  if (
      !customCursor.loadFromPixels(
          cursorImage.getPixelsPtr(),
          cursorImage.getSize(),
          sf::Vector2u(16, 16)
      )) {
      std::cerr << "Failed to load cursor!" << std::endl;
  }
  window.setMouseCursor(customCursor);
  hudText.setFont(font);
  hudText.setCharacterSize(24);
  hudText.setFillColor(sf::Color::White);

  MonsterFactory::registerType<Slime>("SLIME");
  MonsterFactory::registerType<Ghost>("GHOST");
  MonsterFactory::registerType<Skeleton>("SKELETON");
  lobby.loadResources();
  mainMenu.loadResources();
  shop.loadResources();

  levelManager = std::make_unique<LevelManager>();
  roomManager = std::make_unique<RoomManager>();
  player = std::make_unique<Player>(500, 320, 32, 32, "image/heroTileSet.png", levelManager.get());
  player->setWeapon(std::make_unique<Bow>(50, 10, 200));
  levelManager->loadFromFile("data/lobby.txt");
  levelManager->loadTileset("image/map.png");

  roomManager->init(player.get(), levelManager.get());
  currentView = window.getDefaultView();
  //, ��������, ��������� ����  (x = 600, y = 120, ������ 80, ������ 40)
  lobby.setExitZone({ 380, 0, 130, 160 });

  //, � ��������� ������ � ����� � � ������ ���� ����
  lobby.setSpawnPoint({ 450, 330 });
}

void GameManager::drawUI(const sf::View& /*worldView*/) {
    if (!player || !windowPtr) return;

    // 1. ��������� ������� ��� (������ � worldView)
    sf::View oldView = windowPtr->getView();

    // 2. ��������� �� ����� ����, ������� ������ ��������� � ��������� ������
    windowPtr->setView(windowPtr->getDefaultView());

    // 3. ��������� ����� HUD
    hudText.setString(
        "HP: " + std::to_string(player->getHP()) +
        "  Money: " + std::to_string(player->getMoney())
    );
    hudText.setPosition(600.f, 10.f);   // ������ �����-������� ����
    windowPtr->draw(hudText);
    hudText.setCharacterSize(15);
    hudText.setFillColor(sf::Color::White);
    // 4. ���������� ������ ���, ����� ���������� ��������� ��� � �����
    windowPtr->setView(oldView);
}

void GameManager::update(float deltaTime) {
  if (!windowPtr) return;
  sf::Vector2f mousePos = windowPtr->mapPixelToCoords(sf::Mouse::getPosition(*windowPtr));

  switch (currentState) {
  case GameState::MAIN_MENU: {
    mainMenu.update(mousePos);
    break;
  }
  case GameState::LOBBY:
    player->update(deltaTime);
    lobby.update(deltaTime, *player);

    currentView = PlayerCamera::getViewForPlayer(
      player->getPlayerCoordinateX(),
      player->getPlayerCoordinateY(),
      lobby.getLevelManager(),
      windowPtr->getSize().x,
      windowPtr->getSize().y
    );

    if (lobby.isPlayerInExitZone(player->getPosition()) && sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
      startRun();
    }
    break;

  case GameState::INGAME:
    player->update(deltaTime);
    roomManager->update(deltaTime);

    currentView = PlayerCamera::getViewForPlayer(
      player->getPlayerCoordinateX(),
      player->getPlayerCoordinateY(),
      *levelManager,
      windowPtr->getSize().x,
      windowPtr->getSize().y
    );

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      player->attack(mousePos);
    }

    if (!player->life) {
      returnToLobby();
    }
    else if (roomManager->isRoomCleared() &&
        roomManager->isPlayerNearExit() &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {

        currentRoomIndex++;
        loadNextRoom();
    }
    break;

  case GameState::SHOP:
    shop.update(mousePos, *player);
    if (shop.isExitPressed(mousePos)) {
      currentState = GameState::LOBBY;
    }
    break;

  case GameState::PAUSED:
    break;
  }
}

void GameManager::render(sf::RenderWindow& window) {
  window.clear();
  window.setView(currentView);

  switch (currentState) {
  case GameState::MAIN_MENU:
    mainMenu.draw(window);
    break;

  case GameState::LOBBY:
    lobby.draw(window);
    player->draw(window);
    drawUI(currentView);
    break;

  case GameState::SHOP:
    shop.draw(window);
    drawUI(currentView);
    break;

  case GameState::INGAME:
    levelManager->drawLevel(window);
    player->draw(window);
    roomManager->draw(window);
    player->drawWeapon(window);
    roomManager->drawUI(window, currentView);
    drawUI(currentView);
    break;

  case GameState::PAUSED:
    levelManager->drawLevel(window);
    player->draw(window);
    roomManager->draw(window);
    player->drawWeapon(window);
    roomManager->drawUI(window, currentView);
    drawUI(currentView);

    // ����� ������ ����� ������ �����
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    // ������������� ������� ������������ ������
    overlay.setPosition(currentView.getCenter() - currentView.getSize() / 2.f);
    window.draw(overlay);

    sf::Text text("PAUSED (Press ESC to resume)", font, 40);
    // ���������� ����� ������������ ������
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
      textRect.top + textRect.height / 2.0f);
    text.setPosition(currentView.getCenter());
    window.draw(text);
    break;
  }
}

void GameManager::startGame() {
  player->setLevelManager(&lobby.getLevelManager());
  roomManager->init(player.get(), &lobby.getLevelManager());
  currentState = GameState::LOBBY;
  sf::sleep(sf::milliseconds(100));
  std::cout << "Switched to LOBBY" << std::endl;
}

void GameManager::startRun() {
  currentRoomIndex = 0;
  allRoomsCompleted = false;
  player->setLevelManager(levelManager.get());
  roomManager->init(player.get(), levelManager.get());
  loadNextRoom();
}

void GameManager::loadNextRoom() {
  if (currentRoomIndex >= roomList.size()) {
    allRoomsCompleted = true;
    returnToLobby();
    return;
  }

  if (!levelManager->loadFromFile("data/" + roomList[currentRoomIndex])) {
    std::cerr << "Failed to load room: " << roomList[currentRoomIndex] << std::endl;
    return;
  }

  std::string roomName = roomList[currentRoomIndex];
  roomName = roomName.substr(0, roomName.find_last_of('.'));

  roomManager->loadRoom(roomName);
  roomManager->resetWaves();
  // NEW: ���� �r� � ������ ������ �� ������ �����
  if (auto pos = levelManager->findTile('r')) {
      sf::Vector2f spawn((pos->x + 0.5f) * 32.f, (pos->y + 0.5f) * 32.f);
      player->x = spawn.x;
      player->y = spawn.y;
      player->sprite.setPosition(spawn);
      levelManager->setTile(pos->x, pos->y, '=');   // ���������� �r� � ������� ���
  }

  currentState = GameState::INGAME;
  std::cout << "Loading room: " << roomName << std::endl;
}

void GameManager::enterShop() {
  currentState = GameState::SHOP;
  // ���������� ������� ������ ��� ����� � �������
  player->sprite.setPosition(500, 320);
  if (!shopLoaded) {
    shop.loadResources();
    shopLoaded = true;
  }
}
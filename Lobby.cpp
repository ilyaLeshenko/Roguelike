#include "Lobby.h"
#include <iostream>
#include "GameManager.h"

void Lobby::loadResources(const sf::Font& sharedFont) {
  // ��������� ������� ����� � �������
  if (!levelManager.loadFromFile("data/lobby.txt")) {
    std::cerr << "Failed to load lobby level!" << std::endl;
  }
  if (!levelManager.loadTileset("image/map.png")) {
    std::cerr << "Failed to load lobby tileset!" << std::endl;
  }

  // ������ ������ �Main Menu� � ���������� �������
  returnToMenuButton = std::make_unique<Button>("Main Menu", sharedFont, sf::Vector2f(150.f, 40.f));
  // ��������� ������� ������� �� �����, �� ����� �������� ������ � �������� �����������
  returnToMenuButton->setPosition({ 0.f, 0.f });
  returnToMenuButton->setColors(
    sf::Color::White,
    sf::Color::Transparent,
    sf::Color::Transparent,
    sf::Color(80, 80, 120)
  );

  // ������ ������-�������
  shopObject = std::make_shared<StaticObstacle>(400, 400, 364, 213, "image/shop_prot.png", &levelManager);
  shopObject->setCustomBounds(310.f, 187.f);
  shopObject->setInteractable(true);
  shopObject->setInteractionCallback([]() {
    GameManager::getInstance().enterShop();
    });

  // ��������� ������� ���������
  interactionHint.loadResources();

  // ������������� ���� ������ � Run�
  exitZone = sf::FloatRect(500.f, 0.f, 100.f, 50.f);
}

void Lobby::update(float deltaTime, Player& player) {
  sf::RenderWindow* windowPtr = GameManager::getInstance().getWindowPtr();
  if (!windowPtr) return;

  // �����������: ��� UI-������ ���� �������� ���������� ������� (defaultView)
  sf::Vector2f mouseScreenPos = windowPtr->mapPixelToCoords(
    sf::Mouse::getPosition(*windowPtr),
    windowPtr->getDefaultView()
  );
  returnToMenuButton->update(mouseScreenPos);

  // ���� ������ �Main Menu� � ������������ � ������� ����
  if (returnToMenuButton->isPressed()) {
    GameManager::getInstance().returnToMainMenu();
  }

  // ��������� �������� ������ � ���������
  if (shopObject->checkCollision(&player)) {
    sf::FloatRect playerBounds = player.getGlobalBounds();
    sf::FloatRect shopBounds = shopObject->getGlobalBounds();

    float overlapX = std::min(
      playerBounds.left + playerBounds.width - shopBounds.left,
      shopBounds.left + shopBounds.width - playerBounds.left
    );
    float overlapY = std::min(
      playerBounds.top + playerBounds.height - shopBounds.top,
      shopBounds.top + shopBounds.height - playerBounds.top
    );

    if (overlapX < overlapY) {
      if (playerBounds.left < shopBounds.left) {
        player.x = shopBounds.left - playerBounds.width / 2.f;
      }
      else {
        player.x = shopBounds.left + shopBounds.width + playerBounds.width / 2.f;
      }
    }
    else {
      if (playerBounds.top < shopBounds.top) {
        player.y = shopBounds.top - playerBounds.height / 2.f;
      }
      else {
        player.y = shopBounds.top + shopBounds.height + playerBounds.height / 2.f;
      }
    }
    player.sprite.setPosition(player.x, player.y);
  }

  // ��������� ��������� (InteractionHint)
  bool showHint = false;
  bool canInteract = false;

  if (isPlayerInShopZone(player.getPosition())) {
    showHint = true;
    canInteract = true;
    interactionHint.setCustomText("Enter Shop");
  }
  else if (isPlayerInExitZone(player.getPosition())) {
    showHint = true;
    canInteract = true;
    interactionHint.setCustomText("Start Run");
  }

  interactionHint.setCanInteract(canInteract);
  interactionHint.update(deltaTime, showHint);

  if (showHint && canInteract && sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
    if (isPlayerInExitZone(player.getPosition())) {
      GameManager::getInstance().startRun();
    }
    else if (isPlayerInShopZone(player.getPosition())) {
      shopObject->checkPlayerInteraction(player.getPosition());
    }
  }
}

void Lobby::draw(sf::RenderWindow& window) {
  // 1) ������� ������ ���� (�������, �������, ���������)
  levelManager.drawLevel(window);
  shopObject->draw(window);
  interactionHint.draw(window, window.getView());

  // 2) ������ ������ �Main Menu� � �������� �����������
  // ��������� ������� �������� view
  sf::View prevView = window.getView();

  // ������������� �� ��������� view
  window.setView(window.getDefaultView());

  // ������������� ������� (20px �� ������, 20px �� �����)
  returnToMenuButton->setPosition({ 20.f, 50.f });
  returnToMenuButton->draw(window);

  // ��������������� �������� view
  window.setView(prevView);
}

bool Lobby::isPlayerInShopZone(sf::Vector2f playerPos) const {
  sf::FloatRect bounds = shopObject->getGlobalBounds();
  bounds.left -= SHOP_INTERACTION_RANGE;
  bounds.top -= SHOP_INTERACTION_RANGE;
  bounds.width += 2.f * SHOP_INTERACTION_RANGE;
  bounds.height += 2.f * SHOP_INTERACTION_RANGE;
  return bounds.contains(playerPos);
}

bool Lobby::isPlayerInExitZone(sf::Vector2f playerPos) const {
  return exitZone.contains(playerPos);
}

bool Lobby::isExitToMenuPressed(const sf::Vector2f& mousePos) const {
  return false;
}
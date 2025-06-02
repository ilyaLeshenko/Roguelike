#pragma once
#include <SFML/Graphics.hpp>
#include "LevelManager.h"
#include "Player.h"
#include "InteractionHint.h"
#include "StaticObstacle.h"
#include <memory>

class Lobby {
public:
  void loadResources();
  void update(float deltaTime, Player& player);
  void draw(sf::RenderWindow& window);
  bool isPlayerInShopZone(sf::Vector2f playerPos) const;
  bool isPlayerInExitZone(sf::Vector2f playerPos) const;
  LevelManager& getLevelManager() { return levelManager; }
  const LevelManager & getLevelManager() const { return levelManager; }

  void setExitZone(const sf::FloatRect& zone);     // ���� ��������� ������ ����
  void setSpawnPoint(const sf::Vector2f& point);   // ���� ������� ������ ��� ������ � �����
  sf::Vector2f getSpawnPoint() const { return spawnPoint_; }

private:
  static constexpr float SHOP_PENETRATION = 0.3f;
  sf::Vector2f        spawnPoint_{ 500.f, 620.f };
  static constexpr float SHOP_INTERACTION_RANGE = 50.f;
  static constexpr float EXIT_INTERACTION_RANGE = 30.f;
  InteractionHint interactionHint;
  LevelManager levelManager;
  std::shared_ptr<StaticObstacle> shopObject;
  sf::FloatRect exitZone;
};
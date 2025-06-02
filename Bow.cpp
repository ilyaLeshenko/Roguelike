#include "Bow.h"
#include "Arrow.h"

Bow::Bow(float attackSpeed, float damage, float arrowSpeed)
  : arrowSpeed(arrowSpeed) {
  this->damage = damage;
  this->attackSpeed = attackSpeed;
}

void Bow::attack(const sf::Vector2f& startPos, const sf::Vector2f& targetPos) {
  if (cooldown <= 0.f) {
    arrows.push_back(std::make_shared<Arrow>(startPos, targetPos, arrowSpeed, damage, getOwner(), getOwner()->levelManager));
    cooldown = 30 / attackSpeed;
  }
}

void Bow::update(float deltaTime) {
  cooldown -= deltaTime;

  // ��������� ������ ����� ������
  

  // ������� ������� ������
  arrows.erase(
    std::remove_if(arrows.begin(), arrows.end(),
      [](const auto& arrow) {
        return !arrow->life;
      }),
    arrows.end()
  );
  for (auto& arrow : arrows) {
    if (arrow->life) {
      arrow->update(deltaTime);
    }
  }
}

void Bow::draw(sf::RenderWindow& window) {
  for (auto& arrow : arrows) {
    arrow->draw(window);
  }
}


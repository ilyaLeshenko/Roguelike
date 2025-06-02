#pragma once
#include "Entity.h"
#include "SFML/Graphics.hpp"

class Bone : public Entity {
private:
    sf::Vector2f velocity;     // направление и скорость
    float  damage;             // урон
    float  spawnTimer;         // короткая «невидимость» после выстрела
    float  rotationSpeed;      // скорость вращения (град/с)

    // точка старта и максимальная дистанция полёта
    sf::Vector2f startPosition;
    static constexpr float maxDistance = 450.f;

public:
    Bone(const sf::Vector2f& position,
        const sf::Vector2f& target,
        float speed,          // обычно 180
        float damage,
        Entity* shooter,
        LevelManager* lvlMgr);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const;
    bool isOutOfScreen(const sf::RenderWindow& window) const;
    void explode();

    // только игроку наносим урон; остальные сущности игнорируются
    void onCollision(Entity* other) override;
    bool checkCollision(Entity* other) override;
};

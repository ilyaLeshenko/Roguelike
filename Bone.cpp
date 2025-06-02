#include "Bone.h"
#include "Player.h"
#include <cmath>

Bone::Bone(const sf::Vector2f& position,
    const sf::Vector2f& target,
    float speed,
    float dmg,
    Entity* shooter,
    LevelManager* lvlMgr)
    : damage(dmg),
    Entity(position.x, position.y, 32, 32, "image/bone.png", shooter, lvlMgr),
    rotationSpeed(720.f)        // 2 оборота в секунду
{
    sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
    sprite.setPosition(position);
    usePixelPerfect = true;

    // запоминаем точку старта
    startPosition = position;

    // направление движения
    sf::Vector2f dir = target - position;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len != 0) dir /= len;
    velocity = dir * speed;

    // первый кадр выключаем коллизии, чтобы не задеть скелета-владельца
    canCollide = false;
    spawnTimer = 0.1f;

    // начальный поворот по траектории
    float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159265f + 90.f;
    sprite.setRotation(angle);
}

void Bone::update(float dt)
{
    if (!life) return;

    // «Невидимость» после выстрела
    if (spawnTimer > 0.f) {
        spawnTimer -= dt;
        if (spawnTimer <= 0.f) canCollide = true;
    }

    // проверяем непроходимые тайлы
    if (canCollide) {
        for (int i = y / 32; i <= (y + height - 1) / 32; ++i)
            for (int j = x / 32; j <= (x + width - 1) / 32; ++j)
                if (!levelManager->isShootable(j, i)) {
                    explode();
                    return;
                }
    }

    // движение
    sprite.move(velocity * dt);
    x = sprite.getPosition().x;
    y = sprite.getPosition().y;

    // проверяем, не улетела ли кость дальше 450 пикселей от старта
    {
        float dx = x - startPosition.x;
        float dy = y - startPosition.y;
        if (dx * dx + dy * dy >= maxDistance * maxDistance) {
            explode();
            return;
        }
    }

    // вращение
    sprite.rotate(rotationSpeed * dt);
}


void Bone::draw(sf::RenderWindow& window) const
{
    if (life) window.draw(sprite);
}

bool Bone::isOutOfScreen(const sf::RenderWindow& window) const
{
    sf::Vector2f pos = sprite.getPosition();
    auto size = window.getSize();
    return pos.x < 0 || pos.y < 0 || pos.x > size.x || pos.y > size.y;
}

void Bone::explode()
{
    life = false;
}

void Bone::onCollision(Entity* other)
{
    if (!life || !other || !canCollide) return;
    // повреждаем только игрока
    if (auto* player = dynamic_cast<Player*>(other)) {
        player->takeDamage(static_cast<int>(damage));
        explode();
    }
}

bool Bone::checkCollision(Entity* other)
{
    // сталкиваемся лишь с игроком; игнорируем остальных
    if (!dynamic_cast<Player*>(other)) return false;
    if (AABBCollision(other)) {
        return usePixelPerfect ? pixelPerfectCollision(other) : true;
    }
    return false;
}

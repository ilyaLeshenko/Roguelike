#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <optional>

/*
    обозначения тайлов  
       '0' - стена
       'w' - вода (непроходимый тайл)
       ' ' - трава (базовый проходимый тайл)
       '=' - тропинка (проходимая)
       'f' - цветы (проходимые)
       's' - сокровище
       'l' - ловушка
 */

class LevelManager {
private:

  std::vector<sf::Vector2i> exitBlocks_;   // где в карте стояли символы 'z'

  std::vector<std::string> levelData_;

  int width_ = 0;
  int height_ = 0;

  sf::Texture tileset_;
  std::unordered_map<char, sf::IntRect> tileTextures_;
  sf::IntRect getPathVariant(char tileType, int x, int y) const;

  std::unordered_map<char, std::vector<sf::IntRect>> randomTileVariants_;

  sf::IntRect getRandomTileVariant(char tileType) const;


public:
  
  /* Позволит RoomManager-у узнать, где стоят выходы (‘z’) */
  const std::vector<sf::Vector2i>& getExitBlocks() const { return exitBlocks_; }

  /* вызвать, когда комната зачищена */
  void unlockExits();

  std::optional<sf::Vector2i> findTile(char tile) const;
  explicit LevelManager(const std::vector<std::string>& levelData = {});
  bool loadFromFile(const std::string& filePath);

  char getTile(int x, int y) const;
  void setTile(int x, int y, char tile);

  bool loadTileset(const std::string& path); 
  void drawLevel(sf::RenderWindow& window) const;

  bool isWall(int x, int y)const;
  bool isWalkable(int x, int y) const;
  bool isShootable(int x, int y) const;
  // Проверяет, можно ли спавнить монстра на клетке
  bool isSpawnable(int x, int y) const {
    return isWalkable(x, y);
  }

  // Находит случайную свободную клетку в радиусе
  std::pair<int, int> findNearbyWalkable(int x, int y, int radius = 1) const;

  int getWidth() const { return width_; }
  int getHeight() const { return height_; }



};
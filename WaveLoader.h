#pragma once
#include <vector>
#include <string>

struct MonsterWave {
  int x, y;           // ���������� (2,1)
  std::string type;    // ��� ("SLIME")
};

class WaveLoader {
public:
  static std::vector<MonsterWave> loadFromFile(const std::string& filePath);
};
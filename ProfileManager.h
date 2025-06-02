#pragma once
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>

struct Profile {
  std::string name;
  int money;
  int maxHP;
  int strength;  // ����� ���������������� ��� ��������������� ����
  float speed;   // ��������� �������� (1.0 = �������)
};

class ProfileManager {
public:
  ProfileManager(const std::string& filename);

  // ��������� ��� ������� �� ����� � ������ profiles
  void loadProfiles();

  // ��������� ������� ������ profiles � ���� (����������)
  void saveProfiles() const;

  // ���������� ����������� ������ �� ��� �������
  const std::vector<Profile>& getProfiles() const;

  // �������� ����� ������� (�� ��������� ����� � ����!)
  void addProfile(const Profile& p);

  // ����� ������� �� �����
  std::optional<Profile> getProfile(const std::string& name) const;

private:
  std::string filename;
  std::vector<Profile> profiles;
};

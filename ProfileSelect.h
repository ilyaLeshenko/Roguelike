#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Button.h"
#include "ProfileManager.h"

class ProfileSelect {
public:
  ProfileSelect(sf::Font& font, ProfileManager& manager);

  // ��������� (hover) ������
  void handleHover(const sf::Vector2f& mousePos);

  // ��������� ����� ����
  // ����������:
  // � ��� �������, ���� �������� �� ��������������� ������;
  // � "__NEW__", ���� �������� New Profile;
  // � "__BACK__", ���� �������� Back;
  // � "" � ��������� ������.
  std::string handleClick(const sf::Vector2f& mousePos);

  // ��������� ���������: delta>0 � �����, delta<0 � ����
  void handleScroll(float delta);

  // ����������� ������ ������ � ������ ������ ������� ����
  void rebuildButtons(const sf::Vector2u& windowSize);

  // ������ �����
  void draw(sf::RenderWindow& window);

private:
  sf::Font& fontRef;
  ProfileManager& profileManager;

  std::vector<std::unique_ptr<Button>> profileButtons; // ������ ������������ ��������
  std::unique_ptr<Button> newProfileButton;            // ������ "New Profile"
  std::unique_ptr<Button> backButton;                  // ������ "Back"

  sf::Vector2u windowSize; // ������� ����

  float scrollOffset = 0.f; // ������������ �������� (���������)
  float minScroll = 0.f;
  float maxScroll = 0.f;

  // ��������� ������������
  const float buttonWidth = 300.f;
  const float buttonHeight = 50.f;
  const float spacing = 20.f;    // ���������� ����� ��������
  const float topMargin = 100.f; // ������ ������ ����� ������ ��������

  // ������������� minScroll/maxScroll � ������������ scrollOffset
  void updateScrollLimits();
};

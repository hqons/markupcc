#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>

struct Style
{
  sf::Color backgroundColor = sf::Color(255, 255, 255, 0);
  sf::Color textColor = sf::Color::Black;
  unsigned int fontSize = 18;
  float padding = 5;
  float borderRadius = 4;
  sf::Color borderColor = sf::Color(0, 0, 0, 100);
  float borderThickness = 1.0f;
  std::string gridTemplateColumns = "";
  std::string gridTemplateRows = "";
  float gridColumnGap = 5.0f;
  float gridRowGap = 5.0f;
  std::string justifyItems = "start";
  std::string alignItems = "start";
};
void parse_css_style(const std::string &cssText);
sf::Color parse_css_color(const std::string &val);
extern std::unordered_map<std::string, Style> styleSheet;
extern int windowWidth;
extern int windowHeight;

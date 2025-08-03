#pragma once
#include "style.h"
struct Paragraph
{
  std::string text;
  sf::Text sfText;
  sf::RectangleShape background;

  std::string id;
  std::string className;
  float x, y;
  float width;
  float height;

  Paragraph(const std::string &t, const sf::Font &font,
            unsigned int passedFontSize = 16, float maxWidth = 600.f,
            const std::string &_id = "", const std::string &_class = "")
      : text(t), id(_id), className(_class), width(maxWidth)
  {
    Style style = getStyle();

    if (style.fontSize == Style().fontSize)
    {
      style.fontSize = passedFontSize;
    }

    sfText.setFont(font);
    sfText.setCharacterSize(style.fontSize);
    sfText.setFillColor(style.textColor);
    sfText.setString(wrapText(t, font, style.fontSize, maxWidth));

    background.setFillColor(style.backgroundColor);
  }
  void setText(const std::string &newText)
  {
    text = newText;
    sfText.setString(
        wrapText(text, *sfText.getFont(), sfText.getCharacterSize(), width));
  }

  Style getStyle(bool hover = false) const
  {
    std::string selector = "#" + id;
    if (hover && styleSheet.count(selector + ":hover"))
      return styleSheet[selector + ":hover"];
    if (styleSheet.count(selector))
      return styleSheet[selector];

    selector = "." + className;
    if (hover && styleSheet.count(selector + ":hover"))
      return styleSheet[selector + ":hover"];
    if (styleSheet.count(selector))
      return styleSheet[selector];

    selector = "p";
    if (hover && styleSheet.count("p:hover"))
      return styleSheet["p:hover"];
    if (styleSheet.count("p"))
      return styleSheet["p"];

    return Style(); // 默认
  }

  void setPosition(float px, float py)
  {
    x = px;
    y = py;
    sfText.setPosition(px, py);
  }

  void draw(sf::RenderWindow &window)
  {
    Style style = getStyle(isHovered(window));
    sfText.setFillColor(style.textColor);

    // 背景框
    height = getHeight();
    background.setSize({width, height});
    background.setFillColor(style.backgroundColor);
    background.setPosition(x, y);

    window.draw(background);
    window.draw(sfText);
  }

  float getHeight() const
  {
    return sfText.getGlobalBounds().height + 10;
  }

  bool isHovered(const sf::RenderWindow &window) const
  {
    auto mousePos = sf::Mouse::getPosition(window);
    return mousePos.x >= x && mousePos.x <= x + width && mousePos.y >= y &&
           mousePos.y <= y + getHeight();
  }

  static std::string wrapText(const std::string &text, const sf::Font &font,
                              unsigned int fontSize, float maxWidth)
  {
    std::string result;
    std::string currentLine;
    sf::Text measureText;
    measureText.setFont(font);
    measureText.setCharacterSize(fontSize);

    for (char c : text)
    {
      currentLine += c;
      measureText.setString(currentLine);
      if (measureText.getLocalBounds().width > maxWidth)
      {
        currentLine.pop_back();
        result += currentLine + '\n';
        currentLine = c;
      }
    }

    result += currentLine;
    return result;
  }
};
#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include "style.h"
struct Button
{
  sf::RectangleShape rect;
  sf::Text label;
  float width = 200;
  float height = 40;
  std::string id;
  std::string className;
  float x, y;
  std::function<void()> onClick = nullptr;

  Button(const std::string &text, const sf::Font &font, float px, float py,
         const std::string &_id = "", const std::string &_class = "")
      : id(_id), className(_class), x(px), y(py)
  {
    Style style = getStyle();

    if (style.fontSize == Style().fontSize)
    {
      style.fontSize = 18;
    }

    label.setFont(font);
    label.setCharacterSize(style.fontSize);
    label.setString(text);
    label.setFillColor(style.textColor);

    // 自动根据内容调整宽度（加一点 padding）
    float textWidth = label.getLocalBounds().width;
    width = textWidth + style.padding * 2;

    // 限制最大宽度为 maxWidth（避免太宽）
    float maxButtonWidth = windowWidth * 0.8f; // 可调比例
    if (width > maxButtonWidth)
      width = maxButtonWidth;

    height = style.fontSize + style.padding * 2;
    rect.setSize({width, height});
  }
  void setText(const std::string &text)
  {
    label.setString(text);

    // 更新宽度和位置
    float textWidth = label.getLocalBounds().width;
    width = textWidth + getStyle().padding * 2;

    // 限制最大宽度
    float maxButtonWidth = windowWidth * 0.8f;
    if (width > maxButtonWidth)
      width = maxButtonWidth;
  }

  bool pressed = false;

  void handleEvent(const sf::Event &event, const sf::RenderWindow &window)
  {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left && isHovered(window))
    {
      pressed = true;
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left && isHovered(window) &&
        pressed)
    {
      if (onClick)
        onClick();
      pressed = false;
    }

    // 鼠标释放但没命中按钮区域也重置
    if (event.type == sf::Event::MouseButtonReleased)
    {
      pressed = false;
    }
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

    selector = "button";
    if (hover && styleSheet.count("button:hover"))
      return styleSheet["button:hover"];
    if (styleSheet.count("button"))
      return styleSheet["button"];

    return Style(); // 默认
  }
  void setPosition(float px, float py)
  {
    x = px;
    y = py;
  }

  void draw(sf::RenderWindow &window)
  {
    bool hover = isHovered(window);
    Style style = getStyle(hover);

    rect.setSize({width, height});
    rect.setPosition(x, y);
    rect.setFillColor(style.backgroundColor);
    rect.setOutlineColor(style.borderColor);
    rect.setOutlineThickness(style.borderThickness);

    label.setCharacterSize(style.fontSize);
    label.setFillColor(style.textColor);

    sf::FloatRect textBounds = label.getLocalBounds();
    float textX = x + (width - textBounds.width) / 2.f - textBounds.left;
    float textY = y + (height - textBounds.height) / 2.f - textBounds.top;
    label.setPosition(textX, textY);

    window.draw(rect);
    window.draw(label);
  }

  bool isHovered(const sf::RenderWindow &window) const
  {
    auto mousePos = sf::Mouse::getPosition(window);
    return mousePos.x >= x && mousePos.x <= x + width && mousePos.y >= y &&
           mousePos.y <= y + height;
  }

  float getHeight() const
  {
    return height + 10;
  }
};

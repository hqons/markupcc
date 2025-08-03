#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include "grid.h"
#include "paragraph.h"
#include "style.h"
#include "button.h"
extern float scrollOffset;
extern float maxScrollOffset;
extern const float SCROLL_SPEED;
extern bool isScrolling;
extern sf::RectangleShape scrollBar;

// 简单容器元素
struct Div
{
  std::vector<Element> elements;

  std::string id;
  std::string className;

  std::vector<Div> children; // 允许嵌套 Div
  float x, y;
  float maxWidth;
  float scrollDragStartY = 0.f;      // 添加这个成员变量
  float scrollDragStartOffset = 0.f; // 添加这个成员变量

  Div(float px, float py, const std::string &_id = "",
      const std::string &_class = "")
      : x(px), y(py), id(_id), className(_class)
  {
    maxWidth = windowWidth - 2 * px;
  }

  Paragraph *addParagraph(const std::string &text, const sf::Font &font,
                          unsigned fontSize = 16, const std::string &id = "",
                          const std::string &className = "")
  {
    Paragraph *p = new Paragraph(text, font, fontSize, maxWidth, id, className);
    elements.emplace_back(p);
    return p;
  }

  Button *addButton(const std::string &text, const sf::Font &font,
                    const std::string &id = "", const std::string &className = "")
  {
    Button *b = new Button(text, font, 0, 0, id, className);
    elements.emplace_back(b);
    return b;
  }

  Grid *addGrid(float width, float height,
                const std::string &id = "", const std::string &className = "")
  {
    Grid *g = new Grid(0, 0, width, height, id, className);
    elements.emplace_back(g);
    return g;
  }

  void addChild(Div &&child)
  {
    children.push_back(std::move(child));
  }
  Element *getElementById(const std::string &searchId)
  {
    for (auto &elem : elements)
    {
      if (elem.getId() == searchId)
        return &elem;
    }

    for (auto &child : children)
    {
      Element *found = child.getElementById(searchId);
      if (found)
        return found;
    }

    return nullptr;
  }
  void draw(sf::RenderWindow &window)
  {
    // 创建视图(viewport)来实现滚动效果
    sf::View view = window.getDefaultView();
    sf::FloatRect visibleArea(x, y, maxWidth, windowHeight - y); // 移除了scrollOffset
    view.setViewport(sf::FloatRect(0, 0, 1, 1));
    view.reset(visibleArea);
    window.setView(view);

    float currentY = y - scrollOffset; // 改为在这里应用滚动偏移

    Style style = getStyle(isHovered(window));

    sf::RectangleShape bg;
    float totalHeight = getTotalHeight();
    bg.setPosition(x, currentY); // 使用调整后的currentY
    bg.setSize({maxWidth, totalHeight});
    bg.setFillColor(style.backgroundColor);
    window.draw(bg);

    for (auto &elem : elements)
    {
      if (elem.type == ElementType::Paragraph)
      {
        elem.paragraph->setPosition(x, currentY);
        elem.paragraph->draw(window);
        currentY += elem.paragraph->getHeight();
      }
      else if (elem.type == ElementType::Button)
      {
        elem.button->setPosition(x, currentY);
        elem.button->draw(window);
        currentY += elem.button->getHeight();
      }
      else if (elem.type == ElementType::Grid)
      {
        elem.grid->setPosition(x, currentY);
        elem.grid->draw(window);
        currentY += elem.grid->getHeight();
      }
    }

    for (auto &child : children)
    {
      child.x = x + 10;
      child.y = currentY;
      child.draw(window);
      currentY += child.getTotalHeight();
    }

    // 恢复默认视图
    window.setView(window.getDefaultView());

    // 绘制滚动条
    drawScrollBar(window, totalHeight);
  }

  void drawScrollBar(sf::RenderWindow &window, float totalHeight)
  {
    // 只有内容超出可视区域时才显示滚动条
    if (totalHeight <= windowHeight - y)
    {
      scrollOffset = 0.f;
      return;
    }

    // 计算滚动条参数
    float scrollBarWidth = 10.f;
    float scrollBarX = windowWidth - scrollBarWidth - 5.f;
    float scrollTrackHeight = windowHeight - y;

    // 滚动条轨道
    sf::RectangleShape scrollTrack(sf::Vector2f(scrollBarWidth, scrollTrackHeight));
    scrollTrack.setPosition(scrollBarX, y);
    scrollTrack.setFillColor(sf::Color(200, 200, 200));
    window.draw(scrollTrack);

    // 滚动条滑块
    float visibleRatio = (windowHeight - y) / totalHeight;
    float scrollThumbHeight = scrollTrackHeight * visibleRatio;
    float scrollThumbY = y + (scrollOffset / totalHeight) * scrollTrackHeight;

    scrollBar.setSize(sf::Vector2f(scrollBarWidth, scrollThumbHeight));
    scrollBar.setPosition(scrollBarX, scrollThumbY);
    scrollBar.setFillColor(isScrolling ? sf::Color(100, 100, 100) : sf::Color(150, 150, 150));
    window.draw(scrollBar);
  }

  float getTotalHeight() const
  {
    float h = 0;
    for (const auto &elem : elements)
    {
      if (elem.type == ElementType::Paragraph)
        h += elem.paragraph->getHeight();
      else if (elem.type == ElementType::Button)
        h += elem.button->getHeight();
    }

    for (const auto &div : children)
      h += div.getTotalHeight();

    return h;
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

    selector = "div";
    if (hover && styleSheet.count("div:hover"))
      return styleSheet["div:hover"];
    if (styleSheet.count("div"))
      return styleSheet["div"];

    return Style(); // 默认样式
  }
  void handleEvent(const sf::Event &event, const sf::RenderWindow &window)
  {
    for (auto &elem : elements)
    {
      if (elem.type == ElementType::Button)
      {
        elem.button->handleEvent(event, window);
      }
    }

    for (auto &child : children)
    {
      child.handleEvent(event, window);
    }
  }
  void handleScrollEvent(const sf::Event &event, const sf::RenderWindow &window)
  {
    if (event.type == sf::Event::MouseWheelScrolled)
    {
      scrollOffset -= event.mouseWheelScroll.delta * SCROLL_SPEED;
      clampScrollOffset();
    }
    else if (event.type == sf::Event::MouseButtonPressed)
    {
      if (event.mouseButton.button == sf::Mouse::Left &&
          scrollBar.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
      {
        isScrolling = true;
        scrollDragStartY = event.mouseButton.y;
        scrollDragStartOffset = scrollOffset;
      }
    }
    else if (event.type == sf::Event::MouseButtonReleased)
    {
      if (event.mouseButton.button == sf::Mouse::Left)
      {
        isScrolling = false;
      }
    }
    else if (event.type == sf::Event::MouseMoved && isScrolling)
    {
      float totalHeight = getTotalHeight();
      float deltaY = event.mouseMove.y - scrollDragStartY;

      // 计算新的滚动偏移，基于初始偏移和鼠标移动距离
      scrollOffset = scrollDragStartOffset + (deltaY / (windowHeight - y)) * totalHeight;
      clampScrollOffset();
    }
  }

  void clampScrollOffset()
  {
    float totalHeight = getTotalHeight();
    float maxVisibleHeight = windowHeight - y;

    maxScrollOffset = std::max(0.f, totalHeight - maxVisibleHeight);

    // 直接限制在边界内
    scrollOffset = std::clamp(scrollOffset, 0.f, maxScrollOffset);

    const float SCROLL_EDGE_DAMPING = 0.3f; // 边界阻尼系数
    if (scrollOffset < SCROLL_SPEED)
    {
      scrollOffset *= SCROLL_EDGE_DAMPING;
    }
    else if (scrollOffset > maxScrollOffset - SCROLL_SPEED)
    {
      float overshoot = scrollOffset - (maxScrollOffset - SCROLL_SPEED);
      scrollOffset = maxScrollOffset - overshoot * SCROLL_EDGE_DAMPING;
    }
  }

  bool isHovered(const sf::RenderWindow &window) const
  {
    auto mousePos = sf::Mouse::getPosition(window);
    return mousePos.x >= x && mousePos.x <= x + maxWidth && mousePos.y >= y &&
           mousePos.y <= y + getTotalHeight();
  }
};

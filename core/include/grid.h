#pragma once
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include "element.h"
#include "style.h"
#include "paragraph.h"
#include "button.h"
// 定义 Grid 结构
struct Grid
{

  std::vector<std::vector<Element>> cells;
  std::string id;
  std::string className;
  float x, y;
  float width, height;
  std::vector<float> columnWidths;
  std::vector<float> rowHeights;
  float spacing = 5.f;
  int currentRow = 0;
  int currentCol = 0;

  Grid(float px, float py, float w, float h,
       const std::string &_id = "", const std::string &_class = "")
      : x(px), y(py), width(w), height(h), id(_id), className(_class) {
        Style style = getStyle();       // 获取对应的样式（id/class/grid）
      applyStyle(style);              // 应用样式

      }

  void setDimensions(int cols, int rows)
  {
    columnWidths.resize(cols, width / cols);
    rowHeights.resize(rows, height / rows);
  }
  void setPosition(float px, float py)
  {
    x = px;
    y = py;
  }
  void applyStyle(const Style &style)
  {
    // 解析列模板
    if (!style.gridTemplateColumns.empty())
    {
      parseTemplate(style.gridTemplateColumns, columnWidths, width);
    }

    // 解析行模板
    if (!style.gridTemplateRows.empty())
    {
      parseTemplate(style.gridTemplateRows, rowHeights, height);
    }

    // 设置间距
    spacing = style.gridColumnGap; // 或者可以分别处理行和列间距
  }

  void setColumnWidth(int col, float w)
  {
    if (col >= 0 && col < columnWidths.size())
    {
      columnWidths[col] = w;
    }
  }

  void setRowHeight(int row, float h)
  {
    if (row >= 0 && row < rowHeights.size())
    {
      rowHeights[row] = h;
    }
  }

  void addElement(int col, int row, Element &&element)
  {
    if (row >= cells.size())
    {
      cells.resize(row + 1);
    }
    if (col >= cells[row].size())
    {
      cells[row].resize(col + 1);
    }
    cells[row][col] = std::move(element);
  }
  void addChild(Element &&element)
  {
    // 确保尺寸正确
    if (rowHeights.empty() || columnWidths.empty())
    {
      setDimensions(1, 1); // 默认 1x1 网格
    }

    // 自动扩展 grid 行
    if (currentRow >= cells.size())
    {
      cells.resize(currentRow + 1);
    }

    // 自动扩展列
    if (currentCol >= cells[currentRow].size())
    {
      cells[currentRow].resize(columnWidths.size());
    }

    // 放入元素
    cells[currentRow][currentCol] = std::move(element);

    // 递增列号
    currentCol++;
    if (currentCol >= columnWidths.size())
    {
      currentCol = 0;
      currentRow++;
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

    selector = "grid";
    if (hover && styleSheet.count("grid:hover"))
      return styleSheet["grid:hover"];
    if (styleSheet.count("grid"))
      return styleSheet["grid"];

    return Style(); // 默认
  }

  void draw(sf::RenderWindow &window)
  {
    Style style = getStyle(isHovered(window));

    sf::RectangleShape background(sf::Vector2f(width, height));
    background.setPosition(x, y);
    background.setFillColor(style.backgroundColor);
    window.draw(background);
    applyStyle(style);

    float currentY = y;
    for (size_t row = 0; row < cells.size(); ++row)
    {
      float currentX = x;

      for (size_t col = 0; col < cells[row].size(); ++col)
      {
        
        if (col < columnWidths.size() && row < rowHeights.size())
        {
          Element &element = cells[row][col];

          if (element.type == ElementType::Paragraph)
          {
            element.paragraph->setPosition(currentX, currentY);
            element.paragraph->width = columnWidths[col] - spacing * 2;
            element.paragraph->draw(window);
          }
          else if (element.type == ElementType::Button)
          {
            element.button->setPosition(currentX, currentY);
            element.button->width = columnWidths[col] - spacing * 2;
            element.button->draw(window);
          }

          currentX += columnWidths[col] + spacing;
        }
      }

      currentY += (row < rowHeights.size() ? rowHeights[row] : height / cells.size()) + spacing;
    }
  }

  bool isHovered(const sf::RenderWindow &window) const
  {
    auto mousePos = sf::Mouse::getPosition(window);
    return mousePos.x >= x && mousePos.x <= x + width &&
           mousePos.y >= y && mousePos.y <= y + height;
  }

  float getHeight() const
  {
    float total = 0;
    for (float h : rowHeights)
    {
      total += h + spacing;
    }
    return total - spacing; // 减去最后一个多余的spacing
  }
  void parseTemplate(const std::string &templateStr, std::vector<float> &sizes, float totalSize)
{
    sizes.clear();
    std::istringstream iss(templateStr);
    std::string token;

    while (std::getline(iss, token, ' '))
    {
        if (token.empty()) continue;

        try {
            if (token == "auto")
            {
                sizes.push_back(-1.0f);  // auto 标记
            }
            else if (token.find("fr") != std::string::npos)
            {
                std::string number = token.substr(0, token.find("fr"));
                float fr = std::stof(number);
                sizes.push_back(-fr);  // 用负值表示 fr
            }
            else if (token.find("px") != std::string::npos)
            {
                std::string number = token.substr(0, token.find("px"));
                float px = std::stof(number);
                sizes.push_back(px);
            }
            else
            {
                // 尝试直接当作数字
                float px = std::stof(token);
                sizes.push_back(px);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "[CSS ERROR] parseTemplate failed on token: \"" << token << "\" — " << e.what() << "\n";
            // 可选择跳过或默认宽度
            sizes.push_back(0.f);
        }
    }

    // 计算自动和分数单位的值
    calculateSizes(sizes, totalSize);
}


  void calculateSizes(std::vector<float> &sizes, float totalSize)
  {
    float fixedSize = 0.0f;
    int autoCount = 0;
    float frTotal = 0.0f;

    // 第一遍：计算固定尺寸和统计自动/分数单位
    for (float size : sizes)
    {
      if (size >= 0)
      {
        fixedSize += size;
      }
      else if (size == -1.0f)
      {
        autoCount++;
      }
      else
      {
        frTotal += -size; // 分数单位存储为负值
      }
    }

    // 计算剩余空间
    float remainingSize = totalSize - fixedSize - (spacing * (sizes.size() - 1));

    // 第二遍：设置自动和分数单位的值
    for (float &size : sizes)
    {
      if (size == -1.0f)
      {
        // 自动分配剩余空间给auto项
        size = remainingSize / autoCount;
      }
      else if (size < -1.0f)
      {
        // 按比例分配剩余空间给fr项
        size = (-size / frTotal) * remainingSize;
      }
    }
  }
};

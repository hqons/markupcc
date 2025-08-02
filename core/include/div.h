#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

int windowWidth;
int windowHeight;

enum class ElementType
{
  Paragraph,
  Button
};

struct Style
{
  sf::Color backgroundColor = sf::Color(255, 255, 255, 0);
  sf::Color textColor = sf::Color::Black;
  unsigned int fontSize = 18;
  float padding = 5;
  float borderRadius = 4;

  sf::Color borderColor = sf::Color(0, 0, 0, 100);
  float borderThickness = 1.0f;
};

std::unordered_map<std::string, Style> styleSheet;
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

  Paragraph(const std::string& t, const sf::Font& font,
            unsigned int passedFontSize = 16, float maxWidth = 600.f,
            const std::string& _id = "", const std::string& _class = "")
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
  void setText(const std::string& newText)
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
    if (styleSheet.count(selector)) return styleSheet[selector];

    selector = "." + className;
    if (hover && styleSheet.count(selector + ":hover"))
      return styleSheet[selector + ":hover"];
    if (styleSheet.count(selector)) return styleSheet[selector];

    selector = "p";
    if (hover && styleSheet.count("p:hover")) return styleSheet["p:hover"];
    if (styleSheet.count("p")) return styleSheet["p"];

    return Style();  // 默认
  }

  void setPosition(float px, float py)
  {
    x = px;
    y = py;
    sfText.setPosition(px, py);
  }

  void draw(sf::RenderWindow& window)
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

  bool isHovered(const sf::RenderWindow& window) const
  {
    auto mousePos = sf::Mouse::getPosition(window);
    return mousePos.x >= x && mousePos.x <= x + width && mousePos.y >= y &&
           mousePos.y <= y + getHeight();
  }

  static std::string wrapText(const std::string& text, const sf::Font& font,
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

  Button(const std::string& text, const sf::Font& font, float px, float py,
         const std::string& _id = "", const std::string& _class = "")
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
    float maxButtonWidth = windowWidth * 0.8f;  // 可调比例
    if (width > maxButtonWidth) width = maxButtonWidth;

    height = style.fontSize + style.padding * 2;
    rect.setSize({width, height});
  }
  void setText(const std::string& text)
  {
    label.setString(text);

    // 更新宽度和位置
    float textWidth = label.getLocalBounds().width;
    width = textWidth + getStyle().padding * 2;

    // 限制最大宽度
    float maxButtonWidth = windowWidth * 0.8f;
    if (width > maxButtonWidth) width = maxButtonWidth;
  }

  bool pressed = false;

  void handleEvent(const sf::Event& event, const sf::RenderWindow& window)
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
      if (onClick) onClick();
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
    if (styleSheet.count(selector)) return styleSheet[selector];

    selector = "." + className;
    if (hover && styleSheet.count(selector + ":hover"))
      return styleSheet[selector + ":hover"];
    if (styleSheet.count(selector)) return styleSheet[selector];

    selector = "button";
    if (hover && styleSheet.count("button:hover"))
      return styleSheet["button:hover"];
    if (styleSheet.count("button")) return styleSheet["button"];

    return Style();  // 默认
  }
  void setPosition(float px, float py)
  {
    x = px;
    y = py;
  }

  void draw(sf::RenderWindow& window)
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

  bool isHovered(const sf::RenderWindow& window) const
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
struct Element
{
  ElementType type;
  union
  {
    Paragraph* paragraph;
    Button* button;
  };

  Element(Paragraph* p) : type(ElementType::Paragraph), paragraph(p) {}
  Element(Button* b) : type(ElementType::Button), button(b) {}

  // 支持移动（为了 vector 能扩容）
  Element(Element&& other) noexcept : type(other.type)
  {
    if (type == ElementType::Paragraph)
    {
      paragraph = other.paragraph;
      other.paragraph = nullptr;
    }
    else
    {
      button = other.button;
      other.button = nullptr;
    }
  }
  std::string getId() const
  {
    if (type == ElementType::Paragraph && paragraph) return paragraph->id;
    if (type == ElementType::Button && button) return button->id;
    return "";
  }

  Element& operator=(Element&& other) noexcept
  {
    if (this != &other)
    {
      // 先释放自己
      this->~Element();

      type = other.type;
      if (type == ElementType::Paragraph)
      {
        paragraph = other.paragraph;
        other.paragraph = nullptr;
      }
      else
      {
        button = other.button;
        other.button = nullptr;
      }
    }
    return *this;
  }

  ~Element()
  {
    if (type == ElementType::Paragraph)
      delete paragraph;
    else if (type == ElementType::Button)
      delete button;
  }

  Element(const Element&) = delete;
  Element& operator=(const Element&) = delete;
};

// 简单容器元素
struct Div
{
  std::vector<Element> elements;

  std::string id;
  std::string className;

  std::vector<Div> children;  // 允许嵌套 Div
  float x, y;
  float maxWidth;

  Div(float px, float py, const std::string& _id = "",
      const std::string& _class = "")
      : x(px), y(py), id(_id), className(_class)
  {
    maxWidth = windowWidth - 2 * px;
  }

  void addParagraph(const std::string& text, const sf::Font& font,
                    unsigned fontSize = 16, const std::string& id = "",
                    const std::string& className = "")
  {
    Paragraph* p = new Paragraph(text, font, fontSize, maxWidth, id, className);
    elements.emplace_back(p);
  }

  void addButton(const std::string& text, const sf::Font& font,
                 const std::string& id = "", const std::string& className = "")
  {
    Button* b = new Button(text, font, 0, 0, id, className);
    elements.emplace_back(b);
  }

  void addChild(Div&& child)
  {
    children.push_back(std::move(child));
  }
  Element* getElementById(const std::string& searchId)
  {
    for (auto& elem : elements)
    {
      if (elem.getId() == searchId) return &elem;
    }

    for (auto& child : children)
    {
      Element* found = child.getElementById(searchId);
      if (found) return found;
    }

    return nullptr;
  }

  void draw(sf::RenderWindow& window)
  {
    float currentY = y;

    Style style = getStyle(isHovered(window));

    sf::RectangleShape bg;
    float totalHeight = getTotalHeight();
    bg.setPosition(x, y);
    bg.setSize({maxWidth, totalHeight});
    bg.setFillColor(style.backgroundColor);
    window.draw(bg);

    for (auto& elem : elements)
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
    }

    for (auto& child : children)
    {
      child.x = x + 10;
      child.y = currentY;
      child.draw(window);
      currentY += child.getTotalHeight();
    }
  }

  float getTotalHeight() const
  {
    float h = 0;
    for (const auto& elem : elements)
    {
      if (elem.type == ElementType::Paragraph)
        h += elem.paragraph->getHeight();
      else if (elem.type == ElementType::Button)
        h += elem.button->getHeight();
    }

    for (const auto& div : children) h += div.getTotalHeight();

    return h;
  }

  Style getStyle(bool hover = false) const
  {
    std::string selector = "#" + id;
    if (hover && styleSheet.count(selector + ":hover"))
      return styleSheet[selector + ":hover"];
    if (styleSheet.count(selector)) return styleSheet[selector];

    selector = "." + className;
    if (hover && styleSheet.count(selector + ":hover"))
      return styleSheet[selector + ":hover"];
    if (styleSheet.count(selector)) return styleSheet[selector];

    selector = "div";
    if (hover && styleSheet.count("div:hover")) return styleSheet["div:hover"];
    if (styleSheet.count("div")) return styleSheet["div"];

    return Style();  // 默认样式
  }
  void handleEvent(const sf::Event& event, const sf::RenderWindow& window)
  {
    for (auto& elem : elements)
    {
      if (elem.type == ElementType::Button)
      {
        elem.button->handleEvent(event, window);
      }
    }

    for (auto& child : children)
    {
      child.handleEvent(event, window);
    }
  }

  bool isHovered(const sf::RenderWindow& window) const
  {
    auto mousePos = sf::Mouse::getPosition(window);
    return mousePos.x >= x && mousePos.x <= x + maxWidth && mousePos.y >= y &&
           mousePos.y <= y + getTotalHeight();
  }
};

sf::Color parse_css_color(const std::string& val)
{
  if (val == "red") return sf::Color::Red;
  if (val == "green") return sf::Color::Green;
  if (val == "blue") return sf::Color::Blue;
  if (val == "black") return sf::Color::Black;
  if (val == "white") return sf::Color::White;
  if (val == "gray") return sf::Color(128, 128, 128);
  if (val == "yellow") return sf::Color::Yellow;

  // 支持 #RRGGBB 十六进制格式
  if (val.size() == 7 && val[0] == '#')
  {
    int r, g, b;
    std::stringstream ss;
    ss << std::hex << val.substr(1, 2);
    ss >> r;
    ss.clear();
    ss << std::hex << val.substr(3, 2);
    ss >> g;
    ss.clear();
    ss << std::hex << val.substr(5, 2);
    ss >> b;
    return sf::Color(r, g, b);
  }

  return sf::Color::White;  // 默认
}
// 解析 CSS 文本并填充 styleSheet
void parse_css_style(const std::string& cssText)
{
  std::regex blockRegex(R"(([^\{]+)\{([^}]+)\})");
  std::regex propRegex(R"(([^:]+):([^;]+);?)");

  auto blocksBegin =
      std::sregex_iterator(cssText.begin(), cssText.end(), blockRegex);
  auto blocksEnd = std::sregex_iterator();

  for (auto i = blocksBegin; i != blocksEnd; ++i)
  {
    std::string selector =
        std::regex_replace((*i)[1].str(), std::regex(R"(\s+)"), "");
    std::string body = (*i)[2].str();

    Style style;

    auto propsBegin = std::sregex_iterator(body.begin(), body.end(), propRegex);
    auto propsEnd = std::sregex_iterator();

    for (auto j = propsBegin; j != propsEnd; ++j)
    {
      std::string prop =
          std::regex_replace((*j)[1].str(), std::regex(R"(\s+)"), "");
      std::string val =
          std::regex_replace((*j)[2].str(), std::regex(R"(\s+)"), "");

      if (prop == "background-color")
      {
        style.backgroundColor = parse_css_color(val);
      }
      else if (prop == "color")
      {
        style.textColor = parse_css_color(val);
      }
      else if (prop == "font-size")
      {
        style.fontSize = std::stoi(val);
      }
      else if (prop == "padding")
      {
        style.padding = std::stof(val);
      }
      else if (prop == "border-radius")
      {
        style.borderRadius = std::stof(val);
      }
      else if (prop == "border-color")
      {
        style.borderColor = parse_css_color(val);
      }
      else if (prop == "border-width")
      {
        style.borderThickness = std::stof(val);
      }
    }

    styleSheet[selector] = style;
  }
}

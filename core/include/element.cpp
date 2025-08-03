// element.cpp
#include "element.h"
#include "paragraph.h"
#include "div.h" // 如果 Button 在 div.h 里
#include "grid.h"

Element::Element(Paragraph *p) : type(ElementType::Paragraph), paragraph(p) {}
Element::Element(Button *b) : type(ElementType::Button), button(b) {}
Element::Element(Grid *g) : type(ElementType::Grid), grid(g) {}

Element::~Element()
{
  if (type == ElementType::Paragraph)
    delete paragraph;
  else if (type == ElementType::Button)
    delete button;
  else if (type == ElementType::Grid)
    delete grid;
}
Element::Element()
{
}
Element::Element(Element &&other) noexcept : type(other.type)
{
  if (type == ElementType::Paragraph)
  {
    paragraph = other.paragraph;
    other.paragraph = nullptr;
  }
  else if (type == ElementType::Button)
  {
    button = other.button;
    other.button = nullptr;
  }
  else if (type == ElementType::Grid)
  {
    grid = other.grid;
    other.grid = nullptr;
  }
}

Element &Element::operator=(Element &&other) noexcept
{
  if (this != &other)
  {
    this->~Element();
    type = other.type;
    if (type == ElementType::Paragraph)
    {
      paragraph = other.paragraph;
      other.paragraph = nullptr;
    }
    else if (type == ElementType::Button)
    {
      button = other.button;
      other.button = nullptr;
    }
    else if (type == ElementType::Grid)
    {
      grid = other.grid;
      other.grid = nullptr;
    }
  }
  return *this;
}

std::string Element::getId() const
{
  if (type == ElementType::Paragraph && paragraph)
    return paragraph->id;
  if (type == ElementType::Button && button)
    return button->id;
  if (type == ElementType::Grid && grid)
    return grid->id;
  return "";
}
sf::Color parse_css_color(const std::string &val)
{
  if (val == "red")
    return sf::Color::Red;
  if (val == "green")
    return sf::Color::Green;
  if (val == "blue")
    return sf::Color::Blue;
  if (val == "black")
    return sf::Color::Black;
  if (val == "white")
    return sf::Color::White;
  if (val == "gray")
    return sf::Color(128, 128, 128);
  if (val == "yellow")
    return sf::Color::Yellow;

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

  return sf::Color::White; // 默认
}
inline float parse_css_float(const std::string &val)
{
  std::string number;

  // 忽略前导空格
  size_t i = 0;
  while (i < val.size() && std::isspace(val[i]))
    ++i;

  // 收集可能的负号、小数点和数字
  bool hasDigit = false;
  for (; i < val.size(); ++i)
  {
    char c = val[i];
    if ((c >= '0' && c <= '9') || c == '.' || c == '-')
    {
      number += c;
      if (std::isdigit(c))
        hasDigit = true;
    }
    else
    {
      break; // 停在单位（px等）之前
    }
  }

  if (!hasDigit)
  {
    std::cerr << "[CSS ERROR] Invalid float string: \"" << val << "\"\n";
    throw std::invalid_argument("Invalid float string: " + val);
  }

  try
  {
    return std::stof(number);
  }
  catch (const std::exception &e)
  {
    std::cerr << "[CSS EXCEPTION] std::stof failed on \"" << number << "\" ← from \"" << val << "\"\n";
    throw;
  }
}

inline void trim(std::string &s)
{
  size_t start = s.find_first_not_of(" \t\r\n");
  size_t end = s.find_last_not_of(" \t\r\n");
  if (start == std::string::npos)
  {
    s.clear();
  }
  else
  {
    s = s.substr(start, end - start + 1);
  }
}

// 解析 CSS 文本并填充 styleSheet
void parse_css_style(const std::string &cssText)
{
  std::regex blockRegex(R"(([^\{]+)\{([^}]+)\})");
  std::regex propRegex(R"(([^:]+):([^;]+);?)");

  auto blocksBegin = std::sregex_iterator(cssText.begin(), cssText.end(), blockRegex);
  auto blocksEnd = std::sregex_iterator();

  for (auto i = blocksBegin; i != blocksEnd; ++i)
  {
    std::string selector = std::regex_replace((*i)[1].str(), std::regex(R"(\s+)"), "");
    std::string body = (*i)[2].str();

    Style style;

    auto propsBegin = std::sregex_iterator(body.begin(), body.end(), propRegex);
    auto propsEnd = std::sregex_iterator();

    for (auto j = propsBegin; j != propsEnd; ++j)
    {
      std::string prop = (*j)[1].str();
      std::string val = (*j)[2].str();
      trim(prop);
      trim(val);

      // 通用样式属性
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
        style.padding = parse_css_float(val);
      }
      else if (prop == "border-radius")
      {
        style.borderRadius = parse_css_float(val);
      }
      else if (prop == "border-color")
      {
        style.borderColor = parse_css_color(val);
      }
      else if (prop == "border-width")
      {
        style.borderThickness = parse_css_float(val);
      }
      // 网格特定属性
      else if (prop == "grid-template-columns")
      {
        style.gridTemplateColumns = val;
      }
      else if (prop == "grid-template-rows")
      {
        style.gridTemplateRows = val;
      }
      else if (prop == "grid-column-gap")
      {
        style.gridColumnGap = parse_css_float(val);
      }
      else if (prop == "grid-row-gap")
      {
        style.gridRowGap = parse_css_float(val);
      }
      else if (prop == "grid-gap")
      {
        // 同时设置行间距和列间距
        style.gridColumnGap = style.gridRowGap = parse_css_float(val);
      }
      else if (prop == "justify-items")
      {
        style.justifyItems = val;
      }
      else if (prop == "align-items")
      {
        style.alignItems = val;
      }
    }

    styleSheet[selector] = style;
  }
}
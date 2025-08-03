// element.h
#pragma once
#include <string>

enum class ElementType { Paragraph, Button, Grid };

// 前向声明！不要 include
struct Paragraph;
struct Button;
struct Grid;

struct Element {
  ElementType type;
  union {
    Paragraph* paragraph;
    Button* button;
    Grid* grid;
  };

  Element(Paragraph* p);
  Element(Button* b);
  Element(Grid* g);
  Element();
  ~Element();

  Element(Element&& other) noexcept;
  Element& operator=(Element&& other) noexcept;

  std::string getId() const;
};

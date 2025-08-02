#include "include/div.h"
#include "include/font.h"

/*start*/

/*back_start*/
#ifndef MKMLtitle
#define MKMLtitle "My MKCC Application"
#endif
#ifndef MKMLsize_x
#define MKMLsize_x "800"
#endif
#ifndef MKMLsize_y
#define MKMLsize_y "600"
#endif
/*back_end*/
template<typename T>
std::unique_ptr<T> create_script(Div& root) {
    auto s = std::make_unique<T>(root);
    s->on_load();
    return s;
}


class script{
public:
    Div *root;
    script(Div & rootnode){
        root=&rootnode;on_load();
}
    ~script(){
        on_unload();
    }
    virtual void on_load(){

    }
    virtual void on_unload(){

    }
};


/*script*/



int str_to_int(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (const std::invalid_argument&) {
        return 0;
    }
}

int main() {
    sf::RenderWindow window(
    sf::VideoMode(str_to_int(MKMLsize_x), str_to_int(MKMLsize_y)),
    MKMLtitle,
    sf::Style::Titlebar | sf::Style::Close // 禁止拉伸，保留标题栏和关闭按钮
);
    windowWidth = str_to_int(MKMLsize_x);
    windowHeight = str_to_int(MKMLsize_y);
    // 加载字体（SFML 需要）
    sf::Font font;
    std::string fontPath = getSystemFontPath("Arial");
    std::vector<std::unique_ptr<script>> scripts_list;
    
    if (!fontPath.empty()) {
        if (!font.loadFromFile(fontPath)) {
            std::cerr << "The font file exists, but fails to load: " << fontPath << std::endl;
        }
    } else {
        std::cerr << "The system font cannot be found Arial" << std::endl;
    }
    

    // 创建 div
    Div rootdiv(2, 2);
    
    
/*body_start*/


/*scripts_start*/

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        rootdiv.handleEvent(event, window);

        window.clear(sf::Color::White);
        rootdiv.draw(window);
        window.display();
    }
    for (auto& s : scripts_list) {
        if (s) s->on_unload();
    }

    return 0;
}

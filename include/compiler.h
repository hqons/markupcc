#pragma once
#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <random>

enum body_type {
    Paragraph,
    Button
};
struct body_code {
    std::string parent_var;
    std::unordered_map<std::string, std::string> *parent_attrs;
    int body_type;
    std::string text;
    int font_size;
    std::unordered_map<std::string, std::string> attrs;
};
struct div_body {
    std::string var;
    std::unordered_map<std::string, std::string> *parent_attrs;
};
void write_file(const std::string& path, const std::string& content)
{
  std::ofstream file(path);
  if (file) file << content;
}

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) return "";

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

class mkml_node {
public:
    std::string name;                    // 节点名，例如 head、title、body
    std::string content;                 // 文本内容
    std::vector<mkml_node> children;     // 子节点
    std::unordered_map<std::string, std::string> attrs; // 属性键值对
    std::string note;
    mkml_node* parent=nullptr;
    mkml_node() = default;
    mkml_node(const std::string& n) : name(n) {}

};

// 解析 HTML 节点并构建 mkml_node 树
inline mkml_node build_mkml_tree(xmlNode* xml_node) {
    mkml_node node;

    if (xml_node->type == XML_ELEMENT_NODE) {
        node.name = (const char*)xml_node->name;

        // 提取属性
        for (xmlAttr* attr = xml_node->properties; attr; attr = attr->next) {
            std::string key = (const char*)attr->name;
            std::string val = (const char*)xmlNodeGetContent(attr->children);
            node.attrs[key] = val;
        }

        // 对于 style，script，直接收集文本内容
        if (node.name == "style" || node.name == "script") {
            xmlChar* raw_content = xmlNodeGetContent(xml_node);
            if (raw_content) {
                node.content = (const char*)raw_content;
                xmlFree(raw_content);
            }
            return node;
        }



    } else if (xml_node->type == XML_TEXT_NODE) {
        std::string text = (const char*)xml_node->content;
        if (text.find_first_not_of(" \n\t\r") == std::string::npos)
            return {}; // 空白节点忽略
        node.name = "#text";
        node.content = text;
        return node;
    }

    // 递归处理子节点（非 style）
    for (xmlNode* child = xml_node->children; child; child = child->next) {
        mkml_node child_node = build_mkml_tree(child);
        if (!child_node.name.empty()) {
            if (child_node.name == "#text") {
                node.content += child_node.content;
            } else {
                node.children.push_back(child_node);
            }
        }
    }

    return node;
}


// 顶层函数，传入 HTML 字符串返回 mkml_node 树
inline mkml_node parse_html_to_mkml(const std::string& html) {
    htmlDocPtr doc = htmlReadMemory(html.c_str(), html.size(), nullptr, nullptr,
                                     HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (!doc) {
        std::cerr << "Failed to parse MKML.\n";
        return {};
    }

    xmlNode* root = xmlDocGetRootElement(doc);
    mkml_node tree = build_mkml_tree(root);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return tree;
}
std::string generate_uuid_var(const std::string& prefix = "var_", size_t length = 12) {
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string uuid = prefix;

    // 确保首字符是字母或下划线（不能以数字开头）
    char first = charset[dist(rng)];
    while ((first >= '0' && first <= '9')) {
        first = charset[dist(rng)];
    }
    uuid += first;

    // 生成剩下的随机字符
    for (size_t i = 1; i < length; ++i) {
        uuid += charset[dist(rng)];
    }

    return uuid;
}
std::string escape_text(const std::string& text) {
    std::string result;
    for (char c : text) {
        if (c == '\n' || c == '\r') {
            result += ' '; // 将换行替换为空格
        } else if (c == '"') {
            result += "\\\""; // 转义双引号
        } else {
            result += c;
        }
    }
    return result;
}
void recursion_body_code(
    mkml_node & node,
    std::vector<body_code>& body_codes,
    std::vector<std::string>& div_vars,
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>*>& div_info_map
){
    if (node.name == "div") {
        std::string var = generate_uuid_var();
        div_vars.push_back(var);
        div_info_map[var] = &node.attrs; // 保存 div 的 id/class 等属性指针

        for (auto& child : node.children) {
            child.note = var;
            child.parent = &node;
            recursion_body_code(child, body_codes, div_vars, div_info_map); // 传递 div_info_map
        }
    } else if (node.name == "p" || node.name == "h1" || node.name == "h2" || node.name == "h3" ||
               node.name == "h4" || node.name == "h5" || node.name == "h6" || node.name == "button") {

        body_code code;
        code.attrs = node.attrs;

        if (node.parent) {
            code.parent_attrs = &node.parent->attrs;
        }

        code.parent_var = node.note.empty() ? "rootdiv" : node.note;

        if (node.name == "button") {
            code.body_type = body_type::Button;
            code.text = node.content;
        } else {
            code.body_type = body_type::Paragraph;
            code.text = node.content;

            if (node.name == "p")        code.font_size = 16;
            else if (node.name == "h1") code.font_size = 32;
            else if (node.name == "h2") code.font_size = 24;
            else if (node.name == "h3") code.font_size = 19;
            else if (node.name == "h4") code.font_size = 16;
            else if (node.name == "h5") code.font_size = 13;
            else if (node.name == "h6") code.font_size = 11;
        }

        body_codes.push_back(code);
    }
}
std::string sanitize_key(const std::string& key) {
    std::string result = key;
    for (char& c : result) {
        if (c == '-') c = '_';
    }
    return result;
}
void compile(mkml_node& mkml, const std::string& maincpp_path) {
    std::string heads = "";
    std::unordered_map<std::string, std::string> heads_tag;
    std::vector<body_code> body_codes;
    std::vector<std::string> div_vars;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>*> div_info_map;
    std::string css;
    std::unordered_map<std::string, std::string> scripts;
    // 提取 <head> 中的宏定义内容
    for (auto& node : mkml.children) {
        if (node.name == "head") {
            for (auto& child : node.children) {
                if (child.name == "style") {
                    
                    std::string style_text=child.content;
                    if (child.attrs.find("src") != child.attrs.end()){
                        style_text=read_file(child.attrs["src"]);
                    }
                    css.append(style_text + "\n");
                    
                    continue;
                }
                if (child.name == "script"){
                    std::string code_text=child.content;
                    if (child.attrs.find("src") != child.attrs.end()){
                        code_text=read_file(child.attrs["src"]);
                    }
                    std::string name= generate_uuid_var("class");
                    scripts[name]=code_text;
                    continue;
                }

                heads_tag[child.name] = child.content;
                
                
                for (const auto& attr : child.attrs) {
                    heads_tag[child.name + "_" + attr.first] = attr.second;
                }
            }
        } else if (node.name == "body") {
            for (auto& child : node.children) {
                child.note = "rootdiv";
                recursion_body_code(child, body_codes, div_vars, div_info_map);
            }
        }
    }

    // 生成宏定义字符串
    for (const auto& pair : heads_tag) {
        std::string code = "#define MKML" + sanitize_key(pair.first) + " \"" + pair.second + "\"\n";
        heads.append(code);
    }
    std::string scripts_code="\n";
    std::string scripts_list_code="\n";
    //Script
    for (const auto& [key, value] : scripts) {
        std::string code = "\nclass "+key+" : public script {\npublic:\n    using script::script;\n"+value+"};\n";
        scripts_code.append(code);
        code="scripts_list.emplace_back(create_script<"+key+">(rootdiv));\n";
        scripts_list_code.append(code);
    }

    // 将 body_codes 分类到每个 div 下

    std::unordered_map<std::string, std::vector<body_code>> div_map;
    for (const auto& code : body_codes) {
        div_map[code.parent_var].push_back(code);
    }

    // 构建 UI 构造代码
    std::ostringstream body_code_out;
    body_code_out << "// Auto-generated UI build code\n";
    body_code_out<<"std::string css = R\"("<<css<<")\";\n"<<"parse_css_style(css);\n";


    // rootdiv 部分（段落和按钮）
    for (const auto& code : div_map["rootdiv"]) {
        std::string id = "";
        std::string cssclass = "";

        auto it = code.attrs.find("id");
        if (it != code.attrs.end()) id = it->second;
        it = code.attrs.find("class");
        if (it != code.attrs.end()) cssclass = it->second;

        if (code.body_type == body_type::Paragraph) {
            body_code_out << "rootdiv.addParagraph(\"" << escape_text(code.text)
                          << "\", font, " << code.font_size
                          << ", \"" << id << "\", \"" << cssclass << "\");\n";
        } else if (code.body_type == body_type::Button) {
            body_code_out << "rootdiv.addButton(\"" << escape_text(code.text) << "\", font, "
                          << " \"" << id << "\", \"" << cssclass << "\");\n";
        }
    }

    // 子 div 声明与填充
    for (const auto& pair : div_map) {
        const std::string& var = pair.first;
        if (var == "rootdiv") continue;

        std::string id = "";
        std::string cssclass = "";
        auto* attrs_ptr = div_info_map[var];
        if (attrs_ptr) {
            auto it = attrs_ptr->find("id");
            if (it != attrs_ptr->end()) id = it->second;
            it = attrs_ptr->find("class");
            if (it != attrs_ptr->end()) cssclass = it->second;
        }

        body_code_out << "Div " << var << "(10, 0, \"" << id << "\", \"" << cssclass << "\");\n";

        for (const auto& code : pair.second) {
            if (code.body_type == body_type::Paragraph) {
            body_code_out << "rootdiv.addParagraph(\"" << escape_text(code.text)
                          << "\", font, " << code.font_size
                          << ", \"" << id << "\", \"" << cssclass << "\");\n";
            } else if (code.body_type == body_type::Button) {
                body_code_out << "rootdiv.addButton(\"" << escape_text(code.text) << "\", font, "
                              << " \"" << id << "\", \"" << cssclass << "\");\n";
            }
        }
        body_code_out << "rootdiv.addChild(std::move(" << var << "));\n";
    }

    // 注入 main.cpp
    std::ifstream infile(maincpp_path);
    if (!infile.is_open()) {
        std::cerr << "[mkcc] Cannot open main.cpp for injection: " << maincpp_path << "\n";
        return;
    }

    std::ostringstream new_main;
    std::string line;
    while (std::getline(infile, line)) {
        new_main << line << "\n";
        if (line == "/*start*/") {
            new_main << heads;
        }
        if (line == "/*script*/") {
            new_main << scripts_code;
        }
        if (line == "/*body_start*/") {
            new_main << body_code_out.str();
        }
        if (line == "/*scripts_start*/") {
            new_main << scripts_list_code;
        }
    }
    infile.close();

    write_file(maincpp_path, new_main.str());
    std::cout << "[mkcc] File generated " << maincpp_path << "\n";
}

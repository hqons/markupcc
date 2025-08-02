#include <cstdlib>  // for std::system
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "include/compiler.h"
using json = nlohmann::json;
namespace fs = std::filesystem;

#ifdef _WIN32
#define PSEP "\\"
#else
#define PSEP "/"
#endif
inline std::string PATH(const std::string& x, const std::string& y)
{
  return (fs::path(x) / y).string();
}

std::string program_dir;
std::string ppath(std::string path)
{
  return (std::filesystem::path(program_dir) / std::filesystem::path(path))
      .string();
}
// 拆分字符串
std::vector<std::string> split_path(const std::string& path, char delimiter)
{
  std::vector<std::string> parts;
  std::stringstream ss(path);
  std::string item;
  while (std::getline(ss, item, delimiter))
  {
    if (!item.empty()) parts.push_back(item);
  }
  return parts;
}

std::string conversion_path(std::string path)
{
  // 自动识别 / 或 \ 分隔，并统一使用 PSEP
  char delimiter = path.find('\\') != std::string::npos ? '\\' : '/';
  std::vector<std::string> parts = split_path(path, delimiter);

  std::ostringstream result;
  for (size_t i = 0; i < parts.size(); ++i)
  {
    result << parts[i];
    if (i != parts.size() - 1) result << PSEP;
  }
  return result.str();
}

<<<<<<< HEAD

=======
>>>>>>> 387b21eb3c7939d3a89c0580b25f00bfc977950b
void copy_file_safe(const std::string& from, const std::string& to)
{
  try
  {
    fs::copy_file(from, to, fs::copy_options::overwrite_existing);
    std::cout << "[mkcc] Copied: " << from << " → " << to << "\n";
  }
  catch (const fs::filesystem_error& e)
  {
    std::cerr << "[mkcc] Failed to copy " << from << ": " << e.what() << "\n";
  }
}
void copy_directory_safe(const fs::path& from, const fs::path& to)
{
  try
  {
    if (!fs::exists(from) || !fs::is_directory(from))
    {
      std::cerr
          << "[mkcc] Source directory does not exist or is not a directory: "
          << from << "\n";
      return;
    }

    // 创建目标目录（如果不存在）
    if (!fs::exists(to))
    {
      fs::create_directories(to);
    }

    for (const auto& entry : fs::recursive_directory_iterator(from))
    {
      const auto& path = entry.path();
      auto relative_path = fs::relative(path, from);
      fs::path target_path = to / relative_path;

      try
      {
        if (fs::is_directory(path))
        {
          fs::create_directories(target_path);
        }
        else if (fs::is_regular_file(path))
        {
          fs::copy_file(path, target_path,
                        fs::copy_options::overwrite_existing);
          std::cout << "[mkcc] Copied: " << path << " → " << target_path
                    << "\n";
        }
        // 忽略符号链接和其他类型
      }
      catch (const fs::filesystem_error& e)
      {
        std::cerr << "[mkcc] Failed to copy " << path << ": " << e.what()
                  << "\n";
      }
    }
  }
  catch (const fs::filesystem_error& e)
  {
    std::cerr << "[mkcc] Directory copy failed: " << e.what() << "\n";
  }
}

void show_help()
{
  std::cout << "mkcc - Markup + C++ project tool\n\n";
  std::cout << "Usage:\n";
  std::cout << "mkcc init Initializes the project template\n";
  std::cout << "mkcc make Compiles the project\n";
  std::cout << "mkcc run Runs the project\n";
  std::cout << "mkcc release Packages the release version\n";
  std::cout << "mkcc help Displays help information\n";
}
void init()
{
  if (!std::filesystem::exists(".mkcc") ||
      !std::filesystem::is_directory(".mkcc"))
  {
    fs::create_directory(".mkcc");
  }
  else
  {
    std::cout << "[mkcc] The .mkcc already exists." << std::endl;
  }

  if (!std::filesystem::exists("mkccmake.json"))
  {
    copy_file_safe(ppath(PATH("mkcc_resource", "mkccmake.json")),
                   "mkccmake.json");
  }
  else
  {
    std::cout << "[mkcc] The mkccmake.json already exists." << std::endl;
  }
<<<<<<< HEAD
  if (!std::filesystem::exists("makefile"))
  {
    copy_file_safe(ppath(PATH("mkcc_resource", "makefile")),
                   "makefile");
  }
  
=======
>>>>>>> 387b21eb3c7939d3a89c0580b25f00bfc977950b
}
int make(){
  std::ifstream json_file("mkccmake.json");
    if (!json_file)
    {
      std::cerr << "[mkcc] Error: mkccmake.json not found.\n";
      return 1;
    }

    json config;
    try
    {
      json_file >> config;
    }
    catch (const std::exception& e)
    {
      std::cerr << "[mkcc] JSON parsing error: " << e.what() << "\n";
      return 1;
    }

    std::string name = config.value("name", "unknown");
    std::string version = config.value("version", "0.0.0");
    std::string entry = config.value("entry", "");
    std::string build = conversion_path(config.value("output", "build"));

    std::cout << "[mkcc] Building the '" << name << "' version " << version
              << "...\n";

    std::ifstream file(entry);
    if (!file.is_open())
    {
      std::cerr << "[mkcc] Unable to open entry file: " << entry << std::endl;
      return 1;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string markup_source = ss.str();
    file.close();

    mkml_node root = parse_html_to_mkml(markup_source);

    // 创建构建输出目录
    if (!std::filesystem::exists(build))
    {
      std::filesystem::create_directories(build);
    }

    // 写入 main.cpp
    std::string output_cpp_path = PATH(build, "main.cpp");
    copy_file_safe(ppath(PATH("mkcc_resource", "main.cpp")), output_cpp_path);
    copy_directory_safe(ppath(PATH("mkcc_resource", "include")),
                        PATH(build, "include"));
    compile(root, output_cpp_path);
    std::string output_binary = PATH(build, "build.out");  // 可执行文件名

    std::ostringstream cmd;
    cmd << "g++ " << output_cpp_path << " -std=c++17 -o " << output_binary
        << " -lsfml-graphics -lsfml-window -lsfml-system";

    int result = std::system(cmd.str().c_str());
    if (result != 0)
    {
      std::cerr << "[mkcc] Compilation failed with code: " << result << "\n";
      return result;
    }

    std::cout << "[mkcc] Build complete: " << output_binary << "\n";
    return 0;
}
std::string get_program_dir(const char* argv0)
{
  std::filesystem::path exec_path = std::filesystem::absolute(argv0);
  return exec_path.parent_path().string();
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    show_help();
    return 0;
  }
#ifdef __linux__
  program_dir = "/usr/bin";
#endif

  std::string command = argv[1];

  if (command == "init")
  {
    std::cout << "[mkcc] Initializing the project template..." << std::endl;

    init();
    std::cout << "[mkcc] Initialization successful" << std::endl;
  }
  else if (command == "make")
  {
    return make();
  }

  else if (command == "run")
  {
    std::ifstream json_file("mkccmake.json");
    if (!json_file)
    {
      std::cerr << "[mkcc] Error: mkccmake.json not found.\n";
      return 1;
    }

    json config;
    try
    {
      json_file >> config;
    }
    catch (const std::exception& e)
    {
      std::cerr << "[mkcc] JSON parsing error: " << e.what() << "\n";
      return 1;
    }

    std::string build = conversion_path(config.value("output", "build"));
    std::string binary_path = PATH(build, "build.out");

    if (!fs::exists(binary_path))
    {
      std::cerr << "[mkcc] Error: build output not found: " << binary_path
                << "\n";
      int code=make();
      if (code!=0){
        return code;
      }
    }

    std::cout << "[mkcc] Running " << binary_path << "\n";
    return std::system(binary_path.c_str());
  }

  else if (command == "release")
  {
    std::cout << "[mkcc] Packaging for release..." << std::endl;
  }
  else if (command == "help" || command == "--help" || command == "-h")
  {
    show_help();
  }
  else
  {
    std::cout << "[mkcc] Unknown command: " << command << "\n";
    show_help();
  }

  return 0;
}

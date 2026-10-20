#include <argparse/argparse.hpp>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

struct NOTE {
  uint8_t priority;
  std::string content;
  // date
};

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("todo");

  const std::string name_file = "TODO.txt";

  std::filesystem::path todo_path;
  char* HOME = getenv("HOME");
  if (HOME == nullptr) {
      todo_path = std::filesystem::current_path() / name_file;
  } else {
      todo_path = std::filesystem::path(HOME) / ".config" / "todo" / name_file;
  }

  // Arguments
  program.add_argument("-n", "--new").help("Create a new register.");
  program.add_argument("-d", "--date").help("Orden by date.").flag();
  program.add_argument("-p", "--priority")
      .help("Set the priority.")
      .default_value(3);
  program.add_argument("-t", "--todo-path")
      .help("Path to TODO file.")
      .default_value(todo_path);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  todo_path = program.get<std::filesystem::path>("--todo-path");


  if (!std::filesystem::exists(todo_path)) {
      std::filesystem::create_directories(todo_path.parent_path());
  }

  if (program.is_used("--new")) {
      file = todo_path;
  }






  return 0;
}

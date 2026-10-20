#include <algorithm>
#include <argparse/argparse.hpp>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
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
  char *HOME = getenv("HOME");
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
      .default_value(int{3});
  program.add_argument("-t", "--todo-path")
      .help("Path to TODO file.")
      .store_into(todo_path) // Does this really works?
      .default_value(todo_path);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  // TODO: arreglar argumento de path
  todo_path = program.get<std::filesystem::path>("--todo-path");

  if (todo_path.is_relative()) {
    todo_path = std::filesystem::current_path() / todo_path;
  }

  if (!std::filesystem::exists(todo_path)) {
    std::filesystem::create_directories(todo_path.parent_path());
    std::ofstream create_file(todo_path);
    create_file.close();
  }

  std::ifstream file(todo_path);
  if (!file) {
    std::cerr << "Error opening file at desired path." << std::endl;
    return 1;
  }

  // Write todo file
  if (program.is_used("--new")) {
    const auto note = program.present<std::string>("--new");
    if (note.value().empty()) {
      std::cerr << "--new needs content.";
      return 1;
    }

    const int priority = program.get<int>("--priority");
    std::ofstream write(todo_path, std::ios::app);
    if (!write) {
      std::cerr << "Error opening file at writting time.";
      return 1;
    }

    std::string stream = std::to_string(priority) + " " + note.value() + '\n';

    write.write(stream.c_str(), stream.size());
  }

  // Read todo file
  std::vector<std::string> content;
  std::string tmp;
  while (std::getline(file, tmp)) {
    content.push_back(tmp);
  }

  std::sort(content.begin(), content.end());

  for (std::string i : content) {
    std::cout << i << std::endl;
  }

  return 0;
}

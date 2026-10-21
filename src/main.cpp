#include <algorithm>
#include <argparse/argparse.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

constexpr unsigned int PRIO_MAX_VALUE = 3;

void new_note(argparse::ArgumentParser &program);
void arguments(argparse::ArgumentParser &program);
std::vector<std::string> get_todo_content();

static std::filesystem::path todo_path;

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("todo");
  arguments(program);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }
  const std::string NAME_FILE = "TODO.txt";

  char *HOME = getenv("HOME");
  if (HOME == nullptr) {
    todo_path = std::filesystem::current_path() / NAME_FILE;
  } else {
    todo_path = std::filesystem::path(HOME) / ".config" / "todo" / NAME_FILE;
  }

  const std::string custom_path =
      program.get<std::string>("--todo-path");

  if (!custom_path.empty()) {
      todo_path = custom_path;
  }


  if (todo_path.is_relative()) {
    todo_path = std::filesystem::current_path() / todo_path;
  }

  if (!std::filesystem::exists(todo_path)) {
    std::filesystem::create_directories(todo_path.parent_path());
    std::ofstream create_file(todo_path);
    create_file.close();
  }

  // Write todo file
  if (program.is_used("--new")) {
    new_note(program);
  } else {
    // Read todo file
    std::vector<std::string> content = get_todo_content();
    for (std::string i : content) {
      std::cout << i << std::endl;
    }
  }

  return 0;
}

std::vector<std::string> get_todo_content() {

  std::ifstream file(todo_path);

  if (!file) {
    std::cerr << "Error opening file at desired path." << std::endl;
    exit(1);
  }

  std::vector<std::string> content;
  std::string tmp;
  while (std::getline(file, tmp)) {
    content.push_back(tmp);
  }

  std::sort(content.begin(), content.end());
  file.close();
  return content;
}

/**
 * Create a new line at the file with the -n parameter
 */
void new_note(argparse::ArgumentParser &program) {

  const auto note = program.present<std::string>("--new");
  if (note.value().empty()) {
    std::cerr << "--new needs content.";
    exit(1);
  }

  int priority = program.get<int>("--priority");

  std::ofstream write(todo_path, std::ios::app);
  if (!write) {
    std::cerr << "Error opening file at writting time.";
    exit(1);
  }
  std::string stream = std::to_string(priority) + " " + note.value() + '\n';
  write.write(stream.c_str(), stream.size());
  write.close();
}

/**
 * Declare the arguments
 */
void arguments(argparse::ArgumentParser &program) {

  // Arguments
  program.add_argument("-n", "--new").help("Create a new register.");
  program.add_argument("-d", "--date").help("Orden by date.").flag();
  program.add_argument("-p", "--priority")
      .scan<'i', int>()
      .help("Set the priority.")
      .default_value(3);
  program.add_argument("-t", "--todo-path")
      .help("Path to TODO file.")
      .default_value(std::string{});
}

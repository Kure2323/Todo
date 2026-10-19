#include <argparse/argparse.hpp>
#include <cstdint>
#include <fstream>
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

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  // Arguments
  program.add_argument("-n", "--new").help("Create a new register.");

  program.add_argument("-d","--date").help("Orden by date.");

  program.add_argument("-p", "--priority").help("Set the priority.");

  // Open TODO file
  std::ifstream todo("~/.config/todo/TODO.txt");
  if (!todo) {
    std::cerr << "Coudn't find/create TODO.txt file";
    return 1;
  }

  std::vector<std::string> notes;
  std::string tmp;
  while (std::getline(todo, tmp)) {
    notes.push_back(tmp);
  }
  todo.close();
  tmp.clear();

  return 0;
}

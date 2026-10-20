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

  // Arguments
  program.add_argument("-n", "--new").help("Create a new register.");
  program.add_argument("-d", "--date").help("Orden by date.").flag();
  program.add_argument("-p", "--priority")
      .help("Set the priority.")
      .default_value(3);
  program.add_argument("-t", "--todo-path")
      .help("Path to TODO file");

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }







  return 0;
}

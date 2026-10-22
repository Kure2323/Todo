#include <algorithm>
#include <argparse/argparse.hpp>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

struct Note {
  unsigned int priority;
  std::string date;
  std::string content;
}; // 1|%date%|content

constexpr unsigned int PRIO_MAX_VALUE = 3;
static std::filesystem::path todo_path;

void new_note(argparse::ArgumentParser &program);
void arguments(argparse::ArgumentParser &program);
std::vector<std::string> get_todo_content(std::vector<Note> &notes);
std::string date_to_string();
std::tm string_to_date(const std::string &date);

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

  const std::string custom_path = program.get<std::string>("--todo-path");

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

  } else if (!program.is_used("--completed")) {
    // Read todo file
    std::vector<Note> notes;
    std::vector<std::string> content = get_todo_content(notes);

    if (!program.is_used("--reverse")) {
      std::reverse(content.begin(), content.end());
    }

    for (std::string i : content) {
      std::cout << i << std::endl;
    }
  }

  if (program.is_used("--completed")) {
    std::vector<Note> notes;
    std::vector<std::string> content = get_todo_content(notes);

    for (int i = 0; i < notes.size(); i++) {
      std::cout << '(' << i << ") " << notes[i].content << " Prio->"
                << notes[i].priority << std::endl;
    }

    std::string _answer;
    std::cin >> _answer;

    int answer = static_cast<int>(std::stoul(_answer));
    std::ofstream file(todo_path);

    for (int i = 0; i < content.size(); i++) {
      if (i == answer) {
        continue;
      }
      std::string tmp = content[i] + '\n';
      file.write(tmp.c_str(), tmp.size());
    }
  }

  return 0;
}

/**
 * Gets all the content in the todo file
 */
std::vector<std::string> get_todo_content(std::vector<Note> &notes) {

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

  for (std::string i : content) {

    // Split-like
    std::vector<std::string> pieces;
    std::stringstream ss(i);
    std::string tmp;
    while (std::getline(ss, tmp, '|')) {
      pieces.push_back(tmp);
    }
    Note note{static_cast<unsigned int>(std::stoul(pieces[0])),
              date_to_string(), pieces[1]};
    notes.push_back(note);
  }

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

  if (priority < 1 || priority > PRIO_MAX_VALUE) {
    priority = 3;
  }

  std::ofstream write(todo_path, std::ios::app);
  if (!write) {
    std::cerr << "Error opening file at writting time.";
    exit(1);
  }
  std::string stream = std::to_string(priority) + "|   " + note.value() +
                       "    |" + date_to_string() + '\n';
  write.write(stream.c_str(), stream.size());
  write.close();
}

/**
 * Declare the arguments
 */
void arguments(argparse::ArgumentParser &program) {

  // Arguments
  program.add_argument("-n", "--new").help("Create a new register");
  program.add_argument("-c", "--completed").help("Complete a todo").flag();
  program.add_argument("-p", "--priority")
      .scan<'i', int>()
      .help("Set the priority")
      .default_value(3);
  program.add_argument("-t", "--todo-path")
      .help("Path to TODO file")
      .default_value(std::string{});
  program.add_argument("-r", "--reverse").help("Reverse the output").flag();
}

std::string date_to_string() {
  auto now = std::chrono::system_clock::now();
  std::time_t t = std::chrono::system_clock::to_time_t(now);

  std::tm tm{};

  localtime_r(&t, &tm);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M");

  return oss.str();
}

std::tm string_to_date(const std::string &date) {
  std::tm tm{};

  std::istringstream iss(date);
  iss >> std::get_time(&tm, "%Y-%m-%d %H:%M");

  if (iss.fail()) {
    throw std::invalid_argument("Formato de fecha inválido");
  }

  return tm;
}

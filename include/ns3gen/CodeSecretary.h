#pragma once

#include <string>
#include <vector>

class CodeSecretary {
  std::vector<std::string> m_code;
  std::string m_indent;
public:
  CodeSecretary() = default;
  void push_back(std::string line) {
    m_code.push_back(m_indent+line);
  }
  void indentRight() {
    m_indent += "  ";
  }
  void indentLeft() {
    if (m_indent.empty()) return;
    m_indent.pop_back();
    m_indent.pop_back();
  }
  std::vector<std::string> get() {
    return m_code;
  }
};

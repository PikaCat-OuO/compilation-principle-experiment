#pragma once
#include <bits/stdc++.h>

// 代码处理器，默认处理C++代码
class CodeProcessor
{
public:

  // 构造函数
  CodeProcessor();

  // 设置关键字
  void setKeywords(const std::vector<std::string> &keywords);
  // 获取关键字
  const std::vector<std::string> &getKeywords() const;

  // 设置替换表
  void setReplaceTable(const std::unordered_map<std::string, std::string> &replaceTable);
  // 获取替换表
  const std::unordered_map<std::string, std::string> &getReplaceTable() const;

  // 分析代码
  std::vector<std::pair<std::string, std::string> > analyzeCode(const std::string_view code) const;

  /* 代码替换，将自己风格的代码替换回原来的C++代码
   * 语法import -> include
   * { } -> [ ]
   * [ ] -> { }
  */
  std::string replaceCode(const std::string_view code) const;

protected:

  // 是否是无用字符
  bool isUselessChar(const char ch) const;

  // 是否是标识符或关键字头
  bool isIdentifierHead(const char ch) const;
  // 是否是标识符或关键字体
  bool isIdentifierBody(const char ch) const;
  // 是否是关键字
  bool isKeyword(const std::string_view identifier) const;

  // 是否是数字体
  bool isDigitBody(const char ch) const;

  // 是否是需要替换的单词
  bool needToReplace(const std::string_view vocabulary) const;

private:

  // 保存着所有的关键字
  std::vector<std::string> mKeywords;

  // 替换表
  std::unordered_map<std::string, std::string> mReplaceTable;

};

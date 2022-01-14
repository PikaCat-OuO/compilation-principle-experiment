#include "codeprocessor.h"

// 构造函数
CodeProcessor::CodeProcessor() {

  // 初始化默认关键字
  this->setKeywords({
      "asm", "else", "new", "this",
      "auto", "enum", "operator", "throw",
      "bool", "explicit", "private", "true",
      "break", "export", "protected", "try",
      "case", "extern", "public", "typedef",
      "catch", "false", "register", "typeid",
      "char", "float", "reinterpret_cast", "typename",
      "class", "for", "return", "union",
      "const", "friend", "short", "unsigned",
      "const_cast", "goto", "signed", "using",
      "continue", "if", "sizeof", "virtual",
      "default", "inline", "static", "void",
      "delete", "int", "static_cast", "volatile",
      "do", "long", "struct", "wchat_t",
      "double", "mutable", "switch", "while",
      "dynamic_cast", "namespace", "template"
  });

  // 初始化替换表
  this->setReplaceTable({
      {"import", "include"},
      {"[", "{"},
      {"]", "}"},
      {"{", "["},
      {"}", "]"}
  });

}


// 设置关键字
void CodeProcessor::setKeywords(const std::vector<std::string> &keywords) {
  this->mKeywords = keywords;
}

// 获取关键字
const std::vector<std::string> &CodeProcessor::getKeywords() const {
  return this->mKeywords;
}

// 设置替换表
void CodeProcessor::setReplaceTable(const std::unordered_map<std::string, std::string> &replaceTable) {
  this->mReplaceTable = replaceTable;
}

// 获取替换表
const std::unordered_map<std::string, std::string> &CodeProcessor::getReplaceTable() const {
  return this->mReplaceTable;
}


// 是否是无用字符
bool CodeProcessor::isUselessChar(const char ch) const {
  return ch == ' ' or ch == '\t' or ch == '\n';
}


// 是否是关键字或者标识符头
bool CodeProcessor::isIdentifierHead(const char ch) const {
  return ch == '_' or isalpha(ch);
}

// 是否是关键字或者标识符头
bool CodeProcessor::isIdentifierBody(const char ch) const {
  return isIdentifierHead(ch) or isdigit(ch);
}


// 是否是关键字
bool CodeProcessor::isKeyword(const std::string_view identifier) const {
  for (const auto & keyword : this->mKeywords) {
    if (identifier == keyword) return true;
  }
  return false;
}


// 判断是否是数字体
bool CodeProcessor::isDigitBody(const char ch) const {
  return isdigit(ch) or ch == '.';
}


// 是否是需要替换的单词
bool CodeProcessor::needToReplace(const std::string_view vocabulary) const {
  for (const auto& [key, value] : this->mReplaceTable) {
    if (vocabulary == key) return true;
  }
  return false;
}

// 分析代码
std::vector<std::pair<std::string, std::string>>
CodeProcessor::analyzeCode(const std::string_view code) const {

  // 返回值
  std::vector<std::pair<std::string, std::string>> result;

  // 临时存放点
  std::string vocabulary;
  std::string type;

  // 循环分析每一个字符
  const char *current {cbegin(code)};
  while (current != cend(code)) {

    // 忽略空格，换行符，制表符
    while (this->isUselessChar(*current) and current != end(code)) ++current;
    if (current == end(code)) break;

    // 如果是标识符或者关键字头 (下划线或者字母打头)
    if (this->isIdentifierHead(*current)) {
      // 将字符保存到临时存放点中，一直读到标识符或者关键字结束
      while (this->isIdentifierBody(*current)) vocabulary += *current++;
      // 保存词性
      type = (this->isKeyword(vocabulary) ? "关键字" : "标识符");
      // 如果关键字是include，要做特殊处理
      if (vocabulary == "include") {
        // 先将现在的结果保存起来
        result.emplace_back(move(vocabulary), move(type));
        // 找到第一个<
        while (*current != '<') ++current;
        // 接收整一个文件名
        while (*current != '>') vocabulary += *current++;
        // 加上最后一个>
        vocabulary += *current++;
        // 保存词性
        type = "标识符";
      }
    }

    // 如果是数字
    else if (isdigit(*current)) {
      // 将字符保存到临时存放点中，一直读到数字结束
      while (this->isDigitBody(*current)) vocabulary += *current++;
      // 如果在中间遇到x或者X，b或者B
      if (tolower(*current) == 'x' or tolower(*current) == 'b') {
        vocabulary += *current++;
        // 继续接收后面的数字
        while (this->isDigitBody(*current)) vocabulary += *current++;
      }
      // 特判科学计数法
      if (tolower(*current) == 'e') {
        // 如果E后面跟着+-号也可
        if (*current == '+' or *current == '-') vocabulary += *current++;
        // 然后接收后面的数字
        while (isdigit(*current)) vocabulary += *current++;
      }
      // 特判数字后面的ul,ull
      if (tolower(*current) == 'u' and tolower(*(current + 1)) == 'l') {
        vocabulary += *current++;
        vocabulary += *current++;
        if (tolower(*current) == 'l') vocabulary += *current++;
      }
      // 特判f
      else if (tolower(*current) == 'f') vocabulary += *current++;
      // 特判l和ll
      else if (tolower(*current) == 'l') {
        vocabulary += *current++;
        if (tolower(*current) == 'l') vocabulary += *current++;
      }
      // 保存词性
      type = "数值常量";
    }

    // 其余情况用switch处理
    else {
      switch(*current) {
      // 如果是字符字面量
      case '\'':
        vocabulary += *current++;
        // 直到遇见下一个'
        while (*current != '\'') {
          // 处理转义字符的情况，只处理\\，\'，\"三种情况
          if (*current == '\\' and
              (*(current + 1) == '\'' or
               *(current + 1) == '\"' or
               *(current + 1) == '\\')) ++current;
          vocabulary += *current++;
        }
        // 将最后一个'也保存起来
        vocabulary += *current++;
        // 保存词性
        type = "字符常量";
        break;

      // 如果是字符串字面量
      case '\"':
        vocabulary += *current++;
        // 直到遇见下一个"
        while (*current != '\"') {
          // 处理转义字符的情况，只处理\\，\'，\"三种情况
          if (*current == '\\' and
              (*(current + 1) == '\'' or
               *(current + 1) == '\"' or
               *(current + 1) == '\\')) ++current;
          vocabulary += *current++;
        }
        // 将最后一个"也保存起来
        vocabulary += *current++;
        // 保存词性
        type = "字符串常量";
        break;

      // 如果是=号
      case '=':
        vocabulary += *current++;
        // 处理==的情况
        if (*current == '=') vocabulary += *current++;
        // 保存词性
        type = "运算符";
        break;

      // 如果是+号
      case '+':
        vocabulary += *current++;
        // 处理+=和++的情况
        if (*current == '=' || *current == '+') vocabulary += *current++;
        // 保存词性
        type = "运算符";
        break;

      // 如果是-号
      case '-':
        vocabulary += *current++;
        // 处理-=、--、->的情况
        if (*current == '=' || *current == '-' || *current == '>') vocabulary += *current++;
        // 处理->*的情况
        if (*(current - 1) == '>' and *current == '*') vocabulary += *current++;
        // 保存词性
        type = "运算符";
        break;

      // 如果是*号
      case '*':
        vocabulary += *current++;
        // 处理*=的情况
        if (*current == '=') vocabulary += *current++;
        // 保存词性
        type = "运算符";
        break;

      // 如果是/号
      case '/':
        // 处理//单行注释
        if (*(current + 1) == '/') {
          // 直到遇到换行符结束单行注释
          while (*current != '\n') vocabulary += *current++;
          // 保存词性
          type = "单行注释";
        }
        // 处理/**/多行注释
        else if (*(current + 1) == '*') {
          vocabulary += "/*";
          current += 2;
          // 直到遇到*/结束多行注释
          while (*current != '*' and *(current + 1) != '/') vocabulary += *current++;
          // 把*/也保存起来并保存词性
          vocabulary += "*/";
          current += 2;
          type = "多行注释";
        }
        // 其余情况就是一个除号
        else {
          vocabulary += *current++;
          // 保存词性
          type = "运算符";
        }
        break;

      // 如果是%
      case '%':
        vocabulary += *current++;
        // 处理%=的情况
        if (*current == '=') vocabulary += *current++;
        // 保存词性
        type = "运算符";
        break;

      // 如果是&
      case '&':
        vocabulary += *current++;
        // 处理&&和&=的情况
        if (*current == '&' || *current == '=') vocabulary += *current++;
        // 保存词性
        type = "运算符";
        break;

      // 如果是'|'
      case '|':
        vocabulary += *current++;
        // 处理||和|=的情况
        if (*current == '|' || *current == '=') vocabulary += *current++;
        // 保存词性
        type = "运算符";
        break;

      // 如果是!
      case '!':
        vocabulary += *current++;
        // 处理!=的情况
        if (*current == '=') vocabulary += *current++;
        // 保存词性
        type = "运算符";
        break;

      // 如果是^
      case '^':
        vocabulary += *current++;
        // 处理^=的情况
        if (*current == '=') vocabulary += *current++;
        // 保存词性
        type = "运算符";
        break;

      // 如果是<
      case '<':
        vocabulary += *current++;
        // 处理<=的情况
        if (*current == '=') vocabulary += *current++;
        else if (*current == '<') {
          // 处理<<的情况
          vocabulary += *current++;
          // 处理<<=的情况
          if (*current == '=') vocabulary += *current++;
        }
        // 保存词性
        type = "运算符";
        break;

      // 如果是>
      case '>':
        vocabulary += *current++;
        // 处理>=的情况
        if (*current == '=') vocabulary += *current++;
        else if (*current == '>') {
          // 处理>>的情况
          vocabulary += *current++;
          // 处理>>=的情况
          if (*current == '=') vocabulary += *current++;
        }
        // 保存词性
        type = "运算符";
        break;

      // 如果是.
      case '.':
        vocabulary += *current++;
        // 处理.*的情况
        if (*current == '*') vocabulary += *current++;
        // 处理...的情况
        if (*current == '.' and *(current + 1) == '.') {
          vocabulary += "..";
          current += 2;
        }
        // 保存词性
        type = "运算符";
        break;

      // 如果是:
      case ':':
        vocabulary += *current++;
        // 处理::的情况
        if (*current == ':') vocabulary += *current++;
        // 保存词性
        type = "运算符";
        break;

      // 其余字符
      default:
        vocabulary += *current++;
        type = "特殊符号";
      }

    }

    // 可以保存起来了
    result.emplace_back(std::move(vocabulary), std::move(type));
  }

  // 将template后面的两个<>从运算符替换为特殊符号
  for (auto iter {begin(result)}; iter != end(result); ++iter) {
    if (iter->first == "template") {
      // 找到下一个<
      while (iter->first != "<") ++iter;
      // 找到了，将其词性替换为特殊符号
      iter->second = "特殊符号";
      // 找到下一个>
      while (iter->first != ">") ++iter;
      // 找到了，将其词性替换为特殊符号
      iter->second = "特殊符号";
    }
  }

  // 将结果返回
  return result;

}


/* 代码替换，将自己风格的代码替换回原来的C++代码
   * 语法import -> include
   * { } -> [ ]
   * [ ] -> { }
  */
std::string CodeProcessor::replaceCode(const std::string_view code) const {

  // 要返回的经过处理后的代码
  std::string resultCode;

  // 临时存放点
  std::string vocabulary;

  // 循环分析每一个字符
  auto current = cbegin(code);
  while (current != cend(code)) {

    // 如果是标识符或者关键字 (下划线或者字母打头)
    if (this->isIdentifierHead(*current)) {
      // 将字符保存到临时存放点中，一直读到标识符结束
      while (this->isIdentifierBody(*current)) vocabulary += *current++;
      // 如果是符合条件的关键字，就执行替换
      if (this->needToReplace(vocabulary)) resultCode += this->mReplaceTable.at(vocabulary);
      // 否则不需要替换
      else resultCode += vocabulary;
      // 清除临时存放点
      vocabulary.clear();
    }

    // 如果是其他单个符号替换
    else if (this->needToReplace(std::move(std::string{*current}))) {
      // 执行替换
      resultCode += this->mReplaceTable.at(std::move(std::string{*current++}));
    }

    // 其他情况
    else resultCode += *current++;

  }

  // 返回替换后的代码
  return resultCode;

}

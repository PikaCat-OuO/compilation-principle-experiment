#include "codeprocessor.h"

// ���캯��
CodeProcessor::CodeProcessor() {

  // ��ʼ��Ĭ�Ϲؼ���
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

  // ��ʼ���滻��
  this->setReplaceTable({
      {"import", "include"},
      {"[", "{"},
      {"]", "}"},
      {"{", "["},
      {"}", "]"}
  });

}


// ���ùؼ���
void CodeProcessor::setKeywords(const std::vector<std::string> &keywords) {
  this->mKeywords = keywords;
}

// ��ȡ�ؼ���
const std::vector<std::string> &CodeProcessor::getKeywords() const {
  return this->mKeywords;
}

// �����滻��
void CodeProcessor::setReplaceTable(const std::unordered_map<std::string, std::string> &replaceTable) {
  this->mReplaceTable = replaceTable;
}

// ��ȡ�滻��
const std::unordered_map<std::string, std::string> &CodeProcessor::getReplaceTable() const {
  return this->mReplaceTable;
}


// �Ƿ��������ַ�
bool CodeProcessor::isUselessChar(const char ch) const {
  return ch == ' ' or ch == '\t' or ch == '\n';
}


// �Ƿ��ǹؼ��ֻ��߱�ʶ��ͷ
bool CodeProcessor::isIdentifierHead(const char ch) const {
  return ch == '_' or isalpha(ch);
}

// �Ƿ��ǹؼ��ֻ��߱�ʶ��ͷ
bool CodeProcessor::isIdentifierBody(const char ch) const {
  return isIdentifierHead(ch) or isdigit(ch);
}


// �Ƿ��ǹؼ���
bool CodeProcessor::isKeyword(const std::string_view identifier) const {
  for (const auto & keyword : this->mKeywords) {
    if (identifier == keyword) return true;
  }
  return false;
}


// �ж��Ƿ���������
bool CodeProcessor::isDigitBody(const char ch) const {
  return isdigit(ch) or ch == '.';
}


// �Ƿ�����Ҫ�滻�ĵ���
bool CodeProcessor::needToReplace(const std::string_view vocabulary) const {
  for (const auto& [key, value] : this->mReplaceTable) {
    if (vocabulary == key) return true;
  }
  return false;
}

// ��������
std::vector<std::pair<std::string, std::string>>
CodeProcessor::analyzeCode(const std::string_view code) const {

  // ����ֵ
  std::vector<std::pair<std::string, std::string>> result;

  // ��ʱ��ŵ�
  std::string vocabulary;
  std::string type;

  // ѭ������ÿһ���ַ�
  const char *current {cbegin(code)};
  while (current != cend(code)) {

    // ���Կո񣬻��з����Ʊ��
    while (this->isUselessChar(*current) and current != end(code)) ++current;
    if (current == end(code)) break;

    // ����Ǳ�ʶ�����߹ؼ���ͷ (�»��߻�����ĸ��ͷ)
    if (this->isIdentifierHead(*current)) {
      // ���ַ����浽��ʱ��ŵ��У�һֱ������ʶ�����߹ؼ��ֽ���
      while (this->isIdentifierBody(*current)) vocabulary += *current++;
      // �������
      type = (this->isKeyword(vocabulary) ? "�ؼ���" : "��ʶ��");
      // ����ؼ�����include��Ҫ�����⴦��
      if (vocabulary == "include") {
        // �Ƚ����ڵĽ����������
        result.emplace_back(move(vocabulary), move(type));
        // �ҵ���һ��<
        while (*current != '<') ++current;
        // ������һ���ļ���
        while (*current != '>') vocabulary += *current++;
        // �������һ��>
        vocabulary += *current++;
        // �������
        type = "��ʶ��";
      }
    }

    // ���������
    else if (isdigit(*current)) {
      // ���ַ����浽��ʱ��ŵ��У�һֱ�������ֽ���
      while (this->isDigitBody(*current)) vocabulary += *current++;
      // ������м�����x����X��b����B
      if (tolower(*current) == 'x' or tolower(*current) == 'b') {
        vocabulary += *current++;
        // �������պ��������
        while (this->isDigitBody(*current)) vocabulary += *current++;
      }
      // ���п�ѧ������
      if (tolower(*current) == 'e') {
        // ���E�������+-��Ҳ��
        if (*current == '+' or *current == '-') vocabulary += *current++;
        // Ȼ����պ��������
        while (isdigit(*current)) vocabulary += *current++;
      }
      // �������ֺ����ul,ull
      if (tolower(*current) == 'u' and tolower(*(current + 1)) == 'l') {
        vocabulary += *current++;
        vocabulary += *current++;
        if (tolower(*current) == 'l') vocabulary += *current++;
      }
      // ����f
      else if (tolower(*current) == 'f') vocabulary += *current++;
      // ����l��ll
      else if (tolower(*current) == 'l') {
        vocabulary += *current++;
        if (tolower(*current) == 'l') vocabulary += *current++;
      }
      // �������
      type = "��ֵ����";
    }

    // ���������switch����
    else {
      switch(*current) {
      // ������ַ�������
      case '\'':
        vocabulary += *current++;
        // ֱ��������һ��'
        while (*current != '\'') {
          // ����ת���ַ��������ֻ����\\��\'��\"�������
          if (*current == '\\' and
              (*(current + 1) == '\'' or
               *(current + 1) == '\"' or
               *(current + 1) == '\\')) ++current;
          vocabulary += *current++;
        }
        // �����һ��'Ҳ��������
        vocabulary += *current++;
        // �������
        type = "�ַ�����";
        break;

      // ������ַ���������
      case '\"':
        vocabulary += *current++;
        // ֱ��������һ��"
        while (*current != '\"') {
          // ����ת���ַ��������ֻ����\\��\'��\"�������
          if (*current == '\\' and
              (*(current + 1) == '\'' or
               *(current + 1) == '\"' or
               *(current + 1) == '\\')) ++current;
          vocabulary += *current++;
        }
        // �����һ��"Ҳ��������
        vocabulary += *current++;
        // �������
        type = "�ַ�������";
        break;

      // �����=��
      case '=':
        vocabulary += *current++;
        // ����==�����
        if (*current == '=') vocabulary += *current++;
        // �������
        type = "�����";
        break;

      // �����+��
      case '+':
        vocabulary += *current++;
        // ����+=��++�����
        if (*current == '=' || *current == '+') vocabulary += *current++;
        // �������
        type = "�����";
        break;

      // �����-��
      case '-':
        vocabulary += *current++;
        // ����-=��--��->�����
        if (*current == '=' || *current == '-' || *current == '>') vocabulary += *current++;
        // ����->*�����
        if (*(current - 1) == '>' and *current == '*') vocabulary += *current++;
        // �������
        type = "�����";
        break;

      // �����*��
      case '*':
        vocabulary += *current++;
        // ����*=�����
        if (*current == '=') vocabulary += *current++;
        // �������
        type = "�����";
        break;

      // �����/��
      case '/':
        // ����//����ע��
        if (*(current + 1) == '/') {
          // ֱ���������з���������ע��
          while (*current != '\n') vocabulary += *current++;
          // �������
          type = "����ע��";
        }
        // ����/**/����ע��
        else if (*(current + 1) == '*') {
          vocabulary += "/*";
          current += 2;
          // ֱ������*/��������ע��
          while (*current != '*' and *(current + 1) != '/') vocabulary += *current++;
          // ��*/Ҳ�����������������
          vocabulary += "*/";
          current += 2;
          type = "����ע��";
        }
        // �����������һ������
        else {
          vocabulary += *current++;
          // �������
          type = "�����";
        }
        break;

      // �����%
      case '%':
        vocabulary += *current++;
        // ����%=�����
        if (*current == '=') vocabulary += *current++;
        // �������
        type = "�����";
        break;

      // �����&
      case '&':
        vocabulary += *current++;
        // ����&&��&=�����
        if (*current == '&' || *current == '=') vocabulary += *current++;
        // �������
        type = "�����";
        break;

      // �����'|'
      case '|':
        vocabulary += *current++;
        // ����||��|=�����
        if (*current == '|' || *current == '=') vocabulary += *current++;
        // �������
        type = "�����";
        break;

      // �����!
      case '!':
        vocabulary += *current++;
        // ����!=�����
        if (*current == '=') vocabulary += *current++;
        // �������
        type = "�����";
        break;

      // �����^
      case '^':
        vocabulary += *current++;
        // ����^=�����
        if (*current == '=') vocabulary += *current++;
        // �������
        type = "�����";
        break;

      // �����<
      case '<':
        vocabulary += *current++;
        // ����<=�����
        if (*current == '=') vocabulary += *current++;
        else if (*current == '<') {
          // ����<<�����
          vocabulary += *current++;
          // ����<<=�����
          if (*current == '=') vocabulary += *current++;
        }
        // �������
        type = "�����";
        break;

      // �����>
      case '>':
        vocabulary += *current++;
        // ����>=�����
        if (*current == '=') vocabulary += *current++;
        else if (*current == '>') {
          // ����>>�����
          vocabulary += *current++;
          // ����>>=�����
          if (*current == '=') vocabulary += *current++;
        }
        // �������
        type = "�����";
        break;

      // �����.
      case '.':
        vocabulary += *current++;
        // ����.*�����
        if (*current == '*') vocabulary += *current++;
        // ����...�����
        if (*current == '.' and *(current + 1) == '.') {
          vocabulary += "..";
          current += 2;
        }
        // �������
        type = "�����";
        break;

      // �����:
      case ':':
        vocabulary += *current++;
        // ����::�����
        if (*current == ':') vocabulary += *current++;
        // �������
        type = "�����";
        break;

      // �����ַ�
      default:
        vocabulary += *current++;
        type = "�������";
      }

    }

    // ���Ա���������
    result.emplace_back(std::move(vocabulary), std::move(type));
  }

  // ��template���������<>��������滻Ϊ�������
  for (auto iter {begin(result)}; iter != end(result); ++iter) {
    if (iter->first == "template") {
      // �ҵ���һ��<
      while (iter->first != "<") ++iter;
      // �ҵ��ˣ���������滻Ϊ�������
      iter->second = "�������";
      // �ҵ���һ��>
      while (iter->first != ">") ++iter;
      // �ҵ��ˣ���������滻Ϊ�������
      iter->second = "�������";
    }
  }

  // ���������
  return result;

}


/* �����滻�����Լ����Ĵ����滻��ԭ����C++����
   * �﷨import -> include
   * { } -> [ ]
   * [ ] -> { }
  */
std::string CodeProcessor::replaceCode(const std::string_view code) const {

  // Ҫ���صľ��������Ĵ���
  std::string resultCode;

  // ��ʱ��ŵ�
  std::string vocabulary;

  // ѭ������ÿһ���ַ�
  auto current = cbegin(code);
  while (current != cend(code)) {

    // ����Ǳ�ʶ�����߹ؼ��� (�»��߻�����ĸ��ͷ)
    if (this->isIdentifierHead(*current)) {
      // ���ַ����浽��ʱ��ŵ��У�һֱ������ʶ������
      while (this->isIdentifierBody(*current)) vocabulary += *current++;
      // ����Ƿ��������Ĺؼ��֣���ִ���滻
      if (this->needToReplace(vocabulary)) resultCode += this->mReplaceTable.at(vocabulary);
      // ������Ҫ�滻
      else resultCode += vocabulary;
      // �����ʱ��ŵ�
      vocabulary.clear();
    }

    // ������������������滻
    else if (this->needToReplace(std::move(std::string{*current}))) {
      // ִ���滻
      resultCode += this->mReplaceTable.at(std::move(std::string{*current++}));
    }

    // �������
    else resultCode += *current++;

  }

  // �����滻��Ĵ���
  return resultCode;

}

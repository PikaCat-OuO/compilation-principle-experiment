#pragma once
#include <bits/stdc++.h>

// ���봦������Ĭ�ϴ���C++����
class CodeProcessor
{
public:

  // ���캯��
  CodeProcessor();

  // ���ùؼ���
  void setKeywords(const std::vector<std::string> &keywords);
  // ��ȡ�ؼ���
  const std::vector<std::string> &getKeywords() const;

  // �����滻��
  void setReplaceTable(const std::unordered_map<std::string, std::string> &replaceTable);
  // ��ȡ�滻��
  const std::unordered_map<std::string, std::string> &getReplaceTable() const;

  // ��������
  std::vector<std::pair<std::string, std::string> > analyzeCode(const std::string_view code) const;

  /* �����滻�����Լ����Ĵ����滻��ԭ����C++����
   * �﷨import -> include
   * { } -> [ ]
   * [ ] -> { }
  */
  std::string replaceCode(const std::string_view code) const;

protected:

  // �Ƿ��������ַ�
  bool isUselessChar(const char ch) const;

  // �Ƿ��Ǳ�ʶ����ؼ���ͷ
  bool isIdentifierHead(const char ch) const;
  // �Ƿ��Ǳ�ʶ����ؼ�����
  bool isIdentifierBody(const char ch) const;
  // �Ƿ��ǹؼ���
  bool isKeyword(const std::string_view identifier) const;

  // �Ƿ���������
  bool isDigitBody(const char ch) const;

  // �Ƿ�����Ҫ�滻�ĵ���
  bool needToReplace(const std::string_view vocabulary) const;

private:

  // ���������еĹؼ���
  std::vector<std::string> mKeywords;

  // �滻��
  std::unordered_map<std::string, std::string> mReplaceTable;

};

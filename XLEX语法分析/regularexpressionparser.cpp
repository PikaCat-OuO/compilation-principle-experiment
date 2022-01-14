#include "regularexpressionparser.h"

namespace RegularExpressionParser {
// 预先替换，为支持[]
void PreProcess(QString &regularExpression) {
  // 第一遍处理[0-9a-zA-Z]这类情况
  qsizetype index { 0 };

  // 一直寻找'-'，直到找不到
  while ((index = regularExpression.indexOf('-', index)) != -1) {
    // 找到了，取出左右的字符
    char begin { regularExpression[index - 1].toLatin1() };
    char end { regularExpression[index + 1].toLatin1() };

    // 构造即将替换进去的字符串
    QString sequence;
    while (++begin != end) sequence += begin;

    // 替换-符号
    regularExpression.replace(index, 1, sequence);
  }

  // 第二遍处理[0123456789]这类情况
  for (index = 0; index < regularExpression.size(); ++index) {
    // 找到左方括号
    if (regularExpression[index] == '[') {
      // 将[替换为(
      regularExpression.replace(index, 1, '(');

      // 一直加入'|'直到遇见右方括号
      index += 2;
      while (regularExpression[index] != ']') {
        regularExpression.insert(index, '|');
        index += 2;
      }

      // 将]替换为)
      regularExpression.replace(index, 1, ')');
    }
  }
}

// 是否是普通字符
bool IsNormal(const QChar &ch) {
  return ch != '|' && ch != '(' && ch != ')' && ch != '*' && ch != '+' && ch != '?';
}

// 是否属于需要添加连接符的情况
bool NeedToConnect(const QChar &left, const QChar &right) {
  // aa )a *a +a ?a a( )( *( +( ?(
  return (IsNormal(left) or left == ')' or left == '*' or left == '+' or left == '?') and
         (IsNormal(right) or right == '(');
}

// 为正则表达式添加连接符'&'
void AddConnectSign(QString &regularExpression) {
  for (qsizetype index { 0 }; index < regularExpression.size() - 1; ++index) {
    // 检查两个字符是否需要添加连接符
    if (NeedToConnect(regularExpression[index], regularExpression[index + 1])) {
      regularExpression.insert(++index, '&');
    }
  }
}

// 是否是普通字符
bool IsTransient(const QChar &ch) {
  return ch != '&' && ch != '|' && ch != '$' && ch != '(' && ch != ')';
}

// 获得当前运算符的优先级
quint8 GetOperatorPriority(const QChar &op) {
  switch (op.toLatin1()) {
  case '&': return 1;
  case '|': return 2;
  case '$': return 3;
  case '#': return 4;
  default: return 0;
  }
}

// 当前操作符是否可以入栈
bool IsPushable(const QChar &op, const QStack<QChar> &operatorStack) {
  QChar top = operatorStack.top();
  // 左括号可以被任何字符接纳，也可以接纳任何字符
  if (op == '(' or top == '(') return true;
  // 右括号要遇见左括号才可以接纳
  if (op == ')') return top == '(';
  return GetOperatorPriority(op) < GetOperatorPriority(top);
}

void MedialToSuffix(QString &regularExpression) {
  // 添加尾部哨兵
  regularExpression += '$';

  // 操作符暂存栈
  QStack<QChar> operatorStack;
  // 添加栈底哨兵
  operatorStack.push('#');

  // 后缀表达式
  QString result;


  for (const auto &current : regularExpression) {
    // 如果是字符或者*就直接添加到后缀表达式中
    if (IsTransient(current)) result += current;
    // 如果是操作符并且当前操作符的优先级低于栈顶符号，直接入栈
    else if (IsPushable(current, operatorStack)) operatorStack.push(current);
    else {
      // 否则就一直弹栈，直到自己的优先级低于栈顶符号
      while (not IsPushable(current, operatorStack)) result += operatorStack.pop();
      // 如果不是右括号就可以入栈了
      if (current != ')') operatorStack.push(current);
      // 如果是右括号就把栈顶的左括号弹出
      else operatorStack.pop();
    }
  }

  // 保存结果
  regularExpression = result;
}
}

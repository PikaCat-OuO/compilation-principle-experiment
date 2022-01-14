#pragma once
#include <QString>
#include <QMap>
#include <QSet>
#include <QStack>

// NFA机器的状态
using State = quint64;

constexpr QChar Epsilon { L'ε' };

class NFAMachine
{
public:
  NFAMachine();
  // 构造用一个input连接的NFA并在转换表中记录
  NFAMachine(const QChar &input);
  // 用两台NFA机器的连接(&和|)构造一个新的NFA
  NFAMachine(NFAMachine &&beginState, const QChar &op, NFAMachine &&endState);
  // 用后缀正则表达式构造NFA
  NFAMachine(QStringView regularExpression);

  // 重置NFA机器
  void static reset();

  // 获取起始状态
  State getBeginState() const;
  // 获取结束状态
  State getEndState() const;

  // NFA机器的状态计数器
  inline static quint64 ms_stateCounter { 0 };

  // NFA机器的状态转换表
  inline static QMap<State, QMap<QChar, QSet<State>>> ms_stateTransferTable;

protected:
  // 判断是否是普通字符
  static bool isTransient(const QChar &ch);

  // 用ε连接两个NFA机器状态节点
  static void epsilonConnectState(State firstState, State secondState);

private:
  // NFA机器的起始和结束的位置
  State m_beginState, m_endState;
};

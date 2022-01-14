#include "dfamachine.h"

namespace DFAMachine {
// DFA的开始状态和结束状态集合
State BeginState { 0 };
QSet<State> EndStates;

// DFA的状态转换表
QMap<State, QMap<QChar, State>> StateTransferTable;

// 获得一个对NFA机器的状态转换表的引用
constexpr auto &NFAStateTransferTable = NFAMachine::ms_stateTransferTable;

// 求某个状态集合的ε闭包
QSet<State> CalcEpsilonClosure(QSet<State> &&epsilonClosure) {
  // 待遍历的状态
  QStack<State> toExplore;

  // 初始状态加入栈中
  for (const auto &state : epsilonClosure) toExplore.push(state);

  // 逐个遍历直到栈空
  while (not toExplore.isEmpty()) {
    State currentState = toExplore.pop();

    // 寻找与当前状态ε相连的状态
    if (NFAStateTransferTable[currentState].contains(Epsilon)) {
      for (const auto &nextState : NFAStateTransferTable[currentState][Epsilon]) {
        // 确保状态不在闭包中才将其加入闭包并入栈
        if (not epsilonClosure.contains(nextState)) {
          epsilonClosure.insert(nextState);
          toExplore.push(nextState);
        }
      }
    }
  }

  // 返回ε闭包
  return qMove(epsilonClosure);
}

// 求某个ε闭包可以经过哪一些非ε转换
QSet<QChar> CalcNonEpsilonTransfers(const QSet<State> &states) {
  QSet<QChar> result;

  for (const auto &state : states) {
    for (const auto &transfer : NFAStateTransferTable[state].keys()) result.insert(transfer);
  }

  // 去除ε转换
  result -= L'ε';

  // 返回结果
  return result;
}

// 求某个ε闭包的move集
QSet<State> CalcMoveSet(const QSet<State> &states, const QChar &input) {
  // move集
  QSet<State> result;

  for (const auto &state : states) {
    // 查看是否能通过input进入其他状态
    if (NFAStateTransferTable[state].contains(input)) {
      // 如果可以就将他们并到move集合中
      result |= NFAStateTransferTable[state][input];
    }
  }

  return result;
}

// 求某个ε闭包的move集的ε闭包
QSet<State> CalcMoveEpsilonClosure(const QSet<State> &states, const QChar &input) {
  return CalcEpsilonClosure(CalcMoveSet(states, input));
}

// 重置DFA机器
void Reset() {
  DFAMachine::StateTransferTable.clear();
  DFAMachine::BeginState = 0;
  DFAMachine::EndStates.clear();
}

// NFA转DFA
void NFAToDFA(const NFAMachine &NFAMachine) {
  // DFA机器的状态集合
  QVector<QSet<State>> DFAStates { CalcEpsilonClosure(QSet { NFAMachine.getBeginState() }) };

  // 循环一直到DFA状态集合不再增长
  for (qsizetype currentState { 0 }; currentState < DFAStates.size(); ++currentState) {
    // 对于某一个ε闭包，先求里面的状态可以走哪一些非ε转换
    QSet<QChar> transfers { CalcNonEpsilonTransfers(DFAStates[currentState]) };

    // 逐一遍历这些转换
    for (const auto &transfer : transfers) {
      // 计算move集的ε闭包
      QSet<State> epsilonClosure { CalcMoveEpsilonClosure(DFAStates[currentState], transfer) };

      // 检查这个ε闭包在不在DFAStates中，如果不在就添加到其中
      if (not DFAStates.contains(epsilonClosure)) DFAStates.emplaceBack(epsilonClosure);

      // 将这一步添加到DFA状态表中
      StateTransferTable[currentState][transfer] = DFAStates.indexOf(epsilonClosure);
    }
  }

  // 已经遍历完了，最后还要检查这个闭包是否包含了NFA的结束状态，是就将其加入到DFA的结束状态中
  for (qsizetype currentState { 0 }; currentState < DFAStates.size(); ++currentState) {
    if (DFAStates[currentState].contains(NFAMachine.getEndState())) {
      EndStates.insert(currentState);
    }
  }
}
}

#include "mfamachine.h"

namespace MFAMachine {
// 定义空集的表示状态
constexpr State EmptySet { std::numeric_limits<State>::max() };

// MFA的开始状态和结束状态集合
State BeginState;
QSet<State> EndStates;

// MFA的状态转换表
QMap<State, QMap<QChar, State>> StateTransferTable;

// 获得一个对DFA机器的状态转换表的引用
constexpr auto &DFAStateTransferTable = DFAMachine::StateTransferTable;

// 求某个状态集合可以经过哪一些转换
QSet<QChar> CalcTransfers(const QSet<State> &states) {
  QSet<QChar> result;

  // 计算转换结果
  for (const auto &state : states) {
    for (const auto &transfer : DFAStateTransferTable[state].keys()) result.insert(transfer);
  }

  // 返回结果
  return result;
}

// 求某个状态集合的move集
QSet<State> CalcMoveSet(const QSet<State> &states, const QChar &input) {
  // move集
  QSet<State> result;

  for (const auto &state : states) {
    // 查看是否能通过input进入其他状态，如果可以就将他们并到move集合中
    if (DFAStateTransferTable[state].contains(input)) {
      result |= DFAStateTransferTable[state][input];
    }
    // 为了处理空集
    else result |= EmptySet;
  }

  return result;
}

// 计算某一个集合的转换是不是MFAState的子集
bool IsTransferSubSetOfMFA(const QSet<State> &states, const QVector<QSet<State>> &MFAStates) {
  // 如果集合只有一个元素就直接跳过
  if (states.size() == 1) return true;

  // 首先获得这个状态上可以做的转换
  QSet<QChar> transfers { CalcTransfers(states) };

  for (const auto &transfer : transfers) {
    // 先获得转换后的状态
    QSet<State> nextStates { CalcMoveSet(states, transfer) };

    // 如果集合只有一个元素，直接跳过
    if (nextStates.size() == 1) continue;

    // 查看这个状态下的转换是不是MFAStates某个集合的子集
    auto iter { std::find_if(MFAStates.begin(), MFAStates.end(),
                [&](const QSet<State> &MFAState) { return MFAState.contains(nextStates); }) };

    // 如果这个转换下的状态不是MFAStates中任何一个集合的子集
    if (iter == MFAStates.end()) return false;
  }

  // 通过了所有的测试
  return true;
}

// 分割一个不满足条件的MFA
QVector<QSet<State>> SplitState(QSet<State> statesToSplit,
                                const QVector<QSet<State>> &MFAStates) {
  // 用于保存返回结果
  QVector<QSet<State>> result;

  // 还有需要分组的数据
  while (not statesToSplit.isEmpty()) {
    // 用于检测的临时集合
    QSet<State> testStates;

    // 一个一个加入，直到出现错误为止
    for (const auto &state : statesToSplit) {
      testStates.insert(state);

      // 如果插入该状态导致MFA失败，就回滚并留到其他批次处理
      if (not IsTransferSubSetOfMFA(testStates, MFAStates)) testStates.remove(state);
    }

    // 处理完成，移除已分组的状态
    statesToSplit -= testStates;

    // 将结果放入数组中
    result.emplaceBack(qMove(testStates));
  }

  // 返回结果
  return result;
}

// 根据MFAStates构造MFA转换表
void BuildMFATransferTable(const QVector<QSet<State>> &MFAStates) {
  for (qsizetype currentState { 0 }; currentState < MFAStates.size(); ++currentState) {
    const QSet<State> &MFAState = MFAStates[currentState];

    // 首先获得这个状态上可以做的转换
    QSet<QChar> transfers { CalcTransfers(MFAState) };

    // 每一个转换都做一下，并将最后的结果加入MFA转换表
    for (const auto &transfer : transfers) {
      // 寻找子集所属下标
      QSet<State> subSet { CalcMoveSet(MFAState, transfer) - QSet<State> { EmptySet } };
      auto iter { std::find_if(MFAStates.begin(), MFAStates.end(),
                  [&](const QSet<State> &MFAState) { return MFAState.contains(subSet); }) };
      if (iter != MFAStates.end()) {
        StateTransferTable[currentState][transfer] = std::distance(MFAStates.begin(), iter);
      }
    }

    // 设置MFA的开始和结束状态
    if (MFAState.contains(DFAMachine::BeginState)) BeginState = currentState;
    if (DFAMachine::EndStates.contains(MFAState)) EndStates.insert(currentState);
  }
}

// 重置MFA机器
void Reset() {
  MFAMachine::StateTransferTable.clear();
  MFAMachine::BeginState = 0;
  MFAMachine::EndStates.clear();
}

// DFA转MFA
void DFAToMFA() {
  // 所有的状态
  QSet<State> allStates;

  // 将0到n填充到集合中
  for (qsizetype state { 0 }; state < DFAMachine::StateTransferTable.size(); ++state) {
    allStates.insert(state);
  }

  // MFA的状态集合，根据是否是终止状态划分
  QVector<QSet<State>> MFAStates { allStates - DFAMachine::EndStates, DFAMachine::EndStates};

  // 从MFA的状态集合中移除空的状态集合
  MFAStates.removeIf([&](const QSet<State> &states) { return states.isEmpty(); });

  // 当前MFA的状态个数
  qsizetype stateCounter { 0 };

  // 逐个检查是否符合划分条件，如果MFA状态集合变动了就重来
  while (stateCounter != MFAStates.size()) {
    stateCounter = MFAStates.size();

    for (auto &MFAState : MFAStates) {
      // 检查一个集合是否符合MFA要求
      if (not IsTransferSubSetOfMFA(MFAState, MFAStates)) {
        // 进行集合拆分
        QVector<QSet<State>> result { SplitState(MFAState, MFAStates) };
        // 从当前集合中移除MFAState这个状态
        MFAStates.removeOne(MFAState);
        // 将result中的分好的集合加入其中
        for (const auto &states : result) MFAStates.emplaceBack(qMove(states));
        // 重新进行检测
        break;
      }
    }
  }

  // 转换完成，开始构造MFA转换表
  BuildMFATransferTable(MFAStates);
}
}

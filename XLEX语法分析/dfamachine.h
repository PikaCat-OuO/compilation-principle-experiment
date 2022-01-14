#pragma once
#include "nfamachine.h"

namespace DFAMachine{
// DFA的开始状态和结束状态集合
extern State BeginState;
extern QSet<State> EndStates;

// DFA的状态转换表
extern QMap<State, QMap<QChar, State>> StateTransferTable;

// 重置DFA机器
void Reset();

// NFA转DFA
void NFAToDFA(const NFAMachine &NFAMachine);
}

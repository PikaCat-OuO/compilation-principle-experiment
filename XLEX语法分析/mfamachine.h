#pragma once
#include "dfamachine.h"

namespace MFAMachine {
// MFA的开始状态和结束状态集合
extern State BeginState;
extern QSet<State> EndStates;

// MFA的状态转换表
extern QMap<State, QMap<QChar, State>> StateTransferTable;

// 重置MFA机器
void Reset();

// DFA转MFA
void DFAToMFA();
}

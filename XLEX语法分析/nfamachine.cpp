#include "nfamachine.h"

NFAMachine::NFAMachine() { }

// 构造用一个input连接的NFA并在转换表中记录
NFAMachine::NFAMachine(const QChar &input) {
  this->m_beginState = ms_stateCounter++;
  this->m_endState = ms_stateCounter++;
  ms_stateTransferTable[this->m_beginState][input].insert(this->m_endState);
}

// 用两台NFA机器的连接(&和|)构造一个新的NFA
NFAMachine::NFAMachine(NFAMachine &&first, const QChar &op, NFAMachine &&second) {
  switch(op.toLatin1()) {
  // 尾头相连
  case '&':
    epsilonConnectState(first.m_endState, second.m_beginState);
    this->m_beginState = first.m_beginState;
    this->m_endState = second.m_endState;
    break;

  // 前后创建新的节点，头头连，尾尾连
  case '|':
    this->m_beginState = ms_stateCounter++;
    this->m_endState = ms_stateCounter++;
    epsilonConnectState(this->m_beginState, first.m_beginState);
    epsilonConnectState(this->m_beginState, second.m_beginState);
    epsilonConnectState(first.m_endState, this->m_endState);
    epsilonConnectState(second.m_endState, this->m_endState);
    break;

  // 其他情况抛异常
  default:
    throw "不支持的操作符";
  }
}

// 用后缀正则表达式构造NFA
NFAMachine::NFAMachine(QStringView regularExpression) {
  // 存放构造中产生的NFA
  QStack<NFAMachine> NFAstack;

  for (const auto &input : regularExpression) {
    // 如果是普通字符就构造一台NFA
    if (isTransient(input)) NFAstack.emplaceBack(input);

    // 如果是闭包符号就执行闭包操作，头连尾，尾连头
    else if (input == '*') {
      const NFAMachine &top = NFAstack.top();
      epsilonConnectState(top.m_beginState, top.m_endState);
      epsilonConnectState(top.m_endState, top.m_beginState);
    }

    // 如果是?就执行?操作，头连尾
    else if (input == '?') {
      const NFAMachine &top = NFAstack.top();
      epsilonConnectState(top.m_beginState, top.m_endState);
    }

    // 如果是+就执行+操作，尾连头
    else if (input == '+') {
      const NFAMachine &top = NFAstack.top();
      epsilonConnectState(top.m_endState, top.m_beginState);
    }

    // 其余运算符就构造新的NFA
    else {
      NFAMachine secondNFA = NFAstack.pop();
      NFAMachine firstNFA = NFAstack.pop();
      NFAstack.emplaceBack(qMove(firstNFA), input, qMove(secondNFA));
    }
  }

  // 将栈底的NFA就是目标NFA
  *this = NFAstack.pop();
}

// 重置NFA机器
void NFAMachine::reset() {
  NFAMachine::ms_stateTransferTable.clear();
  NFAMachine::ms_stateCounter = 0;
}

// 获取起始状态
State NFAMachine::getBeginState() const { return this->m_beginState; }

// 获取结束状态
State NFAMachine::getEndState() const { return this->m_endState; }

// 判断是否是普通字符
bool NFAMachine::isTransient(const QChar &ch) {
  return ch != '*' && ch != '?' && ch != '+' && ch != '&' && ch != '|';
}

// 用ε连接两个NFA机器状态节点
void NFAMachine::epsilonConnectState(State firstState, State secondState) {
  ms_stateTransferTable[firstState][Epsilon].insert(secondState);
}

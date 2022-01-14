#include "regularexpressionengine.h"

namespace RegularExpressionEngine {
// 重置引擎的状态
void resetEngine() {
  NFAMachine::reset();
  DFAMachine::Reset();
  MFAMachine::Reset();
}

// 正则表达式预处理
void RegularExpressionPreProcess(QString &regularExpression) {
  // 重置引擎的状态
  resetEngine();

  // 对正则表达式进行预处理
  RegularExpressionParser::PreProcess(regularExpression);

  // 添加连接符
  RegularExpressionParser::AddConnectSign(regularExpression);

  // 中缀转后缀
  RegularExpressionParser::MedialToSuffix(regularExpression);
}

// 正则表达式处理为NFA
NFAMachine RegularExpressionToNFA(QString &regularExpression) {
  // 先预处理正则表达式
  RegularExpressionPreProcess(regularExpression);

  // 转化为NFA
  return { regularExpression };
}

// 正则表达式处理为DFA
void RegularExpressionToDFA(QString &regularExpression) {
  // 先转化为NFA
  NFAMachine NFAMachine { RegularExpressionToNFA(regularExpression) };

  // 转化为DFA
  DFAMachine::NFAToDFA(NFAMachine);
}

// 正则表达式处理为MFA
void RegularExpressionToMFA(QString &regularExpression) {
  // 先转化为DFA
  RegularExpressionToDFA(regularExpression);

  // 转化为MFA
  MFAMachine::DFAToMFA();
}

// 正则表达式生成代码
QString RegularExpressionToCode(QString &regularExpression) {
  // 先转化为MFA
  RegularExpressionToMFA(regularExpression);

  // 作代码生成
  return CodeGenerator::MFAToCode();
}
}

#pragma once
#include "regularexpressionparser.h"
#include "codegenerator.h"

namespace RegularExpressionEngine {
// 正则表达式预处理
void RegularExpressionPreProcess(QString &regularExpression);

// 正则表达式处理为NFA
NFAMachine RegularExpressionToNFA(QString &regularExpression);

// 正则表达式处理为DFA
void RegularExpressionToDFA(QString &regularExpression);

// 正则表达式处理为MFA
void RegularExpressionToMFA(QString &regularExpression);

// 正则表达式生成代码
QString RegularExpressionToCode(QString &regularExpression);
}

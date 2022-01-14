#include "codegenerator.h"

namespace CodeGenerator {
// 获得一个对MFA机器的状态转换表的引用
constexpr auto &MFAStateTransferTable = MFAMachine::StateTransferTable;

// 生成state的转换总代码
QString GenerateStateCase(const State currentState) {
  // 先生成这个case的代码
  QString code { QString { R"(
    case %1: switch (ch) {)" }.arg(currentState) };

  // 再生成这个case下所有的子节点的代码
  for (const auto &input : MFAStateTransferTable[currentState].keys()) {
    code += QString { R"(
      case '%1': state = %2; break;)" }.arg(input)
      .arg(MFAStateTransferTable[currentState][input]);
  }

  // 最后生成回环括号、错误处理流程和break;
  code += R"(
      default: return false; } break;
)";

  // 返回最终的代码
  return code;
}

// 生成接收状态的检测的总代码
QString GenerateAccept() {
  // 接收状态的检测的总代码
  QString code;

  // 对于每一个接收状态，分别添加处理代码
  for (const auto &acceptState : MFAMachine::EndStates) {
    code += QString { R"(
  if (state == %1) return true;)" }.arg(acceptState);
  }

  // 返回检查的总代码
  return code;
}

// 根据MFA的转换表生成代码
QString MFAToCode() {
  // 最终生成的代码，引入头文件，添加函数头部和状态机的状态
  QString code { QString { R"(#include <string>

bool Process(const std::string &str) {
  uint64_t state { %1 };

  for (const auto &ch : str) {
    switch (state) {)" }.arg(QString::number(MFAMachine::BeginState)) };

  // 添加状态处理流程
  for (const auto &state : MFAStateTransferTable.keys()) code += GenerateStateCase(state);

  // 生成回环括号
  code += R"(    }
  }
)";

  // 添加接收状态处理
  code += GenerateAccept();

  // 生成最后的接收处理和回环括号
  code += R"(

  return false;
})";

  // 返回处理好的代码
  return code;
}
}

#pragma once
#include <QString>
#include <QStack>

// 正则表达式解析器
namespace RegularExpressionParser
{
// 预先替换，为支持[],和+
void PreProcess(QString &regularExpression);

// 为正则表达式添加连接符
void AddConnectSign(QString &regularExpression);

// 中缀转后缀表达式
void MedialToSuffix(QString &regularExpression);
};

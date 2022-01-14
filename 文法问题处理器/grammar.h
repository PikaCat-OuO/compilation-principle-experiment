#pragma once
#include <QString>
#include <QMap>
#include <QSet>
#include <QStack>
#include <QRegularExpression>
#include <QDebug>

class Grammar
{
public:
  // 利用文法字符串构造文法，处理文法字符串
  Grammar(const QString &grammerString);

  // 消除文法的左递归，包括直接左递归和间接左递归
  void eliminateLeftRecursion();

  // 提取文法的左公因子
  void leftFactoring();

  // 计算文法的first集合元素、follow集合元素
  void calcFirstAndFollowSet();

  // 构造预测分析表
  void constructPredictTable();

  // 执行最左推导
  QPair<bool, QStringList> leftmostDerive(QString sentence);

  // 返回文法开始符号
  QChar getStart();

  // 返回文法所有的产生式
  QMap<QChar, QSet<QString>> getProduction();

  // 返回非终结符的first集合
  QMap<QChar, QSet<QString>> getFirstSet();

  // 返回非终结符的follow集合
  QMap<QChar, QSet<QChar>> getFollowSet();

  // 打印文法
  void printGrammar();

protected:
  // 判断一个符号是不是非终结符
  bool isNonTerminator(const QChar &ch);
  // 判断一个符号是不是终结符
  bool isTerminator(const QChar &ch);

  // 判断一个产生式的右部是不是可以终结
  bool isProductionRightTerminable(const QSet<QChar> &nonTerminators,
                                          const QString &productionRight);
  // 判断一个产生式的右部是不是可以到达
  bool isProductionRightReachable(const QSet<QChar> &nonTerminators,
                                         const QSet<QChar> &terminators,
                                         const QString &productionRight);

  // 化简文法，去除有害规则，消除文法中的无用符号和无用产生式
  void simplifyGrammar();

  // 获得一个未使用过的非终结符
  QChar getNewNonTerminator();

  // 将一个产生式右部中的第一个符号用replacer的产生式替换
  QSet<QString> replace(const QChar &replacer, const QString &productionRight);
  // 将文法规则中replacer为左部的产生式代入replacee为左部的产生式中
  void replace(const QChar &replacer, const QChar &replacee);
  // 替换最左边的非终结符直到出现终结符为止
  QSet<QString> replaceFirst(const QString &productionRight);

  // 获取一个产生式右部集合的前缀
  QSet<QChar> extractPrefixes(const QSet<QString> &productionRights);
  // 寻找一个前缀的所有产生式集合
  QSet<QString> findSuffixes(const QChar &prefix, const QSet<QString> &productionRights);

  // 求某一个非终结符的first集合
  QSet<QString> getFirstSet(const QChar &nonTerminator);
  // 求某一个字符串的first集合
  QSet<QString> getFirstSet(const QString &productionRight);

private:
  // 文法非终结符
  QSet<QChar> m_nonTerminators;
  // 文法终结符
  QSet<QChar> m_terminators;
  // 文法产生式
  QMap<QChar, QSet<QString>> m_productions;
  // 文法开始符号
  QChar m_start;

  // first集合元素
  QMap<QChar, QMap<QString, QSet<QString>>> m_firstSet;
  // follow集合元素
  QMap<QChar, QSet<QChar>> m_followSet;

  // 预测分析表
  QMap<QChar, QMap<QChar, QString>> m_predictTable;
};

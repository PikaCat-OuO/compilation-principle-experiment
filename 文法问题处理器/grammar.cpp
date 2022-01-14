#include "grammar.h"

Grammar::Grammar(const QString &grammerString) {
  // 首先使用正则表达式检查语法错误
  QRegularExpression regex { R"(^G\[([A-Z])\s*\]\s*(:|：)\s*(
+\s*([A-Z]\s*->\s*([a-zA-Z]+|@)(\s*\|\s*([a-zA-Z]+|@))*)\s*)*
+\s*\1\s*->\s*([a-zA-Z]+|@)(\s*\|\s*([a-zA-Z]+|@))*\s*(
+\s*([A-Z]\s*->\s*([a-zA-Z]+|@)(\s*\|\s*([a-zA-Z]+|@))*)\s*)*
*$)" };
  if (not regex.match(grammerString).hasMatch()) throw QString { "输入的文法格式有误，请重新输入" };

  // 将文法字符串分割为一行一行的产生式
  QStringList productions { grammerString.split("\n") };

  // 提取文法开始符号
  this->m_start = productions[0].split("[")[1][0];
  productions.removeFirst();

  // 遍历这些产生式并加入到产生式映射中
  for (const auto &production : productions) {
    // 跳过多余的空行
    if ("" == production.simplified()) continue;
    // 将每一个产生式分割为左部和右部
    QStringList productionContent { production.split("->") };
    QChar productionLeft { productionContent[0].simplified()[0] };
    QStringList productionRights { productionContent[1].split("|") };
    // 注意：这里不将非终结符加入到非终结符集合中，因为待会在化简文法时会构造这个集合

    // 遍历产生式的右部，逐个加入到产生式映射中
    for (const auto &productionRight : productionRights) {
      // 如果产生式右部有终结符就将其加入到终结符集合中
      for (const auto &ch : productionRight) {
        if (isTerminator(ch)) this->m_terminators.insert(ch);
      }

      // 将产生式加入文法映射中，如果是空产生式就进行改造
      if ("@" == productionRight.simplified()) this->m_productions[productionLeft].insert("");
      else this->m_productions[productionLeft].insert(productionRight.simplified());
    }
  }

  // 做完后化简文法
  simplifyGrammar();
}

void Grammar::eliminateLeftRecursion() {
  // 先给非终结符规定一个顺序
  QList<QChar> nonTerminators;
  for (const auto &nonTerminator : this->m_nonTerminators) {
    nonTerminators.emplaceBack(nonTerminator);
  }

  // 按照这个顺序遍历所有的非终结符
  for (auto current { nonTerminators.begin() }; current not_eq nonTerminators.end(); ++current) {
    // 从前面往后面依次代入非终结符的产生式
    for (auto before { nonTerminators.begin() }; before not_eq current; ++before) {
      replace(*before, *current);
    }

    // 消除直接左递归，先定义不含左递归的产生式右部集合
    QSet<QString> nonLeftRecursionPart;

    // 定义包含左递归的产生式右部的集合
    QSet<QString> leftRecursionPart;

    // 遍历当前非终结符的所有产生式右部，区分包含与不包含左递归两部分
    for (const auto &productionRight : this->m_productions[*current]) {
      if ("" == productionRight or *current not_eq productionRight[0]) {
        nonLeftRecursionPart.insert(productionRight);
      }
      // 去除左递归对应的符号
      else leftRecursionPart.insert(productionRight.sliced(1));
    }

    // 如果这个非终结符的产生式不包含左递归，直接跳过
    if (leftRecursionPart.empty()) continue;

    // 否则就申请一个新的非终结符
    QChar newNonTerminator { getNewNonTerminator() };

    // 对于A -> Aa|b的形式，先添加B -> aB|ε这条产生式
    for (const auto &productionRight : leftRecursionPart) {
      this->m_productions[newNonTerminator].insert(productionRight + newNonTerminator);
    }
    this->m_productions[newNonTerminator].insert("");

    // 移除原来的产生式A -> Aa|b
    this->m_productions.remove(*current);

    // 再添加A -> bB这条产生式
    for (const auto &productionRight : nonLeftRecursionPart) {
      this->m_productions[*current].insert(productionRight + newNonTerminator);
    }
  }

  // 做完后化简文法
  simplifyGrammar();
}

void Grammar::leftFactoring() {
  QStack<QChar> nonTerminators;
  for (const auto &nonTerminator : this->m_productions.keys()) {
    nonTerminators.emplaceBack(nonTerminator);
  }
  while (not nonTerminators.empty()) {
    QChar productionLeft { nonTerminators.pop() };

    // 对于每一条形如A -> Ba|Ca|Da|Dc的产生式，将BCD带入直到产生式右部全部都是以终结符打头
    QSet<QString> replaceResult { };
    for (const auto &productionRight : this->m_productions[productionLeft]) {
      replaceResult |= replaceFirst(productionRight);
    }

    // 移除原有的结果
    this->m_productions.remove(productionLeft);

    // 如果替换结果中有空，先加入到提取公因子结果集中
    if (replaceResult.contains("")) this->m_productions[productionLeft].insert("");

    // 获取所有产生式的公共前缀
    QSet<QChar> commonPrefixes { extractPrefixes(replaceResult) };

    // 对于每一个公共前缀，找到属于这个公共前缀的后缀
    for (const auto &commonPrefix : commonPrefixes) {
      QSet<QString> suffixes { findSuffixes(commonPrefix, replaceResult) };

      // 只有长度大于1才能说明有多个后缀有这个公共前缀
      if (suffixes.size() > 1) {
        // 获得一个新的非终结符
        QChar newNonTerminator { getNewNonTerminator() };

        // 构造一个新的产生式来装这些后缀
        this->m_productions[newNonTerminator] = suffixes;

        // 同时修改原来的产生式
        this->m_productions[productionLeft].insert(QString { commonPrefix } +  newNonTerminator);

        // 因为新的产生式可能还是含有左公因子，因此继续放入栈中
        nonTerminators.emplaceBack(newNonTerminator);
      }
      // 其他的情况说明这个公共前缀只有一个后缀，这种情况就直接插入即可
      else this->m_productions[productionLeft].insert(commonPrefix + *suffixes.begin());
    }
  }

  // 做完后化简文法
  simplifyGrammar();
}

void Grammar::calcFirstAndFollowSet() {
  // 等待最后处理的follow等待任务
  QList<QPair<QChar, QChar>> pendingFollow;

  // 先在文法开始符号的follow集合中添加结束符号$
  this->m_followSet[this->m_start].insert('$');

  for (const auto &productionLeft : this->m_nonTerminators) {
    for (const auto &productionRight : this->m_productions[productionLeft]) {
      // 对于每一条产生式的右部，求它的first集合元素
      this->m_firstSet[productionLeft][productionRight] = getFirstSet(productionRight);

      // 求这个表达式里面的非终结符的follow集合元素
      for (qsizetype index { 0 }; index < productionRight.length(); ++index) {
        if (isNonTerminator(productionRight[index])) {
          // 获取后面字符的first集合
          QSet<QString> firstSet { getFirstSet(productionRight.sliced(index + 1)) };

          // 去除ε
          for (const auto &terminator : firstSet) {
            if ("" not_eq terminator) this->m_followSet[productionRight[index]] |= terminator[0];
          }

          // 如果存在ε就加入到等待任务中
          if (firstSet.contains("")) {
            pendingFollow.emplaceBack(productionLeft, productionRight[index]);
          }
        }
      }
    }
  }

  // 把剩下的等待任务完成，一直做到集合大小不再变化为止
  bool done { false };
  while (not done) {
    done = true;
    for (const auto &[from, to] : pendingFollow) {
      qsizetype size { this->m_followSet[to].size() };
      this->m_followSet[to] |= this->m_followSet[from];
      if (size not_eq this->m_followSet[to].size()) done = false;
    }
  }
}

void Grammar::constructPredictTable() {
  for (const auto &nonTerminator : this->m_nonTerminators) {
    for (const auto &productionRight : this->m_productions[nonTerminator]) {
      // 查找first集合元素，构造预测分析表
      for (const auto &terminator : this->m_firstSet[nonTerminator][productionRight]) {
        if ("" not_eq terminator) {
          if (not this->m_predictTable[nonTerminator].contains(terminator[0])) {
            this->m_predictTable[nonTerminator][terminator[0]] = productionRight;
          } else throw QString { "该文法不好，无法化简为LL(1)文法，请手动修改文法" };
        }

        // 如果first集中有空，那么需要加上follow集合元素
        else {
          for (const auto &followTerminator : this->m_followSet[nonTerminator]) {
            if (not this->m_predictTable[nonTerminator].contains(followTerminator)) {
              this->m_predictTable[nonTerminator][followTerminator] = productionRight;
            } else throw QString { "该文法不好，无法化简为LL(1)文法，请手动修改文法" };
          }
        }
      }
    }
  }
}

QPair<bool, QStringList> Grammar::leftmostDerive(QString sentence) {
  // 在句子的末尾加上结束符号
  sentence += '$';
  // 保存推导过程
  QStringList deriveProcedure;

  // 在栈中推入文法开始符号和结束符号
  QStack<QChar> stack;
  stack.emplaceBack('$');
  stack.emplaceBack(this->m_start);

  auto iter { sentence.cbegin() };
  while (not stack.empty() and iter not_eq sentence.cend()) {
    QChar current { stack.pop() };
    // 栈顶是终结符或者串结束符号
    if (isTerminator(current) or current == '$') {
      // 如果栈顶终结符与当前句子位置的符号相同则推导成功
      if (current == *iter) { ++iter; continue; }
      // 否则，推导失败
      else return { false, deriveProcedure };
    }

    // 栈顶是非终结符，判断预测分析表中是否存在该推导
    if (not this->m_predictTable[current].contains(*iter)) return { false, deriveProcedure };
    // 存在该推导，从预测分析表中找到这个符号对应的推导
    QString productionRight { this->m_predictTable[current][*iter] };
    // 把这个推导写入到推导过程中
    deriveProcedure.emplaceBack(QString { current } + "->" +
                                ("" == productionRight ? "@" : productionRight));

    // 将这个产生式右部颠倒放入栈中
    std::reverse(productionRight.begin(), productionRight.end());
    for (const auto &ch : productionRight) stack.emplaceBack(ch);
  }

  return { stack.empty() and iter == sentence.end(), deriveProcedure };
}

QChar Grammar::getStart() { return this->m_start; }

QMap<QChar, QSet<QString>> Grammar::getProduction() { return this->m_productions; }

QMap<QChar, QSet<QString> > Grammar::getFirstSet() {
  QMap<QChar, QSet<QString>> result;

  for (const auto &productionLeft : this->m_firstSet.keys()) {
    for (const auto &productionRights : this->m_firstSet[productionLeft]) {
      result[productionLeft] |= productionRights;
    }
  }

  return result;
}

QMap<QChar, QSet<QChar> > Grammar::getFollowSet() { return this->m_followSet; }

void Grammar::printGrammar() {
  // 打印文法头
  qDebug() << "G[" << this->m_start << "]:";
  // 打印非终结符集
  qDebug() << "非终结符:" << this->m_nonTerminators;
  // 打印终结符集
  qDebug() << "终结符:" << this->m_terminators;
  // 打印first集合
  qDebug() << "first集" << this->m_firstSet;
  // 打印follow 集合
  qDebug() << "follow集" << this->m_followSet;
  // 打印预测分析表
  qDebug() << "预测分析表" << this->m_predictTable;
  // 打印产生式
  qDebug() << "产生式:";
  for (QChar productionLeft : this->m_productions.keys()) {
    QString productionRights;
    for (QString productionRight : this->m_productions[productionLeft]) {
      // 空产生式原样输出
      if ("" == productionRight) productionRights += "|@";
      else productionRights += "|" + productionRight;
    }
    qDebug() << productionLeft << "->" << productionRights.sliced(1);
  }
}

bool Grammar::isNonTerminator(const QChar &ch) { return ch.isUpper(); }

bool Grammar::isTerminator(const QChar &ch) { return ch.isLower(); }

bool Grammar::isProductionRightTerminable(const QSet<QChar> &nonTerminators,
                                                 const QString &productionRight) {
  for (const auto ch : productionRight) {
    // 终结符肯定会出现在终结符集合中，这里只需要关心非终结符即可
    if (isNonTerminator(ch) and not nonTerminators.contains(ch)) return false;
  }
  return true;
}

bool Grammar::isProductionRightReachable(const QSet<QChar> &nonTerminators,
                                                const QSet<QChar> &terminators,
                                                const QString &productionRight) {
  for (const auto &ch : productionRight) {
    // 确保终结符在终结符集合中，非终结符集在非终结符集合中
    if ((isNonTerminator(ch) and not nonTerminators.contains(ch)) or
        (isTerminator(ch) and not terminators.contains(ch))) return false;
  }
  return true;
}

void Grammar::simplifyGrammar() {
  // 消除形如U->U有害规则
  for (const auto &productionLeft : this->m_productions.keys()) {
    this->m_productions[productionLeft].remove(productionLeft);
  }

  // 使用算法2.1消除不可终结的无用符号和无用产生式
  // 第一步
  QSet<QChar> nonTerminators { };
  QMap<QChar, QSet<QString>> productions;

  // 第二、三步 一直做到nonTerminator不再增大为止
  qsizetype oldNonTerminatorsSize { nonTerminators.size() };
  qsizetype newNonTerminatorsSize { std::numeric_limits<qsizetype>::max() };

  while (oldNonTerminatorsSize not_eq newNonTerminatorsSize) {
    oldNonTerminatorsSize = newNonTerminatorsSize;

    for (const auto &productionLeft : this->m_productions.keys()) {
      // 先看一下这个产生式左部是不是已经在nonTerminator中了
      if (nonTerminators.contains(productionLeft)) continue;

      // 不然就检查每一个产生式
      for (const auto &productionRight : this->m_productions[productionLeft]) {
        // 如果这个产生式右部是可终结的，就将它产生式的左部加入到nonTerminators中
        if (isProductionRightTerminable(nonTerminators, productionRight)) {
          nonTerminators.insert(productionLeft);
          // 检查到一个产生式符合就可以退出了
          break;
        }
      }
    }

    newNonTerminatorsSize = nonTerminators.size();
  }

  // 第四步
  for (const auto &productionLeft : nonTerminators) {
    // 检查每一个产生式
    for (const auto &productionRight : this->m_productions[productionLeft]) {
      // 如果这个产生式右部是可终结的，就将这条产生式加入产生式映射中
      if (isProductionRightTerminable(nonTerminators, productionRight)) {
        productions[productionLeft].insert(productionRight);
      }
    }
  }

  // 更新非终结符集合
  this->m_nonTerminators = nonTerminators;
  // 更新产生式
  this->m_productions = productions;


  // 使用算法2.2消除不可到达的无用符号和无用产生式
  // 第一步
  nonTerminators.clear();
  QSet<QChar> terminators;
  productions.clear();

  nonTerminators.insert(this->m_start);

  // 第二、三步 一直做到nonTerminators和terminators不再增大为止
  oldNonTerminatorsSize = nonTerminators.size();
  newNonTerminatorsSize = std::numeric_limits<qsizetype>::max();
  qsizetype oldTerminatorsSize { terminators.size() };
  qsizetype newTerminatorsSize { std::numeric_limits<qsizetype>::max() };

  while (oldNonTerminatorsSize not_eq newNonTerminatorsSize or
         oldTerminatorsSize not_eq newTerminatorsSize) {
    oldNonTerminatorsSize = newNonTerminatorsSize;
    oldTerminatorsSize = newTerminatorsSize;

    for (const auto &productionLeft : this->m_productions.keys()) {
      // 检查是否在nonTerminators中
      if (not nonTerminators.contains(productionLeft)) continue;

      // 如果在就遍历每一个产生式
      for (const auto &productionRight : this->m_productions[productionLeft]) {
        for (const auto &ch : productionRight) {
          // 将非终结符加入到nonTerminators中，终结符加入到terminators中
          if (isNonTerminator(ch)) nonTerminators.insert(ch);
          else terminators.insert(ch);
        }
      }
    }

    newNonTerminatorsSize = nonTerminators.size();
    newTerminatorsSize = terminators.size();
  }

  // 第四步
  // 遍历每一条产生式
  for (const auto &productionLeft : nonTerminators) {
    // 遍历每一个产生式
    for (const auto &productionRight : this->m_productions[productionLeft]) {
      // 查看这个产生式的右部是不是可达的
      if (isProductionRightReachable(nonTerminators, terminators, productionRight)) {
        // 如果可达就放入productions中
        productions[productionLeft].insert(productionRight);
      }
    }
  }

  // 更新非终结符集合
  this->m_nonTerminators = nonTerminators;
  // 更新终结符集合
  this->m_terminators = terminators;
  // 更新产生式
  this->m_productions = productions;
}

QChar Grammar::getNewNonTerminator() {
  // 初始化可用非终结符集合
  QSet<QChar> availableNonTerminators;
  for (char ch { 'A' }; ch <= 'Z'; ++ch) availableNonTerminators.insert(ch);
  availableNonTerminators -= this->m_nonTerminators;
  if (availableNonTerminators.empty()) {
    throw QString { "文法化简时非终结符不够用，无法化简该文法，请手动修改文法" };
  }

  // 返回一个没有使用过的非终结符
  return *this->m_nonTerminators.insert(*availableNonTerminators.begin());
}

QSet<QString> Grammar::replace(const QChar &replacer, const QString &productionRight) {
  // 替换后的结果集合
  QSet<QString> result;

  // 对于替换者每一条的产生式右部
  for (const auto &replacerProductionRight : this->m_productions[replacer]) {
    // 使用替换者的产生式右部替换被替换者的产生式右部的第一个字符
    QString replaced { productionRight };
    replaced.replace(0, 1, replacerProductionRight);
    // 将替换后的结果保存到结果集合中
    result.insert(replaced);
  }

  return result;
}

void Grammar::replace(const QChar &replacer, const QChar &replacee) {
  // 获得被替换者的产生式右部集合
  QSet<QString> replaceeProductionRights { this->m_productions[replacee] };

  // 在产生式映射中移除被替换者
  this->m_productions.remove(replacee);

  // 对于被替换者每一条的产生式右部
  for (const auto &replaceeProductionRight : replaceeProductionRights) {
    // 如果是空串或者第一个符号不是替换者，就直接插入产生式映射中
    if ("" == replaceeProductionRight or replacer not_eq replaceeProductionRight[0]) {
      this->m_productions[replacee].insert(replaceeProductionRight);
      continue;
    }

    // 否则，使用替换者替换每一条的产生式右部第一个字符
    QSet<QString> replaceResult { replace(replacer, replaceeProductionRight) };

    // 将替换后的结果保存到产生式映射中
    for (const auto &replaced : replaceResult) this->m_productions[replacee].insert(replaced);
  }
}

QSet<QString> Grammar::replaceFirst(const QString &productionRight) {
  // 当这个产生式是空产生式或者第一个符号是终结符时就直接返回，无需处理
  if ("" == productionRight or isTerminator(productionRight[0])) {
    return { productionRight };
  }

  // 返回的结果
  QSet<QString> result { };

  // 中间暂存栈
  QStack<QString> stack;
  stack.emplaceBack(productionRight);

  // 设置代入最大次数限制
  uint8_t count { 100 };

  while (not stack.isEmpty()) {
    // 达到最大次数限制直接报错
    if (0 == --count) throw QString { "该文法不好，无法提取左公因子，请手动修改文法" };
    // 对栈中存储的数据作代入处理
    QString currentProduction { stack.pop() };
    QSet<QString> newResult { replace(currentProduction[0], currentProduction) };

    // 检查代入后的结果
    for (const auto &newProductionRight : newResult) {
      // 当这个产生式是空产生式或者第一个符号是终结符时就放入结果集
      if ("" == newProductionRight or isTerminator(newProductionRight[0])) {
        result.insert(newProductionRight);
      }
      // 否则就放入栈中继续操作
      else stack.emplaceBack(newProductionRight);
    }
  }

  return result;
}

QSet<QChar> Grammar::extractPrefixes(const QSet<QString> &productionRights) {
  QSet<QChar> prefixes;

  for (const auto &productionRight : productionRights) {
    if ("" == productionRight) continue;
    prefixes.insert(productionRight[0]);
  }

  return prefixes;
}

QSet<QString> Grammar::findSuffixes(const QChar &prefix, const QSet<QString> &productionRights) {
  QSet<QString> suffixes;

  for (const auto &productionRight : productionRights) {
    if ("" not_eq productionRight and prefix == productionRight[0]) {
      suffixes.insert(productionRight.sliced(1));
    }
  }

  return suffixes;
}

QSet<QString> Grammar::getFirstSet(const QChar &nonTerminator) {
  // 将要返回的first集合
  QSet<QString> first;

  // 对于每一条产生式的右部，调用求first集合的方法求解
  for (const auto &productionRight : this->m_productions[nonTerminator]) {
    first |= getFirstSet(productionRight);
  }

  return first;
}

QSet<QString> Grammar::getFirstSet(const QString &productionRight) {
  // 如果为空就返回空
  if ("" == productionRight) return { "" };

  // 将要返回的first集合
  QSet<QString> first;

  // 逐个遍历里面的符号
  for (const auto &ch : productionRight) {
    // 如果遇到了终结符就直接返回
    if (isTerminator(ch)) return first |= ch;

    // 遇到了非终结符，求这个非终结符的first集合元素
    first |= getFirstSet(ch);

    // 如果求得的first集合中没有空，直接返回
    if (not first.contains("")) return first;

    // 所得的first集合中有空，去除这个空，继续往下遍历
    first.remove("");
  }

  // 如果到达这里，说明整个表达式的first集合中有空
  return first |= "";
}

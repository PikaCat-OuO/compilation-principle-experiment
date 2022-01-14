#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
      , ui(new Ui::Dialog)
{
  ui->setupUi(this);

  // 加上最小化和最大化按钮
  this->setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint |
                       Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

  // 设置不显示表序号
  ui->tableWidget->verticalHeader()->setVisible(false);

  // 调整表格框度
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

Dialog::~Dialog()
{
  delete ui;
}

// 将一系列状态打包为一个QString
QString Dialog::packStates(const QSet<State> &states) const {
  QString result;
  for (const auto &state : states) {
    result += QString::number(state);
    result += ", ";
  }

  // 移除最后的,
  result.remove(result.size() - 2, 2);

  // 返回结果
  return result;
}

// 检查表达式
bool Dialog::checkEmptyRegex() {
  if (ui->lineEdit->text().isEmpty()) {
    QMessageBox::information(this, "提示", "请输入正则表达式");
    return false;
  } else return true;
}

// 设置显示的表头，并返回转换和表格列下标的映射关系
template<typename T>
QMap<QChar, quint64> Dialog::setTableHeader(const T &transferTable) {
  // 获得所有状态
  QSet<QChar> allTransfers;
  for (const auto &transfers : transferTable.values()) {
    for (const auto &transfer : transfers.keys()) allTransfers.insert(transfer);
  }

  // 记录转换状态和表格列号的对应关系
  QMap<QChar, quint64> columnIndex;
  QStringList transferList { "节点编号" };
  quint64 transferCount { 1 };
  for (const auto &transfer : allTransfers) {
    columnIndex[transfer] = transferCount++;
    transferList.emplaceBack(transfer);
  }

  // 清除表格
  ui->tableWidget->clear();
  ui->tableWidget->setRowCount(0);

  // 设置表头
  ui->tableWidget->setColumnCount(transferList.size());
  ui->tableWidget->setHorizontalHeaderLabels(transferList);

  return columnIndex;
}

// 设置显示的表格
template<typename T>
void Dialog::setTableContent(const T &transferTable) {
  // 获得转换和表格列下标的映射关系
  QMap<QChar, quint64> columnIndex { this->setTableHeader(transferTable) };

  // 显示到屏幕上
  for (const auto &[state, transfers] : transferTable.toStdMap()) {
    // 新增一行
    quint64 currentRowCount = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(currentRowCount + 1);

    ui->tableWidget->setItem(currentRowCount, 0,
                             new QTableWidgetItem { QString::number(state) });

    // 输出节点信息
    for (const auto &[transfer, states] : transfers.toStdMap()){
      // 即将要显示到屏幕上的状态串
      QTableWidgetItem *nextStates { nullptr };

      // 查看是不是有多对应的transferTable，如果是的话要构造状态串
      if constexpr (std::is_same_v<T, QMap<State, QMap<QChar, QSet<State>>>>) {
        nextStates = new QTableWidgetItem { this->packStates(states) };
      } else nextStates = new QTableWidgetItem { QString::number(states) };

      // 显示到屏幕上
      ui->tableWidget->setItem(currentRowCount, columnIndex[transfer], nextStates);
    }
  }

  // 调整行大小
  ui->tableWidget->resizeRowsToContents();
}

void Dialog::on_generateNFA_clicked()
{
  // 检查正则表达式为空的情况
  if (not this->checkEmptyRegex()) return;

  // 获取正则表达式
  QString regularExpression { ui->lineEdit->text() };

  // 生成NFA图
  NFAMachine NFAMachine { RegularExpressionEngine::RegularExpressionToNFA(regularExpression) };

  // 设置开始和结束状态
  ui->startState->setText(QString::number(NFAMachine.getBeginState()));
  ui->endState->setText(QString::number(NFAMachine.getEndState()));

  // 显示到屏幕上
  this->setTableContent(NFAMachine.ms_stateTransferTable);
}

void Dialog::on_generateDFA_clicked()
{
  // 检查正则表达式为空的情况
  if (not this->checkEmptyRegex()) return;

  // 获取正则表达式
  QString regularExpression { ui->lineEdit->text() };

  // 转换为DFA
  RegularExpressionEngine::RegularExpressionToDFA(regularExpression);

  // 设置开始和结束状态
  ui->startState->setText(QString::number(DFAMachine::BeginState));
  ui->endState->setText(this->packStates(DFAMachine::EndStates));

  // 显示图到画面上
  this->setTableContent(DFAMachine::StateTransferTable);
}

void Dialog::on_generateMFA_clicked()
{
  // 检查正则表达式为空的情况
  if (not this->checkEmptyRegex()) return;

  // 获取正则表达式
  QString regularExpression { ui->lineEdit->text() };

  // 转化为MFA
  RegularExpressionEngine::RegularExpressionToMFA(regularExpression);

  // 设置开始和结束状态
  ui->startState->setText(QString::number(MFAMachine::BeginState));
  ui->endState->setText(this->packStates(MFAMachine::EndStates));

  // 显示图到画面上
  this->setTableContent(MFAMachine::StateTransferTable);
}

void Dialog::on_generateCode_clicked()
{
  // 检查正则表达式为空的情况
  if (not this->checkEmptyRegex()) return;

  // 获取正则表达式
  QString regularExpression { ui->lineEdit->text() };

  // 转化为代码，并显示代码窗口
  codeDialog->setCode(RegularExpressionEngine::RegularExpressionToCode(regularExpression));
  codeDialog->show();
}


void Dialog::on_open_clicked()
{
  // 获取文件名
  QString fileName { QFileDialog::getOpenFileName(this, "打开正则表达式",
                                                ".", "正则表达式(*.regex)") };

  // 判断文件名为空的情况
  if (fileName == "") return;

  // 构造文件对象
  QFile file { fileName };

  // 读取全部内容
  if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ui->lineEdit->setText(file.readAll());
    QMessageBox::information(this, "提示", "读取成功");
  } else QMessageBox::information(this, "提示", "读取失败");
}


void Dialog::on_save_clicked()
{
  // 获取文件名
  QString fileName { QFileDialog::getSaveFileName(this, "保存正则表达式",
                                                ".", "正则表达式(*.regex)") };

  // 判断文件名为空的情况
  if (fileName == "") return;

  // 构造文件对象
  QFile file { fileName };

  // 写出全部内容
  if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    file.write(ui->lineEdit->text().toStdString().c_str());
    QMessageBox::information(this, "提示", "保存成功");
  } else QMessageBox::information(this, "提示", "保存失败");
}

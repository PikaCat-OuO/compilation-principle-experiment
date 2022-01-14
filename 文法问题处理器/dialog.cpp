#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
      , ui(new Ui::Dialog)
{
  ui->setupUi(this);
  this->setWindowTitle("文法问题处理器");
}

Dialog::~Dialog()
{
  delete ui;
}

QString Dialog::toGrammarString()
{
  // 返回的结果
  QString result;

  // 打印文法头
  result += QString { "G[" } + this->m_grammar->getStart() + "]:\n";

  // 打印产生式
  auto productions { this->m_grammar->getProduction() };
  for (const auto &productionLeft : productions.keys()) {
    QString productionRights;
    for (const auto &productionRight : productions[productionLeft]) {
      // 空产生式原样输出
      if ("" == productionRight) productionRights += "|@";
      else productionRights += "|" + productionRight;
    }
    result += QString { productionLeft } + "->" + productionRights.sliced(1) + '\n';
  }

  return result;
}

bool Dialog::process(quint8 requestedStage)
{
  quint8 stage { SIMPLIFY };

  if (this->m_grammar not_eq nullptr) {
    delete(this->m_grammar);
    this->m_grammar = nullptr;
  }

  try {
    this->m_grammar = new Grammar { ui->grammarInput->toPlainText() };
    while (stage++ < requestedStage) {
      switch (stage) {
      case ELIMINATE_LEFT_RECURSION: this->m_grammar->eliminateLeftRecursion(); break;
      case LEFT_FACTORING: this->m_grammar->leftFactoring(); break;
      case ALL: this->m_grammar->calcFirstAndFollowSet();
        this->m_grammar->constructPredictTable(); break;
      default: break;
      }
    }
  } catch (const QString &msg) {
    QMessageBox::critical(this, "提示", msg);
    return false;
  }

  return true;
}


void Dialog::on_open_clicked()
{
  // 获取文件名
  QString fileName { QFileDialog::getOpenFileName(this, "打开文法规则", ".", "文法规则(*.grammar)") };

  // 判断文件名为空的情况
  if (fileName == "") return;

  // 构造文件对象
  QFile file { fileName };

  // 读取全部内容
  if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ui->grammarInput->setText(file.readAll());
    QMessageBox::information(this, "提示", "读取成功");
  } else QMessageBox::warning(this, "提示", "读取失败");
}

void Dialog::on_save_clicked()
{
  // 获取文件名
  QString fileName { QFileDialog::getSaveFileName(this, "保存文法规则", ".", "文法规则(*.grammar)") };

  // 判断文件名为空的情况
  if (fileName == "") return;

  // 构造文件对象
  QFile file { fileName };

  // 写出全部内容
  if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    file.write(ui->grammarInput->toPlainText().toStdString().c_str());
    QMessageBox::information(this, "提示", "保存成功");
  } else QMessageBox::warning(this, "提示", "保存失败");
}

void Dialog::on_simplifyGrammar_clicked()
{
  if (process(SIMPLIFY)) {
    GrammarDialog *grammarDialog { new GrammarDialog
                                 { this, "化简后的文法", toGrammarString() } };
    grammarDialog->show();
  }
}

void Dialog::on_eliminateLeftRecursion_clicked()
{
  if (process(ELIMINATE_LEFT_RECURSION)) {
    GrammarDialog *grammarDialog { new GrammarDialog
      { this, "消除左递归后的文法", toGrammarString() } };
    grammarDialog->show();
  }
}

void Dialog::on_leftFactoring_clicked()
{
  if (process(LEFT_FACTORING)) {
    GrammarDialog *grammarDialog { new GrammarDialog
                                 { this, "提取左公因子后的文法", toGrammarString() } };
    grammarDialog->show();
  }
}

void Dialog::on_firstSet_clicked()
{
  if (process(ALL)) {
    SetDialog *setDialog { new SetDialog { this->m_grammar->getFirstSet(), this, "first集合" } };
    setDialog->show();
  }
}

void Dialog::on_followSet_clicked()
{
  if (process(ALL)) {
    SetDialog *setDialog { new SetDialog
                         { this->m_grammar->getFollowSet(), this, "follow集合" } };
    setDialog->show();
  }
}

void Dialog::on_leftmostDerive_clicked()
{
  if (process(ALL)) {
    ui->listWidget->clear();

    const auto &[result, procedure] { this->m_grammar->leftmostDerive(
        ui->sentenceInput->text()) };
    if (result) {
      QMessageBox::information(this, "推导结果", "推导成功");
      for (const auto &step : procedure) ui->listWidget->addItem(step);
    } else QMessageBox::information(this, "推导结果", "推导失败");
  }
}

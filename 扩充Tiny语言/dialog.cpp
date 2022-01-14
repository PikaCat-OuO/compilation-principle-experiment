#include "dialog.h"
#include "ui_dialog.h"
#include "analyze.h"
#include <QFileDialog>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
      , ui(new Ui::Dialog)
{
  ui->setupUi(this);
}

Dialog::~Dialog()
{
  delete ui;
}

void Dialog::on_analyzeButton_clicked()
{
  // 初始化
  lineno = 0;
  linepos = 0;
  bufsize = 0;
  EOF_flag = false;

  // 构造文件对象
  QFile sourceCode { "SourceCode" };

  // 写入代码
  if (sourceCode.open(QIODevice::WriteOnly | QIODevice::Text)) {
    sourceCode.write(ui->sourceCodeBox->toPlainText().toStdString().c_str());
    sourceCode.close();
  }

  // 分析代码
  AnalyzeCode();

  // 构造文件对象
  QFile result { "Result" };

  // 读取结果
  if (result.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ui->resultBox->setText(result.readAll());
    result.close();
  }

  // 删除临时文件
  QFile::remove("SourceCode");
  QFile::remove("Result");
}


void Dialog::on_openButton_clicked()
{
  // 获取文件名
  QString fileName{ QFileDialog::getOpenFileName(this, "打开源文件", ".", "源文件(*.tny)") };
  // 构造文件对象
  QFile file { fileName };

  if (fileName.isEmpty()) return;

  // 读取全部内容
  if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ui->sourceCodeBox->setText(file.readAll());
    QMessageBox::information(this, "提示", "读取成功");
  }

  // 文件打开失败
  else QMessageBox::information(this, "提示", "读取失败");
}


void Dialog::on_closeButton_clicked()
{
  // 获取文件名
  QString fileName{ QFileDialog::getSaveFileName(this, "保存源文件", ".", "源文件(*.tny)") };

  // 构造文件对象
  QFile file { fileName };

  if (fileName.isEmpty()) return;

  // 写入全部内容
  if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    file.write(ui->sourceCodeBox->toPlainText().toStdString().c_str());
    // 展开提示
    QMessageBox::information(this,"提示", "保存成功");
  }

  // 如果保存失败
  else QMessageBox::information(this, "提示", "保存失败");
}


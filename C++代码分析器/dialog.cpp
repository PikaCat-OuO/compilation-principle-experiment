#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
      , ui(new Ui::Dialog)
{
  ui->setupUi(this);
  // 调整表格框度
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

Dialog::~Dialog()
{
  delete ui;
}


void Dialog::on_pushButton_3_clicked()
{

  // 获取文件名
  QString fileName{QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开源文件"), ".",
                                                QString::fromLocal8Bit("源文件(*.cpp)"))};
  // 构造文件对象
  QFile file{fileName};

  // 设置文件标题
  if (not fileName.isEmpty()) ui->label_2->setText(fileName);

  // 读取全部内容
  if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    this->mCode = file.readAll();
  }

}


void Dialog::on_pushButton_2_clicked()
{

  // 分析代码
  auto results {this->mCodeProcessor.analyzeCode(this->mCode)};

  // 先清除原有的项目
  ui->tableWidget->clearContents();

  // 将结果显示出来
  for (const auto &[vocabulary, type] : results) {
    // 新增一行
    int currentRowCount = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(currentRowCount + 1);
    ui->tableWidget->setItem(currentRowCount, 0,
                             new QTableWidgetItem{QString::fromLocal8Bit(vocabulary)});
    ui->tableWidget->setItem(currentRowCount, 1,
                             new QTableWidgetItem{QString::fromLocal8Bit(type)});
  }
  // 调整单元格大小
  ui->tableWidget->resizeRowsToContents();
}


void Dialog::on_pushButton_clicked()
{

  // 执行代码替换
  this->mCode = this->mCodeProcessor.replaceCode(this->mCode);

  // 将结果写回到文件，先构造文件对象
  QFile file{ui->label_2->text()};

  // 写入全部内容
  if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    file.write(this->mCode.c_str());
    // 展开提示
    QMessageBox::information(this, QString::fromLocal8Bit("提示"),
                             QString::fromLocal8Bit("转换成功，结果已经写回源文件"));
  }

  // 如果写入失败
  else QMessageBox::information(this, QString::fromLocal8Bit("提示"),
                             QString::fromLocal8Bit("转换失败"));

}


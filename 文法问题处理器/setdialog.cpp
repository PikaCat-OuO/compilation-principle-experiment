#include "setdialog.h"
#include "ui_setdialog.h"

SetDialog::SetDialog(QMap<QChar, QSet<QString>> set, QWidget *parent, QString title) :
                                                                                       QDialog(parent),
                                                                                       ui(new Ui::SetDialog)
{
  ui->setupUi(this);
  this->setWindowTitle(title);
  // 设置不显示表序号
  ui->tableWidget->verticalHeader()->setVisible(false);

  // 调整表格框度
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  // 将结果显示出来
  for (const auto &productionLeft : set.keys()) {
    // 新增一行
    int currentRowCount = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(currentRowCount + 1);
    ui->tableWidget->setItem(currentRowCount, 0, new QTableWidgetItem { productionLeft });

    QString productionRight { '{' };
    for (const auto &ch : set[productionLeft]) {
      if (ch == "") productionRight += "@,";
      else productionRight += ch + ',';
    }
    productionRight.remove(productionRight.length() - 1, 1);
    productionRight += '}';

    ui->tableWidget->setItem(currentRowCount, 1, new QTableWidgetItem { productionRight } );
  }
  // 调整单元格大小
  ui->tableWidget->resizeRowsToContents();
}

SetDialog::SetDialog(QMap<QChar, QSet<QChar>> set, QWidget *parent, QString title) :
                                                                                     QDialog(parent),
                                                                                     ui(new Ui::SetDialog)
{
  ui->setupUi(this);
  this->setWindowTitle(title);
  // 设置不显示表序号
  ui->tableWidget->verticalHeader()->setVisible(false);

  // 调整表格框度
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  // 将结果显示出来
  for (const auto &productionLeft : set.keys()) {
    // 新增一行
    int currentRowCount = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(currentRowCount + 1);
    ui->tableWidget->setItem(currentRowCount, 0, new QTableWidgetItem { productionLeft });

    QString productionRight { '{' };
    for (const auto &ch : set[productionLeft]) productionRight += ch + ',';
    productionRight.remove(productionRight.length() - 1, 1);
    productionRight += '}';

    ui->tableWidget->setItem(currentRowCount, 1, new QTableWidgetItem { productionRight } );
  }
  // 调整单元格大小
  ui->tableWidget->resizeRowsToContents();
}

SetDialog::~SetDialog()
{
  delete ui;
}

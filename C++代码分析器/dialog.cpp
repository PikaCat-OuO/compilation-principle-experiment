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
  // ���������
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

Dialog::~Dialog()
{
  delete ui;
}


void Dialog::on_pushButton_3_clicked()
{

  // ��ȡ�ļ���
  QString fileName{QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("��Դ�ļ�"), ".",
                                                QString::fromLocal8Bit("Դ�ļ�(*.cpp)"))};
  // �����ļ�����
  QFile file{fileName};

  // �����ļ�����
  if (not fileName.isEmpty()) ui->label_2->setText(fileName);

  // ��ȡȫ������
  if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    this->mCode = file.readAll();
  }

}


void Dialog::on_pushButton_2_clicked()
{

  // ��������
  auto results {this->mCodeProcessor.analyzeCode(this->mCode)};

  // �����ԭ�е���Ŀ
  ui->tableWidget->clearContents();

  // �������ʾ����
  for (const auto &[vocabulary, type] : results) {
    // ����һ��
    int currentRowCount = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(currentRowCount + 1);
    ui->tableWidget->setItem(currentRowCount, 0,
                             new QTableWidgetItem{QString::fromLocal8Bit(vocabulary)});
    ui->tableWidget->setItem(currentRowCount, 1,
                             new QTableWidgetItem{QString::fromLocal8Bit(type)});
  }
  // ������Ԫ���С
  ui->tableWidget->resizeRowsToContents();
}


void Dialog::on_pushButton_clicked()
{

  // ִ�д����滻
  this->mCode = this->mCodeProcessor.replaceCode(this->mCode);

  // �����д�ص��ļ����ȹ����ļ�����
  QFile file{ui->label_2->text()};

  // д��ȫ������
  if(file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    file.write(this->mCode.c_str());
    // չ����ʾ
    QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"),
                             QString::fromLocal8Bit("ת���ɹ�������Ѿ�д��Դ�ļ�"));
  }

  // ���д��ʧ��
  else QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"),
                             QString::fromLocal8Bit("ת��ʧ��"));

}


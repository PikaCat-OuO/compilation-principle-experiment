#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "codeprocessor.h"

QT_BEGIN_NAMESPACE
    namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
  Q_OBJECT
public:
  Dialog(QWidget *parent = nullptr);
  ~Dialog();

private slots:
  void on_pushButton_3_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_clicked();

private:

  Ui::Dialog *ui;

  // 代码处理器实例
  CodeProcessor mCodeProcessor;

  // 暂时存储代码的地方
  std::string mCode;

};
#endif // DIALOG_H

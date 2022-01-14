#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
    namespace Ui { class Dialog; }
QT_END_NAMESPACE

    class Dialog : public QDialog
{
  Q_OBJECT

      public:
               Dialog(QWidget *parent = nullptr);
  ~Dialog();
               void test();

private slots:
  void on_analyzeButton_clicked();

  void on_openButton_clicked();

  void on_closeButton_clicked();

private:
  Ui::Dialog *ui;
};

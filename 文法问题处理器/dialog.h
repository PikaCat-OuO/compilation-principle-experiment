#pragma once

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include "grammar.h"
#include "grammardialog.h"
#include "setdialog.h"

constexpr quint8 SIMPLIFY { 0 };
constexpr quint8 ELIMINATE_LEFT_RECURSION { 1 };
constexpr quint8 LEFT_FACTORING { 2 };
constexpr quint8 ALL { 3 };

QT_BEGIN_NAMESPACE
    namespace Ui { class Dialog; }
QT_END_NAMESPACE

    class Dialog : public QDialog
{
  Q_OBJECT

      public:
               Dialog(QWidget *parent = nullptr);
  ~Dialog();

protected:
  // 将一个文法转化成能看的形式输出
  QString toGrammarString();
  // 文法自动机
  bool process(quint8 requestedStage);

private slots:
  void on_open_clicked();

  void on_save_clicked();

  void on_simplifyGrammar_clicked();

  void on_eliminateLeftRecursion_clicked();

  void on_leftFactoring_clicked();

  void on_firstSet_clicked();

  void on_followSet_clicked();

  void on_leftmostDerive_clicked();

private:
  Grammar *m_grammar { nullptr };
  Ui::Dialog *ui;
};


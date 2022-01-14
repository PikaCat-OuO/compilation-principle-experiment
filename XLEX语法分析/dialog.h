#pragma once
#include <QDialog>
#include "regularexpressionengine.h"
#include "codedialog.h"

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
  // 将一系列状态打包为一个QString，方便输出
  QString packStates(const QSet<State> &states) const;

  // 表达式是否为空
  bool checkEmptyRegex();

  // 设置显示的表头
  template<typename T>
  QMap<QChar, quint64> setTableHeader(const T &transferTable);

  // 设置显示的表格
  template<typename T>
  void setTableContent(const T &transferTable);

private slots:
  void on_generateNFA_clicked();

  void on_generateDFA_clicked();

  void on_generateMFA_clicked();

  void on_generateCode_clicked();

  void on_open_clicked();

  void on_save_clicked();

private:
  Ui::Dialog *ui;

  // 指向代码窗口的指针
  CodeDialog *codeDialog { new CodeDialog { this } };
};

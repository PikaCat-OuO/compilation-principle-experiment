#ifndef CODEDIALOG_H
#define CODEDIALOG_H

#include <QDialog>

namespace Ui {
class CodeDialog;
}

class CodeDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CodeDialog(QWidget *parent = nullptr);
  ~CodeDialog();

  // 设置显示的代码
  void setCode(const QString &code);

private:
  Ui::CodeDialog *ui;
};

#endif // CODEDIALOG_H

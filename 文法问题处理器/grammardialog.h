#ifndef GRAMMARDIALOG_H
#define GRAMMARDIALOG_H

#include <QDialog>

namespace Ui {
class GrammarDialog;
}

class GrammarDialog : public QDialog
{
  Q_OBJECT

      public:
               explicit GrammarDialog(QWidget *parent = nullptr, QString title = "", QString text = "");
  ~GrammarDialog();

private:
  Ui::GrammarDialog *ui;
};

#endif // GRAMMARDIALOG_H

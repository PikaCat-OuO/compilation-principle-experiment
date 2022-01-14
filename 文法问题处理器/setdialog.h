#ifndef SETDIALOG_H
#define SETDIALOG_H

#include <QDialog>

namespace Ui {
class SetDialog;
}

class SetDialog : public QDialog
{
  Q_OBJECT

      public:
               explicit SetDialog(QMap<QChar, QSet<QString>> set, QWidget *parent = nullptr, QString title = "");
  explicit SetDialog(QMap<QChar, QSet<QChar>> set, QWidget *parent = nullptr, QString title = "");
  ~SetDialog();

private:
  Ui::SetDialog *ui;
};

#endif // SETDIALOG_H

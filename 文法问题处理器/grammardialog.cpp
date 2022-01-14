#include "grammardialog.h"
#include "ui_grammardialog.h"

GrammarDialog::GrammarDialog(QWidget *parent, QString title, QString text) :
                                                QDialog(parent),
                                                ui(new Ui::GrammarDialog)
{
  ui->setupUi(this);
  this->setWindowTitle(title);
  ui->textEdit->setText(text);
}

GrammarDialog::~GrammarDialog()
{
  delete ui;
}

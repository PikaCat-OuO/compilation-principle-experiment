#include "codedialog.h"
#include "ui_codedialog.h"

CodeDialog::CodeDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::CodeDialog) {
  ui->setupUi(this);

  // 加上最小化和最大化按钮
  this->setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint |
                       Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}

CodeDialog::~CodeDialog()
{
  delete ui;
}

// 设置显示的代码
void CodeDialog::setCode(const QString &code) { ui->textEdit->setText(code); }

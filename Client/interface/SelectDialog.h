#ifndef SELECTDIALOG_H
#define SELECTDIALOG_H

#include <QDialog>

namespace Ui {
class SelectDialog;
}

class SelectDialog : public QDialog
{
  Q_OBJECT

public:
  SelectDialog(QWidget *parent, QStringList choices);
  ~SelectDialog();
  int getSelection ();

public slots:
  void pressed ();

private:
  Ui::SelectDialog *ui;
  int _sel;
};

#endif // SELECTDIALOG_H

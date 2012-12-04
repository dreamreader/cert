#include "SelectDialog.h"
#include "ui_SelectDialog.h"

//------------------------------------------------------------

SelectDialog::SelectDialog(QWidget *parent, QStringList choices) :
    QDialog(parent),
    ui(new Ui::SelectDialog)
{
    ui->setupUi(this);
    _sel = 0;

    for (int i = 0; i != choices.size (); i++) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(choices[i]);
        ui->listWidget->addItem(item);
    }
    ui->listWidget->setFocus();
}

//------------------------------------------------------------

SelectDialog::~SelectDialog()
{
    delete ui;
}

//------------------------------------------------------------

int SelectDialog::getSelection ()
{
    return _sel;
}

//------------------------------------------------------------

void SelectDialog::pressed ()
{
    _sel = ui->listWidget->currentRow();
    close ();
}

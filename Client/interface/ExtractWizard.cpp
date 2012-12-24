#include "ExtractWizard.h"
#include "ui_ExtractWizard.h"

//------------------------------------------------------------

ExtractWizard::ExtractWizard(Nbc &container) :
  QWizard(0),
  ui(new Ui::ExtractWizard)
{
  ui->setupUi(this);

  setButtonText (QWizard::BackButton, QString::fromUtf8("< Назад"));
  setButtonText (QWizard::NextButton, QString::fromUtf8("Далее >"));
  setButtonText (QWizard::FinishButton, QString::fromUtf8("Готово"));
  setButtonText (QWizard::CancelButton, QString::fromUtf8("Отмена"));
  setButtonText (QWizard::HelpButton, QString::fromUtf8("Справка"));

  _nbc = container;
  _result = false;
  _input_scheme.extractFrom(_nbc);
  if (_input_scheme.size() <= 0) {
    throw QString::fromUtf8("Неверный формат контейнера");
  }
 // _username = username;
  _modules.loadAllModules ((QString)"." + QDir::separator () + (QString)"modules" + QDir::separator ());

  QRect frect = frameGeometry();
  frect.moveCenter(QDesktopWidget().availableGeometry().center());
  move(frect.topLeft());
}

//------------------------------------------------------------

ExtractWizard::~ExtractWizard()
{
  delete ui;

  _clear ();
}

//------------------------------------------------------------

void ExtractWizard::paintEvent (QPaintEvent * event)
{
  QDialog::paintEvent(event);
  SchemeVisualiser::redraw (ui->_tblScheme);
}

//------------------------------------------------------------

void ExtractWizard::changePage () {
  int dif = 0;
  Data code;
  bool granted = false;

  switch (currentId ()) {
  case 1: //Ввод биометрии
    this->resize(QSize (720, 520));
    if (_extracted) {
      _extracted = false;
    }
    button (QWizard::NextButton)->hide();

    if (_input_scheme.branchesCount () > 1) {
      QStringList choices;
      for (int i = 0; i != _input_scheme.branchesCount (); i++) {
        Provider null;
        ModuleInfo info;
        _modules.getProvider(_input_scheme.getCurrentProvider(), null, &info);
        choices << info.name;
        _input_scheme.next ();
      }
      SelectDialog dialog (this, choices);
      dialog.exec ();
      int res = dialog.getSelection ();
      res == -1 ? res = 0 : res;
      _input_scheme.toFirstBranch();
      for (int i = 0; i < res; i++) _input_scheme.next ();
    }


    SchemeVisualiser::clearMarks(ui->_tblScheme);
    SchemeVisualiser::mark(_input_scheme.index(), SCHEMEVISUALISER_STATUS_PROGRESS, ui->_tblScheme);

    if (!_modules.getProvider(_input_scheme.getCurrentProvider(), _provider)) {
      throw QString::fromUtf8("Модуль провайдера не найден!");
    }
    if (_provider.getId() == (Uuid) UUID_FINGER_PROVIDER) {
      this->resize(QSize (640, 720));
    } else {
      this->resize(QSize (720, 520));
    }
    _provider.attachDevice();
    _provider.attachWindow(ui->widget);
    ui->_btnAdd->setDefault(true);
    this->QWidget::setVisible(false);
    this->QWidget::setVisible(true);
    _provider.startCapturing(_images[_input_scheme.index()]);
    return;

  case 2:
    this->hide ();

    //Преобразование образов
    _input_scheme.toStart();
    do {
      if (_images[_input_scheme.index()]->size() < 1) continue;
      if (!_modules.getProcessor(_input_scheme.getCurrentProcessor(), _processor)) {
        throw QString::fromUtf8("Модуль процессора не найден!");
      }

      Matrix pattern;
      if (_nbc.getBimPattern(_input_scheme.getCurrentSlotId(), _images[_input_scheme.index()]->getBimType(0), pattern)) {
        _processor.setPattern(pattern);
      }

      if (_images[_input_scheme.index()]->size() > 0) {
        _processor.process (*_params_own[_input_scheme.index()], *_images[_input_scheme.index()]);
      }
    } while (_input_scheme.next ());

    //Извлечение

    switch (int errc = Client::authenticateBio(_username, code, granted) )
    {
    case nbS_OK:
        break;
    default:
        //qDebug() << "failed to usePassword, errc=" << Result (errc);
        throw QString::fromUtf8 ("Невозможно проверить токен доступа");
    }

    if (granted) {
      accept ();
    } else {
      ui->page3->setTitle(QString::fromUtf8("Аутентификация неудачна!") + QString::fromUtf8("\r\n Нажмите \"Назад\", чтобы попытаться снова"));
    }

    _extracted = true;
    accept ();
    return;
  default:
    return;
  }
  return;
}

//------------------------------------------------------------

void ExtractWizard::addImage ()
{
  try {
    //Считываем образ из провайдера
    if (!_provider.capture()) {
      return;
    }

    if ( _input_scheme.nextStep() ) {
      changePage();
      return;
    } else {
      next ();
    }
  } catch (QString error) {
      Gui::error(error);
  }
}

//------------------------------------------------------------

void ExtractWizard::clearImage ()
{
  try {
    _provider.resetInput();
  } catch (QString error) {
    Gui::error(error);
  }
}

//------------------------------------------------------------

void ExtractWizard::_clear ()
{
  try {
    for (uint i = 0; i != _input_scheme.size (); i++) {
      delete _params_own[i];
      delete _params_foreign[i];
      delete _patterns[i];
      _images[i]->clear();
      delete _images[i];
    }
    _images.clear ();
    _params_own.clear ();
    _params_foreign.clear ();
    _patterns.clear ();
  } catch (QString error) {
    Gui::error(error);
  }
}

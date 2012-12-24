#include "TrainWizard.h"
#include "ui_TrainWizard.h"

//------------------------------------------------------------

TrainWizard::TrainWizard(QString username) :
  QWizard(0),
  ui(new Ui::TrainWizard)
{
  ui->setupUi(this);

  setButtonText (QWizard::BackButton, QString::fromUtf8("< Назад"));
  setButtonText (QWizard::NextButton, QString::fromUtf8("Далее >"));
  setButtonText (QWizard::FinishButton, QString::fromUtf8("Готово"));
  setButtonText (QWizard::CancelButton, QString::fromUtf8("Отмена"));
  setButtonText (QWizard::HelpButton, QString::fromUtf8("Справка"));
  ui->txtPin->setValidator(new QIntValidator(0, 65536, ui->txtPin));
  ui->progressImages->setMaximum(MINIMUM_TRAIN_IMAGES);
  ui->progressQuality->setMaximum(100);
  ui->txtKey->setText("00000000000000000000000000000000");
  if (ui->listSchemes_2->count() > 0) {
    ui->listSchemes_2->setCurrentRow(0);
  }

  try {
    _username = username;
    _modules.loadAllModules((QString)"." + QDir::separator () + (QString)"modules" + QDir::separator ());
    _path = (QString)"." + QDir::separator () + _username + QDir::separator ();
    QDir dir (_path);
    dir.exists() ?: dir.mkpath(".");
  } catch (QString error) {
    Gui::error (error);
  }
  QRect frect = frameGeometry();
  frect.moveCenter(QDesktopWidget().availableGeometry().center());
  move(frect.topLeft());
}

//------------------------------------------------------------

void TrainWizard::paintEvent (QPaintEvent * event)
{
  QWizard::paintEvent(event);
  SchemeVisualiser::redraw (ui->scheme);
  SchemeVisualiser::redraw (ui->scheme_2);
}

//------------------------------------------------------------

TrainWizard::~TrainWizard()
{
  delete ui;
  for (uint i = 0; i != _input_scheme.size (); i++) {
    delete _params_own[i];
    delete _params_foreign[i];
    delete _patterns[i];
    _images[i]->clear();
    delete _images[i];
  }
}

//------------------------------------------------------------

void TrainWizard::test()
{
  TestDialog dlg (_nbc);
  dlg.exec ();
  dlg.getStats(_stats.tries (), _stats.successes ());
}

//------------------------------------------------------------

void TrainWizard::edit()
{

}

//------------------------------------------------------------

void TrainWizard::change () {
  switch (currentId ()) {
  case 0: //Инициализация контейнера
    this->resize(QSize (720, 520));
    //listSchemesChanged ();
    txtNameChanged ();
    return;

  case 1: //Ввод ключа
    this->resize(QSize (720, 520));
    if ( _input_scheme.prev() ) {
      next();
      return;
    }
    button (QWizard::NextButton)->setDisabled(true);
    //listSchemesChanged ();
    changeKey ();
    return;

  case 2: //Ввод биометрии
    this->resize(QSize (720, 520));
    ui->btnAccept->setFocus();
    SchemeVisualiser::clearMarks(ui->scheme_2);
    SchemeVisualiser::mark(_input_scheme.index(), SCHEMEVISUALISER_STATUS_PROGRESS, ui->scheme_2);
    _reloadImageList();

  //Подключение провайдера
  try {
    if (!_modules.getProvider(_input_scheme.getCurrentProvider(), _provider)) {
      throw QString::fromUtf8("Модуль биометрического провайдера не найден!");
    }
    if (_provider.getId() == (Uuid) UUID_FINGER_PROVIDER) {
      this->resize(QSize (640, 720));
    } else {
      this->resize(QSize (720, 520));
    }
    _provider.attachDevice();
    _provider.attachWindow(ui->widget);
    this->QWidget::setVisible(false);
    this->QWidget::setVisible(true);
    ui->btnAccept->setDefault(true);
    _provider.startCapturing(_images[_input_scheme.index()]);
    _cleared = true;
  } catch (QString error) {
    Gui::error (error);
    back();
  }
    return;


  case 3: //Обучение
    this->resize(QSize (720, 520));

    if ( _input_scheme.next() ) {
      back();
      return;
    }

    button (QWizard::NextButton)->setDisabled(true);
    ui->label_5->setText(QString::fromUtf8("Идёт обучение системы..."));
    ui->txtQuality->setText(QString::fromUtf8("Пожалуйста, подождите...\r\nОбучение может занять несколько минут."));
    ui->btnTest->setDisabled(true);
    ui->progressQuality->setValue(0);
    button(QWizard::NextButton)->setDisabled(true);
    button(QWizard::BackButton)->setDisabled(true);
    button(QWizard::FinishButton)->setDisabled(true);
    button(QWizard::CancelButton)->setDisabled(true);
    this->repaint ();
    //Преобразование образов
    try {
      _input_scheme.toStart();
      do {
        if (!_modules.getProcessor(_input_scheme.getCurrentProcessor(), _processor)) {
          throw QString::fromUtf8("Модуль биометрического процессора не найден!");
        }
        if (_processor.getPattern (*_patterns[_input_scheme.index()], *_images[_input_scheme.index()])) {
          _processor.setPattern(*_patterns[_input_scheme.index()]);
          _nbc.setBimPattern (_input_scheme.getCurrentSlotId(),_images[_input_scheme.index()]->getBimType(0), *_patterns[_input_scheme.index()]);
        }
        _processor.process (*_params_own[_input_scheme.index()], *_images[_input_scheme.index()]);
        _processor.getOutsBase(*_params_foreign[_input_scheme.index()]);
      } while (_input_scheme.next ());
    } catch (QString error) {
      Gui::error (error);
      back ();
      return;
    }


    //Обучение
    int errc;
    switch (errc = Client::registerContainer(_username, _nbc, _params_own, _stats))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "failed to registerContainer, errc= " << Result (errc);
        throw QString::fromUtf8 ("Невозможно зарегистрировать пользователя");
    }

    //Запись статистики
    QString qual;
    qual = QString::fromUtf8 ("Предполагаемая вероятность ошибки первого рода: ") + QString::number (_stats.auto_e1()) + QString::fromUtf8 ("\r\n");
    qual += QString::fromUtf8 ("Предполагаемая вероятность ошибки второго рода: ") + QString::number (_stats.auto_e2()) + QString::fromUtf8 ("\r\n");
    if (_stats.auto_e2d() == 0) {
      qual += QString::fromUtf8 ("Предполагаемая вероятность ошибки второго рода защищенного контейнера: менее 10e-50") + QString::fromUtf8 ("\r\n");
    } else {
      qual += QString::fromUtf8 ("Предполагаемая вероятность ошибки второго рода защищенного контейнера: ") + QString::number (_stats.auto_e2d()) + QString::fromUtf8 ("\r\n");
    }
    qual += QString::fromUtf8 ("Дополнительная стойкость пин-кода: 0.0001");
    ui->label_5->setText(QString::fromUtf8("Поздравляем! Система успешно обучена."));
    ui->txtQuality->setText(qual);
    ui->btnTest->setEnabled(true);
    ui->progressQuality->setValue(min ((double)100, -100 * log10(_stats.auto_e2()) / 8));
    button (QWizard::NextButton)->setEnabled(true);
    button(QWizard::BackButton)->setEnabled(true);
    button(QWizard::FinishButton)->setEnabled(true);
    button(QWizard::CancelButton)->setEnabled(true);
    return;
  }
  return;
}

//------------------------------------------------------------

void TrainWizard::txtNameChanged ()
{
  if (!ui->txtName->text().isEmpty()) {
    button (QWizard::NextButton)->setEnabled(true);
  } else {
    button (QWizard::NextButton)->setDisabled(true);
  }
}

//------------------------------------------------------------

void TrainWizard::txtCommentChanged ()
{
}

//------------------------------------------------------------

void TrainWizard::clickedSchemeEditor ()
{
}

//------------------------------------------------------------

void TrainWizard::listSchemesChanged ()
{
  if (ui->listSchemes_2->currentItem() == NULL) {
    button (QWizard::NextButton)->setDisabled(true);
  }

  //Создание схемы
  try {
    _nbc = SchemeConstructor::construct (ui->listSchemes_2->currentRow());
  } catch (QString error) {
    Gui::error(error);
    return;
  }

  SchemeVisualiser::view (ui->listSchemes_2->currentRow(), ui->scheme);
  SchemeVisualiser::view (ui->listSchemes_2->currentRow(), ui->scheme_2);
  SchemeVisualiser::mark(0, SCHEMEVISUALISER_STATUS_PROGRESS, ui->scheme_2);

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

  //Создание схемы ввода
  try {
    _input_scheme.extractFrom(_nbc);
  } catch (QString error) {
    Gui::error(error);
    return;
  }

  for (uint i = 0; i != _input_scheme.size (); i++) {
    BimBase *bims = new BimBase;
    Matrix *params_own = new Matrix;
    Matrix *params_foreign = new Matrix;
    Matrix *pattern = new Matrix;
    _images.push_back(bims);
    _params_own.push_back(params_own);
    _params_foreign.push_back(params_foreign);
    _patterns.push_back (pattern);
  }

  if (!ui->txtName->text().isEmpty()) {
    button (QWizard::NextButton)->setEnabled(true);
  }
}

//------------------------------------------------------------

void TrainWizard::addImage ()
{
  if (!_cleared) return;

  try {
    //Считываем образ из провайдера
    if (!_provider.capture()) {
      return;
    }
    //Добавляем элемент в гуй
    QListWidgetItem *item = new QListWidgetItem;
    item->setIcon(QIcon (":/res/48px-Mypaint-icon.svg.png"));
    ui->listImages->addItem(item);
    ui->listImages->item(ui->listImages->count()-1)->setSelected(true);
    ui->listImages->scrollToItem(ui->listImages->item(ui->listImages->count()-1));
    ui->progressImages->setValue(min (MINIMUM_TRAIN_IMAGES, ui->progressImages->value()+1));
  } catch (QString error) {
    Gui::error(error);
  }

  if (ui->listImages->count() >= MINIMUM_TRAIN_IMAGES) {
    button (QWizard::NextButton)->setEnabled(true);
  }
}

//------------------------------------------------------------

void TrainWizard::clearImage ()
{
  try {
    if (_cleared) {
      _provider.resetInput();
    } else {
      _provider.clearWindow();
    }
  } catch (QString error) {
    Gui::error(error);
  }
  _cleared = true;

}

//------------------------------------------------------------

void TrainWizard::listImagesChanged ()
{
  try {
    if ( ( ui->listImages->currentRow() >= 0 ) && (_images[_input_scheme.index()]->size () > 0) ) {
      _provider.show (ui->listImages->currentRow());
    }
  } catch (QString error) {
    Gui::error (error);
  }
  _cleared = false;
}

//------------------------------------------------------------

void TrainWizard::deleteImage ()
{
  _provider.clearWindow();
  _images[_input_scheme.index()]->erase(ui->listImages->currentRow());
  _reloadImageList ();
  _cleared = true;
}

//------------------------------------------------------------

void TrainWizard::deleteAllImages ()
{
  _provider.clearWindow();
  _images[_input_scheme.index()]->clear();

  ui->listImages->clear ();
  ui->progressImages->setValue(0);
  button (QWizard::NextButton)->setDisabled(true);
  _cleared = true;
}

//------------------------------------------------------------

void TrainWizard::_reloadImageList ()
{
  ui->listImages->clear();
  ui->progressImages->setValue (0);

  for (uint i = 0; i != _images[_input_scheme.index()]->size (); i++) {
    QListWidgetItem *item = new QListWidgetItem;
    item->setIcon(QIcon (":/res/48px-Mypaint-icon.svg.png"));
    ui->listImages->addItem(item);
    ui->listImages->item(ui->listImages->count()-1)->setSelected(true);
    ui->listImages->scrollToItem(ui->listImages->item(ui->listImages->count()-1));
    ui->progressImages->setValue(min (MINIMUM_TRAIN_IMAGES, ui->progressImages->value()+1));
  }
  if (ui->listImages->count() < MINIMUM_TRAIN_IMAGES) {
    button (QWizard::NextButton)->setDisabled(true);
  } else {
    button (QWizard::NextButton)->setEnabled(true);
  }
}

//------------------------------------------------------------

void TrainWizard::finishClicked (int id)
{
  int errc;
  if (id == QDialog::Accepted) {
    switch (errc = Client::confirmContainer())
    {
    case nbS_OK:
      break;
    default:
      qDebug() << "failed to confirmRegistration, errc=" << Result (errc);
      throw QString::fromUtf8 ("Невозможно завершить регистрацию пользователя");
    }
  }
}

//------------------------------------------------------------

void TrainWizard::clickedKey ()
{
  QString path = QFileDialog::getOpenFileName (this, QString::fromUtf8("Выберите файл с ключом"), "", QString::fromUtf8("Любой тип файла (*.*)"));
  if (path.isEmpty()) {
    return;
  }

  QFile file (path);
  if (!file.open (QFile::ReadWrite)) {
    Gui::error (QString::fromUtf8("Невозможно открыть файл с ключом"));
    return;
  }

  if (file.size () != 32) {
    Gui::error (QString::fromUtf8("Неправильный файл с ключом. Файл должен состоять из строки длиной 32 символа."));
    return;
  }

  QByteArray raw = file.readLine();
  if (raw.length() != 32) {
    Gui::error (QString::fromUtf8("Неправильный файл с ключом. Файл должен состоять из строки длиной 32 символа."));
    return;
  }
  ui->txtKey->setText(raw);

  _makeKey ();

  button (QWizard::NextButton)->setEnabled(true);
}

//------------------------------------------------------------

void TrainWizard::changeKey ()
{
  if (ui->txtKey->text().length() != 32) {
    button (QWizard::NextButton)->setDisabled(true);
  } else {
    _makeKey ();
    button (QWizard::NextButton)->setEnabled(true);
  }
}

//------------------------------------------------------------

void TrainWizard::loadBims ()
{
  ui->widget->setDisabled(true);
  QString path = QFileDialog::getOpenFileName (this, QString::fromUtf8("Выберите файл с базой биометрических образов"), "", QString::fromUtf8("Биометрические образы (*.bim)"));
  ui->widget->setEnabled(true);
   if (path.isEmpty()) return;
  try {
    _images[_input_scheme.index()]->load(path, _provider.getBimType ());
    _reloadImageList ();
  } catch (QString error) {
    Gui::error(error);
  }
}

//------------------------------------------------------------

void TrainWizard::saveBims ()
{
  ui->widget->setDisabled(true);
  QString path = QFileDialog::getSaveFileName (this, QString::fromUtf8("Выберите файл для сохранения биометрических образов ключом"), "", QString::fromUtf8("Биометрические образы (*.bim)"));
  ui->widget->setEnabled(true);
  if (path.isEmpty()) return;
  try {
    _images[_input_scheme.index()]->save(path, _images[_input_scheme.index()]->getBimType(0));
  } catch (QString error) {
    Gui::error(error);
  }
}

//------------------------------------------------------------

void TrainWizard::_makeKey ()
{
  _code.remeta(Meta(256,MfI1,MtDO));
  _code.resize(1);
  Matrix::Row key = _code.row(0);
  for (int32_t i=0; i<32; ++i) key.at(0).u8[i] = 0x55;
}

//////////////////////////////////////////////////////////////////////////
//файл:
//версия:
//дата:
//изменен:
//автор:        Майоров А.В.
//
//цель:         Объявление класса биометрического контейнера его блоков,
//              фабрик блоков, которые используются преобразователем
//              биометрия-код или в протоколах аутентификации
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_CONTAINER_H
#define _NB_CONTAINER_H

#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>

#include "Enums.h"
#include "Uuid.h"
#include "Data.h"
#include "Matrix.h"
#include "Scheme.h"

#pragma pack(1)

namespace Nb {

using namespace std;


//.............................................................................
//Блок биометрического контейнера
//.............................................................................
class Block {
public:
  Block();
  Block(BlockType bt);
  virtual ~Block();
  //Полный блока
  int32_t size() const;
  //Размер данных
  int32_t sizeOfData() const;
  //Тип блока
  BlockType type() const;
  //Установка типа блока
  void type(BlockType bt);
  //Доступ к данным
  uint8_t* data() const;
  //Доступ к данным по смещению
  uint8_t* at(int32_t pos) const;
  //Проверка наличия данных
  bool isEmpty() const;
  //Определение поддерживаемого типа
  bool isSupportedType(BlockType type) const;
  BlockType getDefaultType() const;
  //Проверка валидности полей согласно типу
  virtual bool isValid() const;

  //Проверка заголовочного файла
  virtual bool isHeader() const;

  //Чтение и запись
  int32_t read(nbBlockHeader header, QDataStream &from);
  int32_t read(QDataStream &from);
  int32_t write(QDataStream &to) const;

  //Добавление данных в конец блока (блок увеличивается в размере)
  //примечание: Позволяет использовать потоковую запись push(A).push(B).push(C)
  Block& push(const uint8_t *data, int32_t size);
  Block& push(int8_t data);
  Block& push(int16_t data);
  Block& push(int32_t data);
  Block& push(const Uuid &data);
  Block& push(const Data &data, bool withSize=false);
  Block& push(const Matrix &data);
  Block& push(const Scheme &data);

  //Чтение с указанной позиции
  //  pos - [вх/вых] позиция, с которой начинается чтение.
  //          в случае ошибки устанавливается в значение > sizeOfData()
  //примечание:
  //  В конце операции изменяется на число считанных байт,
  //  поэтому возможно последовательное чтение pop(x,A).pop(x,B).pop(x,C)
  const Block& pop(int32_t &pos, uint8_t* data, int32_t size) const;
  const Block& pop(int32_t &pos, int8_t &data) const;
  const Block& pop(int32_t &pos, int16_t &data) const;
  const Block& pop(int32_t &pos, int32_t &data) const;
  const Block& pop(int32_t &pos, Uuid &data) const;
  const Block& pop(int32_t &pos, Data &data, bool withSize) const;
  const Block& pop(int32_t &pos, Matrix &data) const;
  const Block& pop(int32_t &pos, Scheme &data) const;
  //Пропуск указанного числа байт при чтении
  const Block& skip(int32_t &pos, int32_t plus) const;

  QByteArray& asByteArray(){ return _data; }
  //Обновление размера блока
  virtual void update(){ setHSize(_data.size()); }

  //Освобождение блока
  virtual void free();
  //Копирование данных
  bool copy(const Block &block);

  //Дублирование блока (с учетом класса блока)
  virtual Block* dup() const;

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug virtual debug(QDebug  dbg) const ;
#endif
protected:
  //Запись и чтение поля размера блока заголовка
  void      setHSize(uint32_t size);
  uint32_t  getHSize() const;
protected:
  nbBlockHeader _h;
  QByteArray    _data;
};


typedef QList<Block*> Blocks;

class BlockHead: public Block {
public:
  //Обновление заголовка
  //  blocks  - [вх] список блоков
  //  ibegin  - [вх] позиция первого блока контейнера (номер блока заголовка)
  //  iend    - [вх] позиция следующего блока контейнера за последним
  virtual bool update(const Blocks &blocks, int32_t ibegin, int32_t iend)=0;

  //Поиск конца блока
  //  blocks  - [вх] список блоков контейнера
  //  ibegin  - [вх] позиция первого блока контейнера (номер блока заголовка)
  //возвращает: номер следующего за последним блоком блока
  virtual int32_t findEnd(const Blocks &blocks, int32_t ibegin) const =0;

  //Получение текущего значения поля размера контейнера (если поддерживается)
  virtual int32_t sizeOfBlocks() const =0;
  //Получение текущего значения числа контейнеров (если поддерживается)
  virtual int32_t countOfBlocks() const =0;
};

//.............................................................................
//  Блок заголовка биометрического контейнера
//.............................................................................
class BlockContHead: public BlockHead {
public:
  BlockContHead();
  BlockContHead(const Uuid &ct, int32_t csize=0);

  virtual bool isHeader() const;

  //Обновление заголовка
  //  blocks  - [вх] список блоков
  //  ibegin  - [вх] позиция первого блока контейнера (номер блока заголовка)
  //  iend    - [вх] позиция последнего блока контейнера + 1
  virtual bool update(const Blocks &blocks, int32_t ibegin, int32_t iend);

  //Поиск конца блока
  //  blocks  - [вх] список блоков контейнера
  //  ibegin  - [вх] позиция первого блока контейнера
  //возвращает: номер последнего блока контейнера + 1
  //            ibegin в случае ошибки
  virtual int32_t findEnd(const Blocks &blocks, int32_t ibegin) const;

  //Получение текущего значения поля размера контейнера (если поддерживается)
  virtual int32_t sizeOfBlocks() const;
  //Получение текущего значения числа контейнеров (если поддерживается)
  virtual int32_t countOfBlocks() const ;

  //Чтение типа контейнера
  virtual Uuid containerType() const;
  //Инициализация блока
  //  ct    - тип контейнера
  //  csize - полный размер контейнера (пересчитывается в update)
  virtual void init(const Uuid &ct, int32_t csize=0);

  //Дублирование блока (с учетом класса блока)
  virtual Block* dup() const;

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug virtual debug(QDebug  dbg) const;
#endif
};

//.............................................................................
//Биометрический контейнер
//.............................................................................
class Container {
public:
  //.............................................................................
  //  Фабрика классов для загрузки контейнеров
  //.............................................................................
  class Factory {
  public:
    //Создание однотипных блоков
    //  blocks  - текущие загруженные блоки (создаваемый блок будет добавлен в конец)
    //  header  - заголовок текущего блока
    //  body    - поток, из которого будет считано оставшееся тело блока
    virtual Block* create(const Blocks &blocks, nbBlockHeader &header) const;
    //Освобождение выделенной памяти
    virtual void   free(Block* block) const;
  };

  Container(const Container::Factory &factory);
  //Общий размер контейнера, включая вложенные для сохранения
  virtual int32_t size() const;
  //Число блоков контейнера
  virtual int32_t count() const;
  //Проверка наличия блоков
  bool isEmpty();
  //Проверка наличия вложенных контейнеров
  bool isComposite();
  //Освобождение
  void free();

  //Копирование контейнера
  bool copy(const Container &bc);
  //Копирование вложенного контейнера, начиная с указанной позиции
  //  ibegin - [вх] позиция начала контейнера
  bool copyNested(const Container &bc, int32_t ibegin);

  //Добавление блока в конец контейнера
  Container& push(const Block &block);
  //Добавление блоков контейнера в конец текущего
  Container& push(const Container &bc, int32_t ibegin=0, int32_t iend=0);
  //Удаление последних блоков
  Container& pop(int32_t count);


  //Сквозной доступ к блоку контейнера по позиции
  Block* at(int32_t pos) const;
  inline Block* first() const { return _blocks[0]; }
  inline Block* last() const { return _blocks[_blocks.count()-1]; }
  const Blocks& all() const;
  //Поиск первого блока вложенного контейнера
  //  pos - [вх] позиция, с которой ведется поиск заголовка контейнера
  //возвращает: номер блока заголовка,
  //            count() в случае ошибки
  int32_t findBegin(int32_t pos) const;
  //Поиск последнего блока вложенного контейнера, заданного началом startPos
  //  ibegin - [вх] блок заголовка контейнера, для которого ведется поиск последнего блока
  //                с учетом вложенных контейнеров
  //возвращает: номер последнего блока вложенного контейнера + 1
  //            ibegin в случае ошибки
  int32_t findEnd(int32_t ibegin) const;

  const Factory& factory() const;

  //Чтение из потока
  int32_t read(QDataStream &from, int32_t count = -1);
  //Сохранение в поток
  int32_t write(QDataStream &to, bool asData = false);

protected:
  Blocks _blocks;
  Factory _factory;
};


#if !defined(QT_NO_DEBUG_STREAM)
QDebug operator <<(QDebug  dbg, const Block &block);
QDebug operator <<(QDebug  dbg, const Blocks &blocks);
QDebug operator <<(QDebug  dbg, const Container &container);
#endif

}//namespace Nb

#pragma pack()
#endif //_NB_CONTAINER_H

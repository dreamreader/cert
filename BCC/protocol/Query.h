#ifndef QUERYBLOCK_H
#define QUERYBLOCK_H

#include <QDataStream>
#include <QTcpSocket>
#include <QList>
#include "../protocol/log.h"
#include "../protocol/nbqueryheader.h"
#include "../nb/nbTypes.h"
#include "../nb/Container.h"


/// Класс запроса
class Query
{
public:
  class QueryHeaderBlock;
  class DataBlock;
  typedef QList<DataBlock*> DataBlocks;

public:
  enum Type
  {
    Unknown = 0,
    WantAuth = 1,
    WantAuthBio = 2,
    WantContainer = 3,
    WantContainers = 4,
    WantRegister = 5,
    WantSignature = 6,
    Ans = 7,
    Data = 8,
    WaitData = 9,
    RepeatData = 10,
    Key = 11,
    Granted = 12,
    Denied = 13,
    Stats = 14,
    Test = 15,
    Ok = 16,
    Err = 17
  };

  bool              _skipHeader;         ///< заголовок присоединён
  DataBlocks        _allocBlocks;        ///< выделенные в памяти блоки
  QueryHeaderBlock* _headerBlock;        ///< блок заголовка запроса
  DataBlocks        _dataBlocks;         ///< блоки данных запроса

  /* Конструктор по умолчанию
   *
   */
  Query();

  /* Конструктор
   *
   */
  Query(Type type);

  /* Деструктор
   *
   */
  ~Query();

  /* Получить поличество блоков
   *
   */
  int count();

  /* Добавить блок в конец
   *
   */
  bool push(DataBlock* block);

  /* Получить блок
   *
   */
  DataBlock* at(unsigned pos);

  /* Удалить первый блок
   *
   */
  bool pop(DataBlock* block);

  /* Очистить запрос
   *
   */
  bool clear();

  /* Создать запрос
   *
   */
  bool create(Type type);

  /* Создать запрос с заданным заголовком
   *
   */
  bool create(Query::QueryHeaderBlock &header);

  /* Проверить запрос
   *
   */
  bool typeIs(Type type);

  /* Получить тип
   *
   */
  Type type();

  /* Принять запрос
   *
   */
  virtual bool read(QTcpSocket &socket);

  /* Отправить запрос
   *
   */
  virtual bool write(QTcpSocket &socket);

protected:
  /* Освободить выделенные блоки
   *
   */
  virtual bool free();

public:

  /// Класс блока данных запроса
  class DataBlock : public Nb::Block
  {
  protected:
    Nb::Block*  _ref;

  public:

    /* Конструктор по умолчанию
     *
     */
    DataBlock();

    /* Конструктор
     *
     */
    DataBlock(Nb::Block *block);

    /* Получить тип блока по умолчанию
     *
     */
    virtual Nb::BlockType getDefaultType() const {return nbBT_PROTOCOL; }

    /* Считать заголовок блока
     *
     */
    virtual bool readHeader(QTcpSocket &socket);

    /* Считать тело блока
     *
     */
    virtual bool readBody(QTcpSocket &socket);

    /* Считать блок
     *
     */
    virtual bool read(QTcpSocket &socket);

    /* Дублировать блок
     *
     */
    virtual DataBlock* dup() const;

    /* Записать блок
     *
     */
    virtual bool write(QTcpSocket &socket) const;

    /* Получить размер данных
     *
     */
    int32_t expectedSize() const;

  };


  /// Класс блока заголовка запроса
  class QueryHeaderBlock : public DataBlock
  {
  public:
    QueryHeaderBlock();

    virtual void free();

    // Получить количество блоков данных запроса
    uint16_t& getCount();

    // Установить количество блоков данных запроса
    void setCount(uint16_t count);

    // Получить тип запроса
    Type getType();

    // Установить тип запроса
    void setType(Query::Type type);

    // Проверить тип запроса
    bool typeIs(Type type);

    // Получить флаги запроса
    nbQueryFlags& getFlags();

    // Установить флаги запроса
    void setFlags(nbQueryFlags flags);

    bool readBody(QTcpSocket &socket);
  };
};

#endif // NBQUERYBLOCK_H

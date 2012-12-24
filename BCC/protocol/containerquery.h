#ifndef CONTAINERQUERY_H
#define CONTAINERQUERY_H

#include "Query.h"
#include "../nb/Container.h"
#include "../nb/Matrix.h"

class ContainerQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

private:
  Query::DataBlocks       _blocks;
  Nbc                     _cont;

public:
  /* Конструктор по умолчанию
   *
   */
  ContainerQuery();

  ~ContainerQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(Nb::Container &container);

  /* Принять запрос
   *
   */
  virtual bool read(QTcpSocket &socket);

  /* Получить контейнер
   *
   */
  Nbc* container();

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // CONTAINERQUERY_H

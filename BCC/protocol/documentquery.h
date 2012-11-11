#ifndef DOCUMENTQUERY_H
#define DOCUMENTQUERY_H

#include "query.h"

class DocumentQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  static const unsigned blockCount = 1;

  Query::DataBlock   _documentBlock;           ///< блок документа

public:
  /* Конструктор
   *
   */
  DocumentQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(Nb::Data &document);

  /* Получить данные блоков запроса
   *
   */
  bool get(Nb::Data &document);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // DOCUMENTQUERY_H

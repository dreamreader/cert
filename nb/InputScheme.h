#ifndef INPUTSCHEME_H
#define INPUTSCHEME_H

//Схема ввода биометрических данных
//note в схеме все слоты, связанные блоком альтернативы, должны идти подряд

#include "../nb/Uuid.h"
#include "../nb/SlotId.h"

#include "../nb/Nbc.h"

using namespace Nb;

class InputScheme
{
public:
  InputScheme ();
  ~InputScheme ();
  bool destroy ();

  Uuid getCurrentProvider ();
  Uuid getCurrentProcessor ();
  SlotId getCurrentSlotId ();

  int branchesCount();

  bool toStart ();
  bool toFirstBranch();

  size_t index ();
  size_t size ();
  bool next ();
  bool prev ();

  size_t step ();
  size_t steps ();
  bool nextStep ();
  bool prevStep ();

  //Извлечение схемы ввода из контейнера со схемой и блоками BtTechId
  bool extractFrom (Nbc &container);
  //Создание из схемы ввода неполной схемы преобразования и блоков BtTechId
  bool writeTo (Nbc &container);

private:
  int _step, _branch, _index;
  int _length, _size; // количество шагов, общее количество технологий
  QList<int> _branch_ids;
  SlotIds _slot_ids;
  Uuids _providers, _processors;
};

#endif // INPUTSCHEME_H

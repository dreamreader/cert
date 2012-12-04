#include "InputScheme.h"

InputScheme::InputScheme ()
{
  destroy ();
}

InputScheme::~InputScheme ()
{
  destroy ();
}

bool InputScheme::destroy ()
{
  _size = _length = _index = _branch = _step = 0;
  _branch_ids.clear();
  _slot_ids.clear ();
  _providers.clear ();
  _processors.clear ();
  return true;
}

Uuid InputScheme::getCurrentProvider ()
{
  if (_index >= _providers.size ()) return Uuid::nil;
  return _providers[_index];
}

Uuid InputScheme::getCurrentProcessor ()
{
  if (_index >= _processors.size ()) return Uuid::nil;
  return _processors[_index];
}

SlotId InputScheme::getCurrentSlotId ()
{
  return _slot_ids[_index];
}

size_t InputScheme::index ()
{
  return _index;
}

size_t InputScheme::step ()
{
  return _step;
}

size_t InputScheme::steps ()
{
  return _length;
}

size_t InputScheme::size ()
{
  return _size;
}

bool InputScheme::next ()
{ //переход к следующему элементу, включая элементы "ИЛИ" - при обучении
  if (_index == _size-1) {
    return false; //край схемы
  }
  _index++;
  if (_branch < branchesCount () - 1) {
    _branch++;
  } else {
    _step++;
    _branch = 0;
  }
  return true;
}

bool InputScheme::prev ()
{ //переход к предыдущему элементу, включая элементы "ИЛИ"
  if (_index == 0) {
    return false; //край схемы
  }
  _index--;
  if (_branch > 0) {
    _branch--;
  } else {
    _step--;
    _branch = branchesCount () - 1;
  }
  return true;
}

bool InputScheme::nextStep ()
{ //Переход к следующему шагу (без перехода по элементам ИЛИ)
  if (_index == _size - 1) {
    return false; //край схемы
  }
  if (_branch_ids[_index] == 0 ) {
      return next ();
  }

  int next_id, branch_id = _branch_ids[_index];
  do {
    _index++;
    if (_index == _size-1) {
        return false;
    }
    next_id = _branch_ids[_index];
  } while (branch_id == next_id);

  _step++;
  _branch = 0;

  return true;
}

bool InputScheme::prevStep ()
{ //Переход к предыдущему шагу (без перехода по элементам ИЛИ)
  if ((_step == 0) || (_index == _size - 1)) {
    return false; //край схемы
  }

  int next_id, branch_id = _branch_ids[_index];
  do {
    _index--;
    next_id = _branch_ids[_index];
  } while (branch_id == next_id);

  _step--;
  toFirstBranch();

  return true;
}

bool InputScheme::extractFrom (Nbc &container)
{
  destroy ();

  int branch_id = 0;

  Scheme scheme = container.getScheme ();
  _slot_ids = scheme.ids(StIn);
  _size = _slot_ids.size ();

  //ищем все входные слоты
  for (int i = 0; i != _size; i++) {
    if ( container.getSlotInfo(_slot_ids[i]).size() < 2 ) {
      continue;
    }
    _providers.push_back( (container.getSlotInfo(_slot_ids[i]))[0] );
    _processors.push_back( (container.getSlotInfo(_slot_ids[i]))[1] );
    _branch_ids.push_back(branch_id);
  }

  //ищем все слоты альтернативы
  Slots cond_slots;
  scheme.find(NULL, NULL, NULL, &cond_slots);

  if (_size > 0) _length = 1;
  //ищем и помечаем связанные блоком альтернативы слотов
  for (int i = 0; i != cond_slots.size (); i++) {
    if (cond_slots[i]->id().btype() != BtCondition) {
      continue;
    }
    SlotIds cids = cond_slots[i]->cids ();
    branch_id++;
    _length++;
    //связанные слоты - выходные слоты преобразователя - идем вглубь
    if (cids[0].btype() == BtNeuralConv) {
      Slots nconvs;
      scheme.find (nconvs, cids);
      cids.clear ();
      for (int j = 0; j != nconvs.size (); j++) {
        cids.push_back(nconvs[j]->cids()[0]);
      }
    }

    for (int j = 0; j != _slot_ids.size (); j++) {
      for (int k = 0; k != cids.size (); k++) {
        if (_slot_ids[j] == cids[k]) {
          _branch_ids[j] = branch_id;
        }
      }
    }
  }
  toStart ();

  return true;
}

bool InputScheme::writeTo (Nbc &container)
{
  return false;
}

bool InputScheme::toStart ()
{ //Переход в начало схемы
  _index = 0;
  _step = 0;
  _branch = 0;
  return true;
}

bool InputScheme::toFirstBranch()
{
  _branch = 0;
  int next_id, branch_id = _branch_ids[_index];
  do {
    if (_index == 0) return true;
    _index--;
    next_id = _branch_ids[_index];
  } while (branch_id == next_id);
  _index++;

  return true;
}

int InputScheme::branchesCount ()
{
  int counter = 1;
  int i;
  int next_id;
  if (_size == 0) {
    return 0;
  }
  if (_branch_ids[_index] == 0) {
    return 0;
  }
  int branch_id = _branch_ids[_index];
  i = _index;
  while (i > 0) { //до
    i--;
    next_id = _branch_ids[i];
    if (next_id != branch_id) {
      break;
    }
    counter++;
  }

  i = _index;
  while (i < _size-1) { //после
    i++;
    next_id = _branch_ids[i];
    if (next_id != branch_id) {
      break;
    }
    counter++;
  }

  return counter;
}

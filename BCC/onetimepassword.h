#ifndef ONETIMEPASSWORD_H
#define ONETIMEPASSWORD_H

#include "rndmt.h"
#include "time.h"
#include "nb/Matrix.h"
#include "nb/Data.h"

class OneTimePassword
{
private:
  Nb::Matrix _passwordMatrix;   //матрица с паролем

public:
  /* Конструктор
   *
   */
  OneTimePassword();
  OneTimePassword(unsigned size, bool random = true, uint8_t fillWith = 0x55);

  void create(unsigned size, bool random = true, uint8_t fillWith = 0x55);

  /* Приведение к матрице
   *
   */
  operator nbMatrix&();
  operator Nb::Matrix&();

  bool operator == (Nb::Matrix &matrix);
  bool isEqual(Nb::Data &data);

};

#endif // ONETIMEPASSWORD_H

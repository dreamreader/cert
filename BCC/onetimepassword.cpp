#include "onetimepassword.h"
#include "protocol/log.h"

/* Конструктор
 *
 */
OneTimePassword::OneTimePassword(unsigned size, bool random, uint8_t fillWith)
{
  _passwordMatrix.remeta(1, new Nb::Meta(size, Nb::MfI1, Nb::MtDiscreteOwn));
  _passwordMatrix.resize(1);

  Nb::Vbp dst = _passwordMatrix.data()[0][0];

  if (random)
  {
    unsigned total(0), needed(0);
    random::RndMT rndMt(time(NULL));
    unsigned long val;
    while (total < size)
    {
      val = rndMt.rand(ULONG_MAX);
      needed = size - total;
      if (needed >= sizeof(unsigned long)) needed = sizeof(unsigned long);
      memcpy(dst, &val, needed);
      total += needed;
    }
  }
  else
  {
      for (unsigned i=0; i<size/8; i++)
        dst.u8[i] = fillWith;
  }
}

/* Приведение к матрице
 *
 */
OneTimePassword::operator nbMatrix&()
{
  return (nbMatrix&)_passwordMatrix;
}

OneTimePassword::operator Nb::Matrix&()
{
  return _passwordMatrix;
}

bool OneTimePassword::operator == (Nb::Matrix &matrix)
{
  Log::write("compare");
  if ((matrix.nrows() != 1) ||
      (matrix.ncols() != 1) ||
      (matrix.meta()->count() != _passwordMatrix.meta()->count()) ||
      (matrix.meta()->format() != _passwordMatrix.meta()->format()) ||
      (matrix.meta()->type() != _passwordMatrix.meta()->type()))
    return false;

  Log::write("format ok!");
  int ham(0);
  for (unsigned b=0; b<_passwordMatrix.meta()->count(); b++)
  {    
    if (_passwordMatrix.row(0).at(0).i1(b) !=
        matrix.row(0).at(0).i1(b))
    {
      ham++;
      //return false;
    }
  }
  Log::write("ham", (int)ham);
  return (ham == 0);
}


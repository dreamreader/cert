#ifndef BCBITSTREAM_H
#define BCBITSTREAM_H

#include <QDataStream>

namespace Bc {

  class BitStream{
  public:
    enum BitOrder{
      BigEndian=0,
      LittleEndian=1
    };

    BitStream(QDataStream &s, BitOrder bitOrder):_stream(s),_order(bitOrder){
      inb  = 0;
      outb = 0;
      nempty = 8;
      nfilled = 0;
    }
    ~BitStream(){ writeAlign(); readAlign(); }

    //Запись бит (до 32)
    //примечание: беззнаковые числа нужны для корректного выполнения операции >>
    bool writeBits(unsigned x, unsigned nbits);
    //Запись 0 до границы байта
    bool writeAlign();
    //Чтение бит
    bool readBits(unsigned &x, unsigned nbits);
    //Чтение 0 из потока до границы байта
    void readAlign();

  protected:
    QDataStream &_stream;
    unsigned    inb;
    unsigned    nempty;

    unsigned    outb;
    unsigned    nfilled;
    BitOrder    _order;
    static int _bmask[];
  };

}






#endif // BCBITSTREAM_H

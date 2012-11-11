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

    //������ ��� (�� 32)
    //����������: ����������� ����� ����� ��� ����������� ���������� �������� >>
    bool writeBits(unsigned x, unsigned nbits);
    //������ 0 �� ������� �����
    bool writeAlign();
    //������ ���
    bool readBits(unsigned &x, unsigned nbits);
    //������ 0 �� ������ �� ������� �����
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

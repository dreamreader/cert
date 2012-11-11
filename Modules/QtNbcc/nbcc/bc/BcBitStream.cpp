#include <QDebug>
#include "BcBitStream.h"

namespace Bc {



int BitStream::_bmask[] = {
            0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff,
            0x1ff,0x3ff,0x7ff,0xfff,0x1fff,0x3fff,0x7fff,0xffff,
            0x1ffff,0x3ffff,0x7ffff,0xfffff,0x1fffff,0x3fffff,
            0x7fffff,0xffffff,0x1ffffff,0x3ffffff,0x7ffffff,
            0xfffffff,0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff};

//Кодирование bits/BigEndian
//Записывает в поток с младшего бита (первый - старший), а читает со старшего
//write     >> [01234567] >> stream
//stream    << [01234567] >> read
//

//Кодирование bits/LittleEndian
//Другой способ записи со старшего бита сложнее в реализации
//stream << [01234567] << writeBits
//read   << [01234567] << stream
//

bool BitStream::writeBits(unsigned x, unsigned nbits){
  if (_order == BigEndian){
    while (nbits >= nempty){
      outb = (outb << nempty) | (x >> (nbits - nempty));  //Присоединить старшие биты x с младшими outb
      if (_stream.writeRawData((const char*)&outb,1)!=1)
        return false;
      x &= _bmask[nbits - nempty];  //Замаскировать старшие биты "0"
      nbits -= nempty;
      outb   = 0;
      nempty = 8;
    }
    if (nbits > 0){
      outb = (outb << nbits) | x;
      nempty -= nbits;
    }
  } else {
    //Записывать начиная с младшего бита к старшему
    while (nbits >= nempty){
      outb |= x <<(8-nempty);
      if (_stream.writeRawData((const char*)&outb,1)!=1)
        return false;
      x >>= nempty; //Удалить записанные младшие биты
      nbits -= nempty;
      outb   = 0;
      nempty = 8;
    }
    if (nbits > 0){
      outb |= (x & _bmask[nbits])<<(8-nempty);
      nempty -= nbits;
    }
  }
  return true;
}



bool BitStream::writeAlign(){
  if (_order == BigEndian){
    if (nempty != 8) {
      outb <<= nempty;
      if (_stream.writeRawData((const char*)&outb,1)!=1)
        return false;
      nempty = 8;
      outb  = 0;
    }
  } else {
    if (nempty != 8) {
      outb &= _bmask[8-nempty]; //Убрать возможные артефакты
      if (_stream.writeRawData((const char*)&outb,1)!=1)
        return false;
      nempty = 8;
      outb  = 0;
    }
  }
  return true;
}



bool BitStream::readBits(unsigned &x, unsigned nbits){
  if (_order == BigEndian){
    x = 0;
    while (nbits > nfilled){
      x |= inb << (nbits - nfilled);  //Вытолкнуть старшие биты inb к младшим x
      nbits -= nfilled;
      if (_stream.readRawData((char*)&inb,1)!=1)
        return false;
      nfilled = 8;
    }
    if (nbits > 0){
      x |= inb >> (nfilled - nbits);  //Сместить старшие биты в начало и записать "остаток" в начало x
      inb &= _bmask[nfilled - nbits]; //Оставить только не считанные младшие биты
      nfilled -= nbits;
    }
  } else {
    unsigned h = 32-nbits;
    while (nbits > nfilled){
      x = x>>nfilled | inb;
      nbits -= nfilled;
      if (_stream.readRawData((char*)&inb+3,1)!=1)//Считать данные в старший байт
        return false;
      nfilled = 8;
    }
    if (nbits > 0){
      x = (x>>nbits) | (inb << (nfilled - nbits));
      nfilled -= nbits;
      inb &= ~_bmask[32-nfilled];//Убрать считанные младшие биты
    }
    x>>=h;
  }
  return true;
}



void BitStream::readAlign(){
  inb = nfilled = 0;
}



}

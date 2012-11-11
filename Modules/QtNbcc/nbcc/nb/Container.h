//////////////////////////////////////////////////////////////////////////
//����:
//������:
//����:
//�������:
//�����:        ������� �.�.
//
//����:         ���������� ������ ��������������� ���������� ��� ������,
//              ������ ������, ������� ������������ ����������������
//              ���������-��� ��� � ���������� ��������������
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_CONTAINER_H
#define _NB_CONTAINER_H

#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>

#include "Enums.h"
#include "Uuid.h"
#include "Data.h"
#include "Matrix.h"
#include "Scheme.h"

#pragma pack(1)

namespace Nb {

using namespace std;


//.............................................................................
//���� ��������������� ����������
//.............................................................................
class Block {
public:
  Block();
  Block(BlockType bt);
  virtual ~Block();
  //������ �����
  int32_t size() const;
  //������ ������
  int32_t sizeOfData() const;
  //��� �����
  BlockType type() const;
  //��������� ���� �����
  void type(BlockType bt);
  //������ � ������
  uint8_t* data() const;
  //������ � ������ �� ��������
  uint8_t* at(int32_t pos) const;
  //�������� ������� ������
  bool isEmpty() const;
  //����������� ��������������� ����
  bool isSupportedType(BlockType type) const;
  BlockType getDefaultType() const;
  //�������� ���������� ����� �������� ����
  virtual bool isValid() const;

  //�������� ������������� �����
  virtual bool isHeader() const;

  //������ � ������
  int32_t read(nbBlockHeader header, QDataStream &from);
  int32_t read(QDataStream &from);
  int32_t write(QDataStream &to) const;

  //���������� ������ � ����� ����� (���� ������������� � �������)
  //����������: ��������� ������������ ��������� ������ push(A).push(B).push(C)
  Block& push(const uint8_t *data, int32_t size);
  Block& push(int8_t data);
  Block& push(int16_t data);
  Block& push(int32_t data);
  Block& push(const Uuid &data);
  Block& push(const Data &data, bool withSize=false);
  Block& push(const Matrix &data);
  Block& push(const Scheme &data);

  //������ � ��������� �������
  //  pos - [��/���] �������, � ������� ���������� ������.
  //          � ������ ������ ��������������� � �������� > sizeOfData()
  //����������:
  //  � ����� �������� ���������� �� ����� ��������� ����,
  //  ������� �������� ���������������� ������ pop(x,A).pop(x,B).pop(x,C)
  const Block& pop(int32_t &pos, uint8_t* data, int32_t size) const;
  const Block& pop(int32_t &pos, int8_t &data) const;
  const Block& pop(int32_t &pos, int16_t &data) const;
  const Block& pop(int32_t &pos, int32_t &data) const;
  const Block& pop(int32_t &pos, Uuid &data) const;
  const Block& pop(int32_t &pos, Data &data, bool withSize) const;
  const Block& pop(int32_t &pos, Matrix &data) const;
  const Block& pop(int32_t &pos, Scheme &data) const;
  //������� ���������� ����� ���� ��� ������
  const Block& skip(int32_t &pos, int32_t plus) const;

  QByteArray& asByteArray(){ return _data; }
  //���������� ������� �����
  virtual void update(){ setHSize(_data.size()); }

  //������������ �����
  virtual void free();
  //����������� ������
  bool copy(const Block &block);

  //������������ ����� (� ������ ������ �����)
  virtual Block* dup() const;

  #if !defined(QT_NO_DEBUG_STREAM)
    QDebug virtual debug(QDebug  dbg) const ;
  #endif
protected:
  //������ � ������ ���� ������� ����� ���������
  void      setHSize(uint32_t size);
  uint32_t  getHSize() const;
protected:
  nbBlockHeader _h;
  QByteArray    _data;
};


typedef QList<Block*> Blocks;

class BlockHead: public Block {
public:
  //���������� ���������
  //  blocks  - [��] ������ ������
  //  ibegin  - [��] ������� ������� ����� ���������� (����� ����� ���������)
  //  iend    - [��] ������� ���������� ����� ���������� �� ���������
  virtual bool update(const Blocks &blocks, int32_t ibegin, int32_t iend)=0;

  //����� ����� �����
  //  blocks  - [��] ������ ������ ����������
  //  ibegin  - [��] ������� ������� ����� ���������� (����� ����� ���������)
  //����������: ����� ���������� �� ��������� ������ �����
  virtual int32_t findEnd(const Blocks &blocks, int32_t ibegin) const =0;

  //��������� �������� �������� ���� ������� ���������� (���� ��������������)
  virtual int32_t sizeOfBlocks() const =0;
  //��������� �������� �������� ����� ����������� (���� ��������������)
  virtual int32_t countOfBlocks() const =0;
};

//.............................................................................
//  ���� ��������� ��������������� ����������
//.............................................................................
class BlockContHead: public BlockHead {
public:
  BlockContHead();
  BlockContHead(const Uuid &ct, int32_t csize=0);

  virtual bool isHeader() const;

  //���������� ���������
  //  blocks  - [��] ������ ������
  //  ibegin  - [��] ������� ������� ����� ���������� (����� ����� ���������)
  //  iend    - [��] ������� ���������� ����� ���������� + 1
  virtual bool update(const Blocks &blocks, int32_t ibegin, int32_t iend);

  //����� ����� �����
  //  blocks  - [��] ������ ������ ����������
  //  ibegin  - [��] ������� ������� ����� ����������
  //����������: ����� ���������� ����� ���������� + 1
  //            ibegin � ������ ������
  virtual int32_t findEnd(const Blocks &blocks, int32_t ibegin) const;

  //��������� �������� �������� ���� ������� ���������� (���� ��������������)
  virtual int32_t sizeOfBlocks() const;
  //��������� �������� �������� ����� ����������� (���� ��������������)
  virtual int32_t countOfBlocks() const ;

  //������ ���� ����������
  virtual Uuid containerType() const;
  //������������� �����
  //  ct    - ��� ����������
  //  csize - ������ ������ ���������� (��������������� � update)
  virtual void init(const Uuid &ct, int32_t csize=0);

  //������������ ����� (� ������ ������ �����)
  virtual Block* dup() const;

  #if !defined(QT_NO_DEBUG_STREAM)
    QDebug virtual debug(QDebug  dbg) const;
  #endif
};

//.............................................................................
//�������������� ���������
//.............................................................................
class Container {
public:
  //.............................................................................
  //  ������� ������� ��� �������� �����������
  //.............................................................................
  class Factory {
  public:
    //�������� ���������� ������
    //  blocks  - ������� ����������� ����� (����������� ���� ����� �������� � �����)
    //  header  - ��������� �������� �����
    //  body    - �����, �� �������� ����� ������� ���������� ���� �����
    virtual Block* create(const Blocks &blocks, nbBlockHeader &header) const;
    //������������ ���������� ������
    virtual void   free(Block* block) const;
  };

  Container(const Container::Factory &factory);
  //����� ������ ����������, ������� ��������� ��� ����������
  virtual int32_t size() const;
  //����� ������ ����������
  virtual int32_t count() const;
  //�������� ������� ������
  bool isEmpty();
  //�������� ������� ��������� �����������
  bool isComposite();
  //������������
  void free();

  //����������� ����������
  bool copy(const Container &bc);
  //����������� ���������� ����������, ������� � ��������� �������
  //  ibegin - [��] ������� ������ ����������
  bool copyNested(const Container &bc, int32_t ibegin);

  //���������� ����� � ����� ����������
  Container& push(const Block &block);
  //���������� ������ ���������� � ����� ��������
  Container& push(const Container &bc, int32_t ibegin=0, int32_t iend=0);
  //�������� ��������� ������
  Container& pop(int32_t count);


  //�������� ������ � ����� ���������� �� �������
  Block* at(int32_t pos) const;
  inline Block* first() const { return _blocks[0]; }
  inline Block* last() const { return _blocks[_blocks.count()-1]; }
  const Blocks& all() const;
  //����� ������� ����� ���������� ����������
  //  pos - [��] �������, � ������� ������� ����� ��������� ����������
  //����������: ����� ����� ���������,
  //            count() � ������ ������
  int32_t findBegin(int32_t pos) const;
  //����� ���������� ����� ���������� ����������, ��������� ������� startPos
  //  ibegin - [��] ���� ��������� ����������, ��� �������� ������� ����� ���������� �����
  //                � ������ ��������� �����������
  //����������: ����� ���������� ����� ���������� ���������� + 1
  //            ibegin � ������ ������
  int32_t findEnd(int32_t ibegin) const;

  const Factory& factory() const;

  //������ �� ������
  int32_t read(QDataStream &from, int32_t count = -1);
  //���������� � �����
  int32_t write(QDataStream &to, bool asData = false);

protected:
  Blocks _blocks;
  Factory _factory;
};


#if !defined(QT_NO_DEBUG_STREAM)
  QDebug operator <<(QDebug  dbg, const Block &block);
  QDebug operator <<(QDebug  dbg, const Blocks &blocks);
  QDebug operator <<(QDebug  dbg, const Container &container);
#endif

}//namespace Nb

#pragma pack()
#endif //_NB_CONTAINER_H

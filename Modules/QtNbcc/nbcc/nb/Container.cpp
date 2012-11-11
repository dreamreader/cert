//////////////////////////////////////////////////////////////////////////
//����:
//������:
//����:
//�������:
//�����:
//
//����:
//
//
//
//////////////////////////////////////////////////////////////////////////
#include "Container.h"

namespace Nb {

  //...........................................................................
  //����
  //...........................................................................

  Block::Block(){
    _h.sizeHi = 0;
    _h.sizeLo = 0;
    _h.blockType = Nb::BtNbc; //��� �� ��������� ������ ���� ������������� � ������ �� �������
  }

  Block::Block(BlockType bt){
    _h.sizeHi = 0;
    _h.sizeLo = 0;
    _h.blockType = bt;
  }

  Block::~Block(){
    free();
  }

  //������ ������ �����
  int32_t Block::size() const {
    return _data.size() + sizeof(_h);
  }

  //������ ������ �����
  int32_t Block::sizeOfData() const {
    return _data.size();
  }

  //��� �����
  BlockType Block::type() const {
    return BlockType(_h.blockType);
  }

  //��������� ���� �����
  void Block::type(BlockType bt){
    _h.blockType = bt;
  }

  //������ � ������
  uint8_t* Block::data() const {
    return (uint8_t*)_data.data();
  }

  //������ � ������ �� ��������
  uint8_t* Block::at(int32_t pos) const {
    return (uint8_t*)(_data.data() + pos);
  }

  //�������� ������� ������
  bool Block::isEmpty() const{
    return _data.isEmpty();
  }

  //�������� ��������������� ����
  bool Block::isSupportedType(BlockType type) const{
    return true;
  }

  //�������� ������������� �����
  bool Block::isHeader() const {
    return false;
  }

  BlockType Block::getDefaultType() const {
    return BtNbc;
  }

  //�������� ���������� ����� �������� ����
  bool Block::isValid() const {
    if (getHSize() != _data.size())
      return false;
    return true;
  }

  //������ � ������
  int32_t Block::read(nbBlockHeader header, QDataStream &from){
    int32_t r=0;
    free();
    //��������� ��������� � ������� ������
    _h = header;
    _data.resize(getHSize());
    r += from.readRawData((char*)_data.data(),getHSize());
    if (from.status() != QDataStream::Ok) return -1;
    if (isValid() && isSupportedType(_h.blockType)) return r;
    return -1;
  }

  int32_t Block::read(QDataStream &from){
    int32_t r=0,ri=0;
    free();
    r += from.readRawData((char*)&_h, sizeof(nbBlockHeader));
    if (from.status() != QDataStream::Ok) return -1;
    if (-1 == (ri = read(_h,from))) return -1;
    return r+ri;
  }

  int32_t Block::write(QDataStream &to) const{
    if (to.status() != QDataStream::Ok) return false;
    //�������� ��������� � ����
    to.writeRawData((char*)&_h,sizeof(nbBlockHeader));
    to.writeRawData((char*)_data.data(),getHSize());
    if (to.status() != QDataStream::Ok) return false;
    return size();
  }

  //���������� ������ � ����� ����� (���� ������������� � �������)
  //����������: ��������� ������������ ��������� ������ push(A).push(B).push(C)
  Block& Block::push(const uint8_t *data, int32_t size){
    _data.append((const char*)data,size);
    setHSize(_data.size());
    return *this;
  }

  Block& Block::push(int8_t data){
    return push((uint8_t*)&data, sizeof(uint8_t));
  }

  Block& Block::push(int16_t data){
    return push((uint8_t*)&data, sizeof(uint16_t));
  }

  Block& Block::push(int32_t data){
    return push((uint8_t*)&data, sizeof(uint32_t));
  }

  Block& Block::push(const Uuid &data){
    return push((uint8_t*)&data, sizeof(Uuid));
  }

  Block& Block::push(const Data &data, bool withSize){
    QDataStream to(&_data,QIODevice::Append);
    data.write(to,withSize);
    setHSize(_data.size());
    return *this;
  }

  Block& Block::push(const Matrix &data){
    QDataStream to(&_data,QIODevice::Append);
    data.write(to);
    setHSize(_data.size());
    return *this;
  }

  Block& Block::push(const Scheme &data){
    QDataStream to(&_data,QIODevice::Append);
    data.writeScheme(to);
    setHSize(_data.size());
    return *this;
  }

  //������ � ��������� �������
  //  pos - [��/���] �������, � ������� ���������� ������
  //����������:
  //  � ����� �������� ���������� �� ����� ��������� ����,
  //  ������� �������� ���������������� ������ pop(x,A).pop(x,B).pop(x,C)
  const Block& Block::pop(int32_t &pos, uint8_t* data, int32_t size) const{
    //��������� ������������ ������� � ������������� �����
    if (pos < 0) { ELOG(1); return *this;}
    if ((pos + size) > sizeOfData()){ pos = -1; ELOG(2); return *this; }
    //������� ������ � �����
    uint8_t* ptr = (uint8_t*)_data.data() + pos;
    for (int32_t i=0; i < size;++i){
      *(data + i) = *(ptr + i);
    }
    //�������� �������
    pos += size;
    return *this;
  }

  const Block& Block::pop(int32_t &pos, int8_t &data) const{
    return pop(pos,(uint8_t*)&data,sizeof(int8_t));
  }

  const Block& Block::pop(int32_t &pos, int16_t &data) const{
    return pop(pos,(uint8_t*)&data,sizeof(int16_t));
  }

  const Block& Block::pop(int32_t &pos, int32_t &data) const{
    return pop(pos,(uint8_t*)&data,sizeof(int32_t));
  }

  const Block& Block::pop(int32_t &pos, Uuid &data) const {
    return pop(pos,(uint8_t*)&data,sizeof(Uuid));
  }

  //������� ���� ������
  const Block& Block::pop(int32_t &pos, Data &data, bool withSize) const {
    if (pos < 0 || pos > sizeOfData()) { pos = -1; return *this; }

    QDataStream from((QByteArray*)&_data,QIODevice::ReadOnly);
    from.skipRawData(pos);

    int32_t r = data.read(from,withSize,sizeOfData()-pos);
    if (r < 0) pos = -1;
    else       pos += r;
    return *this;
  }

  const Block& Block::pop(int32_t &pos, Matrix &data) const {
    if (pos < 0 || pos > sizeOfData()) { pos = -1; return *this; }

    QDataStream from((QByteArray*)&_data,QIODevice::ReadOnly);
    from.skipRawData(pos);

    int32_t r = data.read(from);
    if (r < 0) pos = -1;
    else       pos += r;
    return *this;
  }

  const Block& Block::pop(int32_t &pos, Scheme &data) const{
    if (pos < 0 || pos > sizeOfData()) { pos = -1; return *this; }

    QDataStream from((QByteArray*)&_data,QIODevice::ReadOnly);
    from.skipRawData(pos);

    int32_t r = data.readScheme(from,sizeOfData()-pos);
    if (r < 0) pos = -1;
    else       pos += r;
    return *this;
  }

  //������� ���������� ����� ���� ��� ������
  const Block& Block::skip(int32_t &pos, int32_t plus) const {
    pos += plus;
    return *this;
  }


  //������������ �����
  void Block::free(){
    _data.clear();
    setHSize(0);
    _h.blockType = getDefaultType();
  }

  //����������� ������
  bool Block::copy(const Block &block){
    if (!isSupportedType(block.type()))
      return false;
    _h    = block._h;
    _data = block._data;
    return true;
  }

  //������������ ����� (� ������ ������ �����)
  Block* Block::dup() const {
    Block* block = new Block;
    block->copy(*this);
    return block;
  }

  //������ � ������ ���� ������� ����� ���������
  void      Block::setHSize(uint32_t size){
    (*(uint32_t*)&_h) = ((*(uint32_t*)&_h) & 0xFF000000) | size;
  }

  uint32_t  Block::getHSize() const {
    return (*(uint32_t*)&_h) & 0x00FFFFFF;
  }

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug Block::debug(QDebug dbg) const {
    QDebug d = dbg.nospace();
    d<<"Block("<<type()<<" "<<size();
    if (sizeOfData()){
      d<<" ["<<AsHex(data(),8,' ',sizeOfData())<<"]";
    } else
      d<<" [Nil]";
    return d<<")";
  }

  QDebug BlockContHead::debug(QDebug  dbg) const{
    QDebug d = dbg.nospace();
    return d<<"Block("<<type()<<" "<<size()<< " "<< sizeOfBlocks() << " " << containerType() << ")";
  }
#endif
  //.............................................................................



  BlockContHead::BlockContHead(){
    type(BtNbc);
  }



  BlockContHead::BlockContHead(const Uuid &ct, int32_t csize){
    type(BtNbc);
    init(ct,csize);
  }


  //�������� ������������� �����
  bool BlockContHead::isHeader() const {
    return (type() == BtNbc);
  }

  //���������� ���������
  //  blocks  - [��] ������ ������
  //  ibegin  - [��] ������� ������� ����� ���������� (����� ����� ���������)
  //  iend    - [��] ������� ���������� ����� ���������� + 1
  bool BlockContHead::update(const Blocks &blocks, int32_t ibegin, int32_t iend){
    if (_data.isEmpty()) return false;
    if (iend > blocks.count() || ibegin >= blocks.count()) return false;

    nbNbcBody_t *body = (nbNbcBody_t*)_data.data();
    body->size = 0;
    for (int32_t i=ibegin; i<iend; ++i)
      body->size += blocks.at(i)->size();
    return true;
  }

  //����� ����� �����
  //  blocks  - [��] ������ ������ ����������
  //  ibegin  - [��] ������� ������� ����� ����������
  //����������: ����� ���������� ����� ���������� + 1
  //            ibegin � ������ ������
  int32_t BlockContHead::findEnd(const Blocks &blocks, int32_t ibegin) const{
    int32_t iend = ibegin;
    int32_t fullSize = sizeOfBlocks();
    while (fullSize>0){
      if (iend >= blocks.count()) break;
      fullSize -= blocks.at(iend)->size();
      iend++;
    }
    return iend;
  }

  //��������� �������� �������� ���� ������� ���������� (���� ��������������)
  int32_t BlockContHead::sizeOfBlocks() const {
    if (_data.isEmpty()) return 0;
    return (int32_t)((nbNbcBody_t*)_data.data())->size;
  }
  //��������� �������� �������� ����� ����������� (���� ��������������)
  int32_t BlockContHead::countOfBlocks() const {
    //����������� ����� ������ �� ��������������
    return 0; 
  }

  //������ ���� ����������
  Uuid BlockContHead::containerType() const {
    if (_data.isEmpty()) return Uuid();
    return Uuid(((nbNbcBody_t*)_data.data())->nbcType);
  }


  void BlockContHead::init(const Uuid &ct, int32_t csize){
    if (!_data.isEmpty()) free();
    push(csize);
    push((uint8_t*)&ct,sizeof(nbUuid));
  }

  //������������ ����� (� ������ ������ �����)
  Block* BlockContHead::dup() const {
    BlockContHead* block = new BlockContHead(containerType(),sizeOfBlocks());
    block->copy(*this);
    return block;
  }

  //.............................................................................

  //�������� ���������� ������
  //  blocks  - ������� ����������� ����� (����������� ���� ����� �������� � �����)
  //  header  - ��������� �������� �����
  //  body    - �����, �� �������� ����� ������� ���������� ���� �����
  Block* Container::Factory::create(const Blocks &blocks, nbBlockHeader &header) const {
    //� ����������� �� ���� ����� ������� ��������������� ���
    BlockType bt = BlockType(header.blockType);
    Block *block = nbNULL;
    switch (bt){
    case BtNbc:
      block = new BlockContHead(); break;
    default:
      block = new Block();
    }
    return block;
  }

  //������������ ������ ���������� ������
  void Container::Factory::free(Block* block) const {
    if (block) delete block;
  }

  //.............................................................................

  Container::Container(const Container::Factory &factory):_factory(factory){
  }

  const Container::Factory& Container::factory() const {
    return _factory;
  }

  //����� ������ ����������, ������� ���������
  //(����������� �����������)
  int32_t Container::size() const {
    int32_t fullSize = 0;
    for (int32_t i=0; i<_blocks.size(); ++i)
      fullSize += _blocks.at(i)->size();
    return fullSize;
  }

  //����� ������ ����������
  //(����������� ���������� �� ������ �������� ����� ������)
  int32_t Container::count() const {
    return _blocks.size();
  }

  //�������� ������� ������
  bool Container::isEmpty(){
    return _blocks.empty();
  }

  //�������� ������� ��������� �����������
  bool Container::isComposite(){
    //���� ������ ��� ��������� ���� �������� ����������,
    //��������� ����� ���������
    for (int32_t i=1; i<_blocks.size(); ++i){
      if (_blocks.at(i)->isHeader()) return true;
    }
    return false;
  }

  //������������
  void Container::free(){
    for (int32_t i=0; i<_blocks.size(); ++i)
      _factory.free(_blocks.at(i));
    _blocks.clear();
  }

  //����������� ����������
  bool Container::copy(const Container &bc){
    free();
    push(bc);
    return true;
  }

  //����������� ���������� ����������, ������� � ��������� �������
  //  ibegin - [��] ������� ������ ����������
  bool Container::copyNested(const Container &bc, int32_t ibegin){
    //�������� ������� �����
    if (ibegin != bc.findBegin(ibegin)) return false;
    //�������� ���������� �����
    int32_t iend = bc.findEnd(ibegin);
    if (iend == ibegin) return false;

    free();
    for (int32_t i=ibegin; i<iend; ++i)
      push(*bc.at(i));
    return true;
  }

  //���������� �����
  Container& Container::push(const Block &block){
    _blocks.push_back(block.dup());
    return *this;
  }

  //���������� ����������
  Container& Container::push(const Container &bc, int32_t ibegin, int32_t iend){
    //if (ibegin <= 0    || ibegin >= bc.count())  ibegin = 0;
    //if (iend <= ibegin || iend>bc.count())       iend = bc.count();

    for (int32_t i=/*ibegin*/0; i<bc.count()/*iend*/; ++i)
      _blocks.push_back(bc.at(i)->dup());
    return *this;
  }

  //�������� ��������� ������
  Container& Container::pop(int32_t count){
    while (count>0 && _blocks.count()){
      delete _blocks.last();
      _blocks.pop_back();
      count--;
    }
    return *this;
  }

  //�������� ������ � ����� ���������� �� �������
  Block* Container::at(int32_t pos) const {
    return _blocks.at(pos);
  }

  //��������� ������� � ������ �����������
  const Blocks& Container::all() const {
    return _blocks;
  }

  //����� ������� ����� ���������� ����������
  //  pos - [��] �������, � ������� ������� ����� ��������� ����������
  //����������: count(), ���� �� ������
  int32_t Container::findBegin(int32_t pos) const {
    for (int32_t i=pos; i<_blocks.size(); ++i){
      if (_blocks.at(i)->isHeader()) return i;
    }
    return _blocks.size();
  }


  //����� ���������� ����� ���������� ����������, ��������� ������� startPos
  //  ibegin - [��] ���� ��������� ����������, ��� �������� ������� ����� ���������� �����
  //                � ������ ��������� �����������
  //����������: ����� ���������� ����� ���������� ���������� + 1
  //            ibegin � ������ ������
  int32_t Container::findEnd(int32_t ibegin) const {
    //����������� ����� ����� � ������������ � ������� ���������
    Block* block = _blocks.at(ibegin);
    if (!block) { ELOG(1); return ibegin; }
    if (!block->isHeader()) {ELOG(2); return ibegin; }
    return ((BlockHead*)block)->findEnd(_blocks,ibegin);
  }



  int32_t Container::read(QDataStream &from, int32_t count){
    int32_t r=0;
    free();
    int32_t size=0;
    nbBlockHeader bh;
    //������� ������ ���� ����������, ������� ����� ������������������ ��� ���� ���������
    if (count<=0){
      //������� ���������
      r += from.readRawData((char*)&bh,sizeof(nbBlockHeader));

      if (from.status() != QDataStream::Ok) return -1;
      //������� ����
      BlockHead *head = (BlockHead*)_factory.create(_blocks,bh);
      if (!head || !head->isHeader()) {
        _factory.free(head);
        return -1;
      }
      //������� ���� �����
      r += head->read(bh, from);                  //UPD: read(_bh_, from)
      if (from.status() != QDataStream::Ok) return -1;

      //� ����������� �� ����������, ������� ��������� ����� ������ ���
      //��������� ����� �������� ������ �������
      count = head->countOfBlocks();
      size  = (int32_t)head->sizeOfBlocks();
      //�������� ���� ��������� � ���������
      _blocks.push_back(head);
      count -= 1;
      size -= (int32_t)head->size();
    }

    //������� �����
    if (count>0){ //�� �����                      //UPD: >0
      while (count){
        from.readRawData((char*)&bh,sizeof(nbBlockHeader));
        Block *block = _factory.create(_blocks,bh);
        if (!block) return -1;
        //������� ���� �����
        r += block->read(bh, from);               //UPD: read(_bh_, from)
        if (from.status() != QDataStream::Ok) return -1;

        _blocks.push_back(block);  
        count--;
      }
      return r;
    } else if (size){ //�� ������ �������
      while (size>0){
        from.readRawData((char*)&bh,sizeof(nbBlockHeader));
        Block *block = _factory.create(_blocks,bh);
        if (!block) return -1;
        //������� ���� �����
        r += block->read(bh, from);             //UPD: read(_bh_, from)
        if (from.status() != QDataStream::Ok) return -1;

        _blocks.push_back(block);  
        size -= block->size();
      }
      return r;
    } else {
      return -1;
    }
  }



  int32_t Container::write(QDataStream &to, bool asData){
    int32_t w=0;
    if (asData) {
      int32_t sz = size();
      if (-1 == to.writeRawData((char*)&sz,sizeof(int32_t))) return -1;
    }

    for (int32_t i=0; i<_blocks.size(); ++i){
      w += _blocks.at(i)->write(to);
      if (to.status() != QDataStream::Ok) return -1;
    }
    return w;
  }

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug operator <<(QDebug  dbg, const Block &block){
    return block.debug(dbg);
  }

  QDebug operator <<(QDebug  dbg, const Blocks &blocks){
    QDebug d = dbg.nospace();
    d<<"Blocks(\n";
    for (int i=0; i<blocks.count(); ++i){
      if (blocks[i])  d<<"\t"<<i<<":"<<*blocks[i]<<"\n";
      else            d<<"\t"<<i<<":Nil\n";
    }
    return d<<")";
  }

  QDebug operator <<(QDebug  dbg, const Container &container){
    return dbg.nospace()<<"Container("<<container.all()<<")";
  }

#endif


} //end namespace Nb

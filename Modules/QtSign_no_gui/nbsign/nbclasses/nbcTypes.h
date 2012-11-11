/****************************************************************************
** ����:        nbcTypes.h
** ������:      1.0
** ����:
** �������:
** �����:       ������� �.�.
**
** ��������:    ������ ��� �������� ����� ������ ����.
****************************************************************************/

#ifndef _NBC_TYPES_H
#define _NBC_TYPES_H

#include <qstring.h>
#include <qvector.h>

#include "..\nb\nb.h"

#pragma pack(1)

using namespace std;

// ���� ����� ������������ (���� �� ���������)
typedef struct nbCrcBody_t {
  int16_t   zone;     // ����� ��������������� ������ � �� ������������ ������������ ����� ������������
  uint16_t  owner;    // ����������� = 0
  uint16_t  format;   // ������ = 0
  uint8_t   crc[16];  // ����������� �����
} nbCrcBody;

//...........................................................................
//  ���������� ������������� �������������
//...........................................................................

bool operator ==(const nbUuid &id1, const nbUuid &id2);
bool operator !=(const nbUuid &id1, const nbUuid &id2);

class NbUuid:public nbUuid {
public:
  NbUuid();
  NbUuid(const nbUuid &id);
  
  // ��������� ��������������, ���������� � �������������� �������
  bool generate();
  // ��������� �������� ��������������
  void erase();
  // �������� �� ������� ��������
  bool nil() const;

  // �������������� � ������
  QString toString() const;
  // �������������� �� ������
  bool fromString(const QString &s);
};

// ������ ���������������
typedef QVector<NbUuid> NbUuids;

//...........................................................................
//  ���� ������
//...........................................................................
class NbData {
public:
  // �����������
  NbData();
  NbData(uint32_t size);
  NbData(const nbData data, bool isAttached=true);
  NbData(const NbData &source);
  NbData(const QString &s);
  // ����������
  virtual ~NbData();

  operator nbData() const { return _data; }
  
  NbData& operator =(const NbData &source) { copyFrom((nbData)source); return *this; }

  bool operator==(const NbData &d) const { return (0==compare(d)); }
  bool operator!=(const NbData &d) const { return (0!=compare(d)); }
      
  // ������������� �����
  //  - ������ ������������ resize ��� ��������������� �����
  void attach(nbData data);
  // ������������ �����
  nbData detach();
  
  // ����������� ����� ������ �� nbData
  void copyFrom(const nbData data);
  // ����������� ����� ������ �� ������� ������������ �����
  void copyFrom(const void* data, uint32_t size);
  // ����������� ����� ������ � ������
  bool copyTo(void* data, uint32_t size) const;
 
  bool empty() const;
  // ������������ ����� ������
  void free();
  // ������������ ����� ������, ���� ���� �� �����������
  // (��������� ��������, ���� �������������� ���� ��� ���������� �� nbData �������� detach)
  void freeForced();
  
  // ��������� ������� �����
  //  - ��� �������������� ������ ������ ����� ���������� ������ � ������� ����������
  bool resize(uint32_t size);
  // ��������� � ���� "������" ������������ ������� ����� ��� ������������ ��������� �������
  void setWishfulSize(uint32_t size);
  
  // ������ �����
  uint32_t size() const;

  // �������������� ������, �������������� ������ utf-8, � ������ utf-16
  QString toString() const;

  // �������������� ������ �� ������ utf-16 � ������ utf-8
  bool fromString(const QString &s);
  // ����������� ������� ����� ������ � �������� wchar
  uint32_t getStringLength() const;
  
  // ��������� ��������� ������� ������
  uint32_t getWishfulSize() const;
  // �������
  uint32_t capacity() const { return _capacity; }
  // ��������� �������
  bool reserve(uint32_t capacity);

  // ��������� (0 (c==d), >0 (c>d), <0 (c<d))
  int32_t compare(const NbData &d) const;

protected:

  // ��������� ��������� � ��������� �������
  void setDataTerminal(uint32_t pos){ if (_data) { _data[pos] = 0; _data[pos+1] = 0; } }

  nbData    _data;        // ������������� ������
  uint32_t  _capacity;    // ������� ������ � ������ (��� ����� ����� ������ � ������������ �����)
  bool      _isAttached;
};

//.............................................................................
//  �������������� ���
//.............................................................................
class NbMeta: public nbMeta {
public:
  NbMeta();
  NbMeta(const nbMeta &mi);
  NbMeta(uint32_t count, nbMetaFormat format, nbMetaType type);

  bool operator==(const NbMeta &mi) const { return equal(mi); }
  bool operator!=(const NbMeta &mi) const { return !equal(mi); }
  //�������� �� ���������
  bool equal(const NbMeta &mi) const;
  //��������, �������� �� ������������ ������������� range
  bool subrange(const NbMeta &range) const;
  //�������� �������� �� ������������ ��������
  bool ranged() const;
  //������������ ���������� �� ����
  bool anyType() const;
  //������������ ���������� �� ����� ���������
  bool anyCount() const;
  //������������ ���������� �� �������
  bool anyFormat() const;
  //�������� �� �������������� ��������
  bool any() const;
  
  static const NbMeta nil();
  
  //����� �������� ��� � �����
  uint32_t sizeItem() const;
  //����� ��� � ������ � ������������� �� �����
  uint32_t size() const;
};  

typedef QVector<NbMeta> NbMetas;

//.............................................................................
struct NbBlockHeader: public nbBlockHeader {
  NbBlockHeader(nbBlockHeader bh);
  uint32_t    bodySize() const;
  void        bodySize(uint32_t size);
  nbBlockType type() const;
  void        type(nbBlockType bt);
};


//.............................................................................
//  ���
//  - ��������� ���������������� ��������� �� ���� ������ ��� ������ ���������
//    ��������� ����
//  - ��������� �������� � ���, ��������������� � ���� ������ ���
//.............................................................................
union Vbp {
public:
  //������ ������ � ��������� ����� �������������� ���������
  nbVbp     v;
  int8_t    *i8;
  int16_t   *i16;
  int32_t   *i32;
  int64_t   *i64;
  uint8_t   *u8;
  uint16_t  *u16;
  uint32_t  *u32;
  uint64_t  *u64;
  float     *r32;
  double    *r64;
public:
  Vbp():v(nbNULL){}
  Vbp(const nbVbp &vec):v(vec){}
  inline bool create(uint32_t size) { free(); u8 = new uint8_t[size]; if (!u8) return false; }
  inline void free(){ if (u8) { delete[] u8; u8 = nbNULL; }}

  operator nbVbp() const { return v; }
  Vbp& operator =(const nbVbp& vec){ v=vec; return *this; }
  bool empty(){ return (v==nbNULL); }
  static Vbp null(){ static Vbp vec(nbNULL); return vec; }
  
  Vbp shift(int32_t size) const { return (Vbp)((uint8_t*)v + size); }
  //������� ������ � ������ ��� ������ � ���������� �� 1,2,4 ���,
  //������������������� ��� �����������

  uint32_t u1(uint32_t pos) const;
  void     u1(uint32_t pos, uint32_t val);
  uint32_t u2(uint32_t pos) const;
  void     u2(uint32_t pos, uint32_t val);
  uint32_t u4(uint32_t pos) const;
  void     u4(uint32_t pos, uint32_t val);

  //������� ������ � ������ ��� ������ � ���������� �� 1,2,4 ���,
  //������������������� ��� ��������

  int32_t  i1(uint32_t pos) const;          //���������� -1 � 1
  void     i1(uint32_t pos, int32_t val);   //������������� >0 ��� <=0

  //������� �������������� �������� �������� �������������

  void convU1fromR32(const Vbp &vec, uint32_t count);
  void convR32fromU1(const Vbp &vec, uint32_t count);
  void convI1fromR32(const Vbp &vec, uint32_t count);
  void convR32fromI1(const Vbp &vec, uint32_t count);
  void convU16fromR32(const Vbp &vec, uint32_t count);
  void convR32fromU16(const Vbp &vec, uint32_t count);
  void convI16fromR32(const Vbp &vec, uint32_t count);
  void convR32fromI16(const Vbp &vec, uint32_t count);
  void convI32fromR32(const Vbp &vec, uint32_t count);
  void convR32fromI32(const Vbp &vec, uint32_t count);
  void convU32fromR32(const Vbp &vec, uint32_t count);
  void convR32fromU32(const Vbp &vec, uint32_t count);

  bool conv(const Vbp &vec, uint32_t count, nbMetaFormat mfTo, nbMetaFormat mfFrom);
  void copy(uint8_t *data, uint32_t size);
};

//...........................................................................
//  ������� ���
//...........................................................................
class NbMatrix: public nbMatrix {
public:
  //������ �� ������ �������
  class Row {
  public:
    Row():
      _ref(nbNULL),_irow(0),_handle(nbNULL){}
    Row(const Row &row):
      _ref(row._ref),_irow(row._irow),_handle(row._handle){}
    Row(const nbMatrix *m, uint32_t irow, nbHandle handle=nbNULL):
      _ref(m),_irow(irow),_handle(handle){}
    //����������� � ���������� ���������� ������ (������ ������ �������������)
    Row(uint32_t ncols, NbMeta meta):_ref(nbNULL),_irow(0),_handle(nbNULL){
      NbMatrix *m = new NbMatrix(ncols,meta);
      _ref  = m; _irow = 0; _handle = (nbHandle)m;
    }
    //��������� ��������, ���� ��� �������� (�.�. ������� ���������� ������� NbMatrix)
    bool resize(uint32_t ncols, NbMeta meta);
    //������������ ��������� �������
    void free();

    Vbp operator [](uint32_t icol) const { return (Vbp)_ref->data[_irow][icol]; }
    Vbp& operator [](uint32_t icol) { return (Vbp&)_ref->data[_irow][icol]; }
    Vbp* operator*() const { return (Vbp*)_ref->data[_irow]; }
    bool empty() const { return (!_ref); }

    Vbp    at(uint32_t icol) const { return ((Vbp**)_ref->data)[_irow][icol]; }
    Vbp&   at(uint32_t icol) { return ((Vbp**)_ref->data)[_irow][icol]; }

    Vbp* data() const { return (Vbp*)_ref->data[_irow]; }
    NbMeta meta() const { return (NbMeta)_ref->meta[_irow]; }
    uint32_t count() const { return _ref->ncols; }
    const nbMatrix* ref() const { return _ref; }

    nbHandle  handle() const { return _handle; }
    void clear(){ _ref=nbNULL; _irow=0; _handle=nbNULL; }
    
    bool copy(const Row &row);
    bool convert(const Row &row);

    //���������� ������� ������
    void tideup();

  protected:
    const nbMatrix *_ref;     // ������ �� �������
    uint32_t  _irow;          // ����� ������
    nbHandle  _handle;  // ��������������� ��������� (�������� ���������)
  };

public:
  
  NbMatrix();
  //����������� ������ �������
  NbMatrix(uint32_t nrows, uint32_t ncols, const NbMeta *meta);
  //����������� ��� �������
  NbMatrix(uint32_t ncols, const NbMeta meta);
  //����������� � ������������ �������
  NbMatrix(const NbMatrix &m, nbMetaFormat f);
  //����������� � ������������ �������
  NbMatrix(const NbMatrix::Row &r, nbMetaFormat f);

  ~NbMatrix();

  Vbp* operator[](uint32_t irow) const { return (Vbp*)nbMatrix::data[irow]; }

  Row    row(uint32_t irow) const;
  Vbp    at(uint32_t irow, uint32_t icol) const;
  Vbp&   at(uint32_t irow, uint32_t icol);

  Vbp** data() const;
  uint32_t ncols() const;
  uint32_t nrows() const;
  NbMeta* meta() const;
  NbMeta  meta(uint32_t irow) const;
  //������������ �������
  uint32_t capacity() const;

  bool valid() const;    //�������� ��������� ���� ������� (nrows < 65536)
  bool empty() const;    //�������� �������
  bool pattern() const;  //�������� ������� �������
  bool full() const;     //�������� �� ������ �������

  //��������� ������ ��� ������������
  bool resizeMeta(uint32_t nrows, const NbMeta *meta);
  //��������� ������ ��� ������
  bool resizeData(uint32_t ncols);
  //������������� ������ ��� ������ ��� ��������� ���������� 
  //(������ ��� ���������� � ������ ������ ���� �������� �����)
  bool resizeVbp(uint32_t irow, NbMeta meta);
  //������������ ������� (���������� freeMeta)
  void free();
  //������������ ������������ (� ������)
  void freeMeta();
  //������������ ������
  void freeData();

  bool copyVbp(uint32_t irow, const Row& v); //����������� �� ������ �� ������ (������ �� ��������������)
  bool copyVbp(const nbMatrix &m);           //����������� ������� (������ �� ��������������)

  bool copy(const nbMatrix &m);  //  ����������� �������

  //����������� ������� �� ������ ������� � ������
  //(����� �������� � ����� ��������� ������� ������ ���������)
  bool convert(const NbMatrix &m);

protected:
  void freeVbp(uint32_t irow);
private:
  uint32_t _capacity;
};

//...........................................................................
//  ������������� �����
//...........................................................................
class NbSlotID {
public:
  NbSlotID():_id(nbSLOT_OUT){}
  NbSlotID(nbSlotID id):_id(id){}
  operator nbSlotID() const { return _id; }

  uint16_t    type() const   { return _id & nbSLOT_MASK_TYPE; }
  uint16_t    index() const  { return _id & nbSLOT_MASK_INDEX; }
  uint8_t     bindex() const { return (uint8_t)(_id & nbSLOT_MASK_INDEX);}
  nbBlockType btype() const  { return (nbBlockType)((_id & nbSLOT_MASK_BTYPE)>>7); }
  //������� ������������ ������
  NbSlotID inline static in(uint16_t i) { return NbSlotID(nbSLOT_IN(i)); }
  NbSlotID inline static conv(nbBlockType bt, uint8_t i) { return NbSlotID(nbSLOT_CONV(bt,i)); }
  NbSlotID inline static convDef(nbBlockType bt){ return NbSlotID(nbSLOT_CONV_DEFAULT(bt)); }
  NbSlotID inline static specIn(uint16_t i){ return NbSlotID(nbSLOT_SPEC_IN(i)); }
  NbSlotID inline static specOut(uint16_t i){ return NbSlotID(nbSLOT_SPEC_OUT(i)); }
  static const NbSlotID out;

  //������� ��������� ����
  NbSlotID inline static retype(NbSlotID id, uint16_t st){ return NbSlotID(st | id.index()); }
protected:
  nbSlotID _id;
};

typedef QVector<NbSlotID> NbSlotIDs;

#pragma pack()
#endif

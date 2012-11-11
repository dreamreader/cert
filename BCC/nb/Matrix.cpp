//////////////////////////////////////////////////////////////////////////
//файл:
//версия:
//дата:
//изменен:
//автор:
//
//цель:
//
//
//
//////////////////////////////////////////////////////////////////////////
#include <QtCore/QVector>
#include "Matrix.h"

namespace Nb {

  #define nbROW_GUID   (rows > 0x0000ffff)

  //-------------------------------------------------------------------------------------
  //Matrix::Row
  //-------------------------------------------------------------------------------------

  Matrix::Row::Row():_m(Null),_irow(0){}



  Matrix::Row::Row(const Row &row):_m(row._m),_irow(row._irow){
  }



  Matrix::Row::Row(Matrix &m, int32_t irow):_m(&m),_irow(irow){
  }



  Matrix::Row::Row(const nbMatrix *m, int32_t irow):_m((Matrix*)m),_irow(irow){
    //Сформировать ссылку, запретив изменять размер и метаописание
    _irow |= 0x80000000;
  }


  Vbp*    Matrix::Row::operator*() const {
    if (!_m || !_m->data()) return Null;
    return _m->data()[irow()];
  }

  Vbp* Matrix::Row::data() const {
    if (!_m || !_m->data()) return Null;
    return _m->data()[irow()];
  }



  Meta Matrix::Row::meta() const {
    if (!_m) return Meta();
    return _m->meta(irow());
  }



  int32_t Matrix::Row::count() const {
    if (!_m) return 0;
    return _m->ncols();
  }



  Vbp    Matrix::Row::at(int32_t icol) const {
    if (!_m || !_m->data()) return Null;
    return _m->data()[irow()][icol];
  }



  Vbp&   Matrix::Row::at(int32_t icol) {
    return _m->data()[irow()][icol];
  }



  Vbp Matrix::Row::operator [](int32_t icol) const {
    if (!_m || !_m->data()) return Null;
    return _m->data()[irow()][icol];
  }



  Vbp& Matrix::Row::operator [](int32_t icol) {
    return _m->data()[irow()][icol];
  }



  Matrix::Row& Matrix::Row::operator=(const Row& row){
    _m = row._m;
    _irow = row._irow;
    return *this;
  }



  Matrix::Row Matrix::Row::ref() const {
    return Row((const nbMatrix*)_m,_irow);
  }


  bool Matrix::Row::isNull() const{
    return (_m==0);
  }


  bool Matrix::Row::isEmpty() const {
    if (!_m) return true;
    return _m->isEmpty();
  }



  bool Matrix::Row::isLocked() const {
    return _irow != irow();
  }



  bool Matrix::Row::resize(int32_t ncols){
    if (isLocked() || !_m) { return false; }
    return _m->resize(ncols);
  }



  bool Matrix::Row::resize(int32_t ncols, Meta meta){
    if (isLocked() || !_m) return false;
    return ( _m->remeta1(irow(),meta) && _m->resize(ncols));
  }


  //Определить параметры объединения rows
  //  umeta   [вых] - общее метаописание
  //  ucount  [вых] - общий размер
  bool Matrix::Row::metaForUnite(const Rows &rows, UniteType ut, int32_t &ucount, Meta &umeta){
    //Проверить нулевое число примеров
    if (rows.count() == 0) return false;
    //Рассчитать метаописание и число примеров
    Meta um = rows.at(0).meta();
    int32_t ucols = rows.at(0).count();

    if (um.sizeOfItem()*Meta::SizeOfByte != um.size()) return false;
    switch (ut){
    case UtByVbp: //объединение данных по примерам
      for (int32_t i=0; i<rows.count(); ++i){
        if (ucols != rows.at(i).count()) return false; //Проверить размер
        Meta cm = rows.at(i).meta();
        if (cm.sizeOfItem()*Meta::SizeOfByte != cm.size()) return false;
        if (um.format() != cm.format() || um.type() != cm.type()) return false; //Проверить тип и формат
        um.count(um.count() + cm.count());
      }
      break;
    case UtByIndex: //объединение примеров по номеру
    case UtByOrder: //объединение примеров по порядку
      for (int32_t i=0; i<rows.count(); ++i){
        if (um != rows.at(i).meta()) return false;
        ucols += rows.at(i).count();
      }
      break;
    default:
      return false;
    }
    umeta = um; ucount = ucols;
    return true;
  }



  void Matrix::Row::clear(){
    _m = Null;
    _irow = 0;
  }



  void Matrix::Row::tideUp(){
    Vbp* d = data();
    if (!d) return;
    int32_t sz = meta().size();
    for (int32_t icol=0; icol<count(); ++icol,++d)
      memset((*d).u8,0x00,sz);
  }



  bool Matrix::Row::copy(const Row &row){
    if (meta() != row.meta()) return false;
    return copyb(row,0,0,row.count(),0,0,row.meta().size());
  }


  //Копирование одного элемента
  bool Matrix::Row::copy1(const Row &row, int32_t ifrom, int32_t ito){
    Vbp src = row.at(ifrom);
    Vbp dst = at(ito);
    if (!src.isEmpty() || !dst.isEmpty()) return false;
    dst.copy(src,row.meta().size());
    return true;
  }

  bool Matrix::Row::copy(const Row &row, int32_t ifrom, int32_t ito, int32_t icount){
    if (meta() != row.meta()) return false;
    return copyb(row,ifrom,ito,icount,0,0,row.meta().size());
  }



  bool Matrix::Row::copyb(const Row &row, int32_t ifrom, int32_t ito, int32_t icount, int32_t sfrom, int32_t sto, int32_t ssize){
    //Проверить совпадение полей
    if (row.data() == data()) {
      if (ifrom == ito && sfrom == sto) return true;
    }
    //Проверить число примеров
    if (row.count()-ifrom > icount || count()-ito > icount) return false;
    //Проверить метаописание
    if (row.meta().size()+sfrom > ssize || meta().size()+sto > ssize) return false;
    if (meta().format() != row.meta().format()) return false;

    //Скопировать примеры со смещением
    Vbp* src = row.data();
    Vbp* dst = data();
    if (!src || !dst) return false;
    src += ifrom; dst += ito;

    for (int32_t icol=0; icol<icount; ++icol,++src, ++dst)
      if (*dst && *src)
        (*dst).shift(sto).copy((*src).shift(sfrom),ssize); //Скопировать со смещением полей

    return true;

  }



  bool Matrix::Row::convert(const Matrix::Row &row){
    //Выйти, если источник и приемник совпадают
    if (data() == row.data()) return true;
    //Проверить совпадение числа элементов и числа примеров в строке
    uint32_t ncols = count();
    uint32_t ecols = meta().count();
    if ((ncols != row.count()) || (ecols != row.meta().count())) return false;
    //Выполнить преобразование для каждого элемента каждого примера вектора
    //(для преобразований, который отличаются только интерпретацией, использовать копирование)
    MetaFormat from = row.meta().format();
    MetaFormat to   = meta().format();
    Vbp* src = row.data();
    Vbp* dst = data();
    switch (from){
      case MfU1:
        switch (to){
          case MfR32:  for (uint32_t j=0; j<ncols; ++j) dst[j].convR32fromU1(src[j],ecols); return true;
          default: break;
        }
        break;
      case MfI1:
        switch (to){
          case MfR32:  for (uint32_t j=0; j<ncols; ++j) dst[j].convR32fromI1(src[j],ecols); return true;
          default: break;
        }
        break;
      case MfR32:
        switch (to){
          case MfU1:   for (uint32_t j=0; j<ncols; ++j) dst[j].convU1fromR32(src[j],ecols); return true;
          case MfI1:   for (uint32_t j=0; j<ncols; ++j) dst[j].convI1fromR32(src[j],ecols); return true;
          case MfU16:  for (uint32_t j=0; j<ncols; ++j) dst[j].convU16fromR32(src[j],ecols); return true;
          case MfI16:  for (uint32_t j=0; j<ncols; ++j) dst[j].convI16fromR32(src[j],ecols); return true;
          case MfI32:  for (uint32_t j=0; j<ncols; ++j) dst[j].convI32fromR32(src[j],ecols); return true;
          case MfU32:  for (uint32_t j=0; j<ncols; ++j) dst[j].convU32fromR32(src[j],ecols); return true;
          default: break;
        }
        break;
      case MfU16:
        switch (to){
          case MfR32:  for (uint32_t j=0; j<ncols; ++j) dst[j].convR32fromU16(src[j],ecols); return true;
          default: break;
        }
        break;
      case MfI16:
        switch (to){
          case MfR32:  for (uint32_t j=0; j<ncols; ++j) dst[j].convR32fromI16(src[j],ecols); return true;
          default: break;
        }
        break;
      case MfI32:
        switch (to){
          case MfR32:  for (uint32_t j=0; j<ncols; ++j) dst[j].convR32fromI32(src[j],ecols); return true;
          default: break;
        }
        break;
      case MfU32:
        switch (to){
          case MfR32:  for (uint32_t j=0; j<ncols; ++j) dst[j].convR32fromU32(src[j],ecols); return true;
          default: break;
        }
        break;
      default:
        break;
    }
    //Скопировать, если форматы идентичны по размеру
    return copy(row);
  }


  bool Matrix::Row::unite(const Matrix::Rows &rows, Matrix::UniteType ut){
    uint32_t sz;
    uint32_t sh=0;
    int32_t rcols=0;
    Vbp* d  = data();
    Vbp* rd;
    switch (ut){
    case UtByVbp://([1||2||3])
      //Для всех частей выполнить
      for (int irow=0; irow<rows.count(); ++irow){
        d  = data();
        rd = rows.at(irow).data();
        sz = rows.at(irow).meta().size();
        rcols = rows.at(irow).count();
        //Для всех примеров выполнить
        for (int icol=0; icol<rcols; ++icol){
          (*d).shift(sh).copy((*rd).u8,sz);
          rd++; d++;
        }
        sh += sz;
      }
      return true;
    case UtByIndex://(123,123,123)
      if (!rows.count()) return true;
      sz    = rows.at(0).meta().size();
      rcols = rows.at(0).count();
      for (int icol=0; icol<rcols;++icol){
        for (int irow=0; irow<rows.count(); ++irow){
          rd = rows.at(irow).data() + icol;
          (*d).copy((*rd).u8,sz);
          rd++; d++;
        }
      }
      return true;
    case UtByOrder://(111,22,33333)
      if (!rows.count()) return true;
      sz = rows.at(0).meta().size();
      for (int irow=0; irow<rows.count(); ++irow){
        rd = rows.at(irow).data();
        rcols = rows.at(irow).count();
        for (int icol=0; icol<rcols;++icol){
          (*d).copy((*rd).u8,sz);
          rd++; d++;
        }
      }
      return true;
    }
    return false;
  }


  bool Matrix::Row::divide(Matrix::Rows &rows, Matrix::UniteType ut) const {
    uint32_t sz;
    uint32_t sh=0;
    int32_t rcols=0;
    Vbp* d  = data();
    Vbp* rd;
    switch (ut){
    case UtByVbp://([1||2||3])
      //Для всех частей выполнить
      for (int irow=0; irow<rows.count(); ++irow){
        d  = data();
        rd = rows.at(irow).data();
        sz = rows.at(irow).meta().size();
        rcols = rows.at(irow).count();
        //Для всех примеров выполнить
        for (int icol=0; icol<rcols; ++icol){
          (*rd).copy((*d).shift(sh).u8,sz);
          rd++; d++;
        }
        sh += sz;
      }
      return true;
    case UtByIndex://(123,123,123)
      if (!rows.count()) return true;
      sz    = rows.at(0).meta().size();
      rcols = rows.at(0).count();
      for (int icol=0; icol<rcols;++icol){
        for (int irow=0; irow<rows.count(); ++irow){
          rd = rows.at(irow).data() + icol;
          (*rd).copy((*d).u8,sz);
          rd++; d++;
        }
      }
      return true;
    case UtByOrder://(111,22,33333)
      if (!rows.count()) return true;
      sz = rows.at(0).meta().size();
      for (int irow=0; irow<rows.count(); ++irow){
        rd = rows.at(irow).data();
        rcols = rows.at(irow).count();
        for (int icol=0; icol<rcols;++icol){
          (*rd).copy((*d).u8,sz);
          rd++; d++;
        }
      }
      return true;
    }
    return false;

  }

  //Преобразование в форму неизменяемой матрицы с одним столбцом
  const nbMatrix Matrix::Row::toM() const{
    if (!_m) {  //Создание описания пустой матрицы
      nbMatrix m = {0,0,Null,Null};
      return m;
    } else {    //Создание описания полной матрицы
      nbMatrix m = {1,count(),(nbMeta*)_m->meta()+irow(), (nbVbp**)_m->data()+irow()};
      return m;
    }
  }


  //-------------------------------------------------------------------------------------
  //Matrix
  //-------------------------------------------------------------------------------------

  Matrix::Matrix(){
    _m.nrows = 0;
    _m.ncols = 0;
    _m.meta  = Null;
    _m.data  = Null;
    _ncap = 0;
  }


  Matrix::Matrix(int32_t nrows, int32_t ncols, const Meta *meta){
    _m.nrows = 0;
    _m.ncols = 0;
    _m.meta  = Null;
    _m.data  = Null;
    _ncap = 0;

    remeta(nrows,meta);
    resize(ncols);
  }


  Matrix::Matrix(int32_t ncols, Meta meta){
    _m.nrows = 0;
    _m.ncols = 0;
    _m.meta  = Null;
    _m.data  = Null;
    _ncap = 0;

    remeta(1,&meta);
    resize(ncols);
  }

  Matrix::Matrix(const Rows &rows){
    _m.nrows = 0;
    _m.ncols = 0;
    _m.meta  = Null;
    _m.data  = Null;
    _ncap = 0;

    remeta(rows);
    if (rows.count()){
      resize(rows.at(0).count());
      copy(rows);
    }
  }

  Matrix::~Matrix(){
    free();
  }


  Matrix::MatrixType Matrix::type() const {
    if (_m.nrows <= 0x0000FFFF) {
      if (_m.data)
        return MtFull;
      else {
        if (_m.meta) {
          if (_m.nrows)
            return MtTemplate;
          else
            return MtPointer;
        }
        else
          return MtEmpty;
      }
    }
    else {
      return MtGuid;
    }
  }


  Matrix::Row Matrix::row(int32_t irow) const{
    return Matrix::Row((Matrix&)*this,irow);
  }


  Matrix::Rows Matrix::rows() const{
    Matrix::Rows r;
    for (int32_t irow=0; irow<nrows(); ++irow)
      r.push_back(row(irow));
    return r;
  }


  Vbp Matrix::at(int32_t irow, int32_t icol) const {
    return (Vbp)_m.data[irow][icol];
  }


  Vbp&   Matrix::at(int32_t irow, int32_t icol){
    return (Vbp&)_m.data[irow][icol];
  }


  Vbp** Matrix::data() const {
    return (Vbp**)_m.data;
  }


  int32_t Matrix::ncols() const {
    return _m.ncols;
  }


  int32_t Matrix::nrows() const {
    return _m.nrows;
  }

  int32_t Matrix::ncap() const {
    return _ncap;
  }



  Meta* Matrix::meta() const {
    return (Meta*)_m.meta;
  }



  Meta  Matrix::meta(int32_t irow) const {
    return (Meta)_m.meta[irow];
  }



  bool Matrix::isEmpty() const{
    if (!_m.meta) return true;
    if (!_m.data || !_m.ncols) return true;
    for (int32_t irow=0; irow<(int32_t)_m.nrows; ++irow){
      if (!_m.data[irow])  return true;
    }
    return false;
  }



  bool Matrix::isRow(int32_t irow) const{
    if (!_m.meta || !_m.data || !_m.data[irow])  return false;
    return true;
  }


  void Matrix::empty(){
    resize(0);
  }

  void Matrix::free1(Vbp** d, int32_t nfrom){
    if (d && *d){
      Vbp* d2 = *d;
      for (int32_t ic=0; ic<nfrom; ++ic)  //Удалить примеры
        d2[ic].free();
      delete[] d2; //Удалить список примеров
      *d = Null;
    }
  }



  void Matrix::free(){
    if (_m.data){
      for (int32_t i=0;i<(int32_t)_m.nrows; ++i)
        free1((Vbp**)_m.data+i,_m.ncols);
      delete [] _m.data;  //Удалить список столбцов
      _m.data   = Null;
      _m.ncols  = 0;
      _ncap     = 0;
    }
    if (_m.meta){
      delete [] _m.meta;
      _m.meta = Null;
      _m.nrows= 0;
    }
  }


  bool Matrix::remeta(int32_t nrows, const Meta *meta){
    free(); //Удалить все данные
    if (nrows > 0x0000FFFF) return false;
    //Выделить память под метаописания
    if (!nrows) return false;
    _m.meta  = new nbMeta[nrows];
    if (!_m.meta) return false;
    _m.nrows = nrows;
    memcpy(_m.meta,meta,sizeof(nbMeta)*nrows);
    return true;
  }



  bool Matrix::remeta(const Rows &rows){
    int32_t c = rows.count();
    QVector<Meta> m; m.reserve(c);
    for (int32_t i=0; i<c; ++i)
      m.push_back(rows.at(i).meta());
    return remeta(c,m.data());
  }

  bool Matrix::remeta(Meta meta){
    return remeta(1,&meta);
  }

  bool Matrix::remeta1(int32_t irow, Meta meta){
    if (irow >= (int32_t)_m.nrows) return false;
    //Если данные уже были выделены
    if (_m.data){
      //Перевыделить память с учетом изменения метаописания, но не числа элементов
      if (!reserve1((Vbp**)_m.data,_ncap,_ncap,_m.meta[irow],meta))
        return false;
    }
    _m.meta[irow] = meta;
    return true;
  }



  bool Matrix::reserve1(Vbp** d, int32_t nfrom, int32_t nto, Meta mfrom, Meta mto){
    if (!d) return false;
    int32_t msto  = mto.size();
    int32_t msfrom= mfrom.size();
    if (msto > msfrom){
      //Перевыделить память под имеющиеся примеры (копировать части данных не нужно)
      Vbp *d2 = *d;
      for (int32_t ic=0; ic<nfrom; ++ic){
        if (d2[ic]) {
          d2[ic].free();
          if (!d2[ic].create(msto)) return false;
        }
      }
    }

    if (nto > nfrom){
      //Увеличить число примеров
      Vbp* d2 = new Vbp[nto];
      if (!d2) return false;
      //Скопировать имеющиеся ссылки на них
      for (int32_t ic=0; ic<nfrom; ++ic)
        d2[ic] = d[0][ic];
      //Выделить память под недостающие примеры
      for (int32_t ic=nfrom; ic<nto; ++ic)
        if (!d2[ic].create(msto)) return false;
      //Удалить список примеров (сами примеры использовались повторно)
      if (*d) delete[] *d;
      *d = d2;
    }
    return true;
  }



  bool Matrix::reserve(int32_t capacity){
    if (!_m.nrows) return false;
    if (_ncap >= capacity) return true;  //Проверить необходимость перевыделения памяти

    //Если вектора частей нет, создать его
    if (!_m.data){
      _m.data = (nbVbp**)new Vbp*[_m.nrows];
      if (!_m.data) return false;
      for (int32_t ir=0; ir<(int32_t)_m.nrows; ++ir)
        _m.data[ir]=Null;
    }

    //Выполнить резервирование данных примеров
    for (int32_t ir=0; ir<(int32_t)_m.nrows; ++ir){
      if (!reserve1((Vbp**)(_m.data+ir),_ncap,capacity,Meta(),Meta(_m.meta[ir])))
        return false;
    }
    _ncap = capacity;

    return true;
  }



  bool Matrix::resize(int32_t ncols){
    if (ncols > _ncap) {      //Проверить размер буфера
      if (!reserve(ncols)) return false;
    }
    _m.ncols = ncols;
    return true;
  }


  bool Matrix::copy(const Rows &rows){
    int32_t nr = (rows.count() > nrows())?nrows():rows.count();
    for (int32_t ir=0; ir<nr; ++ir){
      int32_t ms = rows[ir].meta().size();
      //Копирование данных по столбцам
      Vbp* s = rows[ir].data();
      Vbp* d = (Vbp*)_m.data[ir];
      //Для каждого из примеров
      for (int32_t ic=0; ic<rows[ir].count(); ++ic)
        d[ic].copy(s[ic],ms);
    }
    return true;
  }

  bool Matrix::copy(const Matrix &m){
    if (this == &m) return true;
    Rows rows = m.rows();
    return copy(rows);
  }


  bool Matrix::convert(const Rows &rows){
    if (rows.count() != nrows()) return false;
    //Для каждого вектора
    for (int32_t ir=0; ir<nrows(); ++ir){
      row(ir).convert(rows[ir]);
    }
    return true;
  }

  //Конвертация матрицы из одного формата в другой
  //(число векторов и число элементов вектора должны совпадать)
  bool Matrix::convert(const Matrix &m){
    if (this == &m) return true;
    Rows rows = m.rows();
    return convert(rows);
  }


  //Запись матрицы в поток
  int32_t Matrix::write(QDataStream &to) const {
    int32_t w = 0;
    if (to.status() != QDataStream::Ok)
      return false;

    if ((type() == Matrix::MtEmpty) || (type() == Matrix::MtTemplate) || (type() == Matrix::MtFull)){
      uint32_t nil = 0;
      //Записать заголовок матрицы ВБП
      w += to.writeRawData((const char*)&_m.nrows, sizeof(uint32_t));       //число примеров
      w += to.writeRawData((const char*)&_m.ncols, sizeof(uint32_t));       //число частей
      w += to.writeRawData((const char*)&nil, sizeof(uint32_t));            //смещение метаописания
      w += to.writeRawData((const char*)&nil, sizeof(uint32_t));            //смещение данных
      if (to.status() != QDataStream::Ok) return -1;
      //Записать метаописание матрицы ВБП
      if (_m.nrows)
        for (uint32_t row = 0; row < _m.nrows; row++)
          w += to.writeRawData((const char*)&meta(row), sizeof(nbMeta));
      //Записать данные матрицы ВБП
      if (_m.nrows && _m.ncols){
        for (uint32_t row = 0; row < _m.nrows; row++)
          for (uint32_t col = 0; col < _m.ncols; col++)
            w += to.writeRawData((const char*)((nbVbp)(data()[row][col])), meta(row).size());
      }
    } else { //Тип матрицы: глобальный идентификатор базы образов || указатель интерфейса для доступа к базе
      //Записать заголовок матрицы ВБП, интерпретируя поля указателей особым образом
      w += to.writeRawData((const char*)&_m.nrows, sizeof(uint32_t));      //число примеров
      w += to.writeRawData((const char*)&_m.ncols, sizeof(uint32_t));      //число частей
      w += to.writeRawData((const char*)&_m.meta, sizeof(uint32_t));        //смещение метаописания
      w += to.writeRawData((const char*)&_m.data, sizeof(uint32_t));        //смещение данных
    }
    if (to.status() != QDataStream::Ok) return -1;
    return w;
  }

  //Чтение матрицы из потока
  int32_t Matrix::read(QDataStream &from)  {
    int32_t r=0;
    if (from.status()!= QDataStream::Ok) return -1;

    free();
    Matrix t;
    //Считать заголовок матрицы ВБП
    r += from.readRawData((char*)&t._m.nrows, sizeof(uint32_t));   //число примеров
    r += from.readRawData((char*)&t._m.ncols, sizeof(uint32_t));   //число частей
    r += from.readRawData((char*)&t._m.meta, sizeof(uint32_t));    //смещение метаописания
    r += from.readRawData((char*)&t._m.data, sizeof(uint32_t));    //смещение данных
    if (from.status() != QDataStream::Ok) return -1;

    if ((t.type() == Matrix::MtEmpty) || (t.type() == Matrix::MtTemplate) || (t.type() == Matrix::MtFull)){
      Meta* metaBuff = new Meta[t.nrows()];
      if (!metaBuff) return false;
      //Считать метаописание матрицы ВБП
      for (int32_t row = 0; row < t.nrows(); row++){
        r += from.readRawData((char*)&metaBuff[row], sizeof(nbMeta));
      }
      remeta(t.nrows(),metaBuff);
      if (metaBuff) delete[] metaBuff;
      //Считать данные матрицы ВБП
      resize(t.ncols());
      if (_m.nrows && _m.ncols){
        for (int32_t row = 0; row < (int32_t)_m.nrows; row++){
          for (int32_t col = 0; col < (int32_t)_m.ncols; col++){
            r += from.readRawData((char*)((nbVbp)data()[row][col]), meta(row).size());
          }
        }
      }
    } else {
      _m.data = (nbVbp**)t._m.data;
      _m.meta = (nbMeta*)t._m.meta;
    }
    t._m.data = Null;
    t._m.meta = Null;
    if (from.status() != QDataStream::Ok) return -1;
    return r;
  }

  //Создание вектора ссылок по матрице
  Matrix::Rows Matrix::fromM(const nbMatrix &m){
    Matrix::Rows rr;
    for (int32_t i=0; i<(int32_t)m.nrows; ++i)
      rr.push_back(Row(&m,i));
    return rr;
  }



#if !defined(QT_NO_DEBUG_STREAM)

  //Данные столбца
  QDebug operator<<(QDebug  dbg, const Matrix::Row &x){
    QDebug d = dbg.nospace();
    if (!x._m) return d<<"Row(Nil)";
    if (x.irow()>=x._m->nrows()) return d<<"Row(ErrIRow)";
    if (!x._m->meta()) return d<<"Row(ErrMeta)";

    d<<"Row(cols:"<<x.count()<<" meta:"<<x.meta()<<" data:";
    Meta mt = x.meta();
    if (!x.data())
      d<<"\nNil";
    else
    for (int i=0; i<x.count(); ++i){
      Vbp data = x.at(i);
      if (!data.v){
        d<<i<<":Nil ";
      } else {
        switch(mt.format()){
        case MfI1:
        case MfU1:
          d<<"\n\t"<<i<<":["<<AsU1(data.u8,4,mt.size())<<"] ";
          break;
        case MfU32:
        case MfI32:
          d<<"\n\t"<<i<<":["<<AsI32(data.i32,8,QChar::Nbsp,mt.size()/sizeof(int32_t))<<"] ";
          break;
        case MfR32:
          d<<"\n\t"<<i<<":["<<AsR32(data.r32,8,QChar::Nbsp,mt.size()/sizeof(float))<<"] ";
          break;
        default:
          d<<"\n\t"<<i<<":["<<AsHex(data.u8,8,QChar::Nbsp,mt.size())<<"] ";
        }
      }
    }
    return d<<"\n)";
  }

  QDebug operator<<(QDebug  dbg, const Matrix &x){
    QDebug d = dbg.nospace();

    d<<"Matrix(rows:"<<x.nrows()<<" cols:"<<x.ncols()<<" caps:"<<x.ncap()<<" data:\n";
    if (x.data())
    for (int i=0; i<x.nrows(); ++i){
      d<<i<<":"<<x.row(i)<<"\n";
    } else {
      d<<"Nil\n";
    }
    return d<<")";
  }

#endif

}//end namespace Nb

//////////////////////////////////////////////////////////////////////////
//����:
//������:
//����:
//�������:
//�����:        ������� �.�.
//
//����:         ���������� ������ ������� �������� ��������������
//              ����������
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_MATRIX_H
#define _NB_MATRIX_H

#include <QtCore/QDataStream>
#include <QtCore/QList>

#include "Enums.h"
#include "Meta.h"
#include "Vbp.h"

#pragma pack(1)

namespace Nb {

  //...........................................................................
  //  ������� ���
  //...........................................................................
  class Matrix {
  public:

    class Row;
    typedef QList<Matrix::Row> Rows;

    enum UniteType {
      UtByVbp,    //�����������/���������� ������ (1,2,3)...(1,2,3)
                  //  ������������ rows, ����� count, � ����� �������� ������ ���������
      UtByIndex,  //�����������/���������� �� ������� rows 1,2,3,...,1,2,3
                  //  ������������ rows � ����� �������� ������ ���������
      UtByOrder   //�����������/���������� �� ������� rows 1,1,1,...,3,3,3
                  //  ������������ rows � ����� �������� ������ ���������
    };
    //��� ������������� �������
    enum MatrixType {
      MtEmpty     = 0x00, //��� ���� 0
      MtTemplate  = 0x01, //���� �� ����
      MtFull      = 0x02, //���� � ������ �� ����
      MtGuid      = 0x03, //������������� ���������� (��������������)
      MtPointer   = 0x04  //��������� �� ��������� (��������������)
    };

    //������ �� ������ �������
    //����������:
    //  1. ��������� ������� �� ������ �������������, ���� ������� ������ �� ���
    //  2. ��������� ������� ������������ ������ �� ������ ������� ����� �������,
    //      � ������ "��������� �������", "��������� ������������"
    //  3. �������� "������/������" ������ ������� ������ ������ ������� ��������� ������� ��� ������������
    class Row {
    public:
      Row();                                      //����������� ������ ������
      Row(const Row &row);                        //����������� ����������� ������
      Row(Matrix &m, int32_t irow);               //����������� ������ �� �������
      Row(const nbMatrix *m, int32_t irow);       //����������� ������ �� ������������ �������

      Vbp*    operator*() const;

      Vbp*    data() const;                       //������
      Meta    meta() const;                       //������������
      int32_t count() const;                      //����� ��������

      Vbp    at(int32_t icol) const;              //������ �� ������ �������
      Vbp&   at(int32_t icol);                    //������ �� ������ �������

      Vbp operator [](int32_t icol) const;        //������ �� ������ �������
      Vbp& operator [](int32_t icol);             //������ �� ������ �������

      Row& operator=(const Row& row);             //�������� ������������
      Row ref() const;                            //�������� �������� ������

      bool isNull() const;
      bool isEmpty() const;   //�������� ������� ������
      bool isLocked() const;   //�������� ����������� (������ � ������ "������/������" ������)

      //��������� ������ (��������� ������������ ��� ��� �������)

      bool resize(int32_t ncols);                 //��������� ������ �� ����� ��������
      bool resize(int32_t ncols, Meta meta);      //��������� ������ �� ����� �������� � ������������

      void clear();         //�������� ������ (������ �� ���������)
      void tideUp();        //��������� ������

      //����������� ���� �������� (row.count <= count)
      //����������:
      //  ��������� ������ �� ����������
      //  ������������ � ����� �������� ������ ���������
      bool copy(const Row &row);
      //����������� ������ ��������
      bool copy1(const Row &row, int32_t ifrom, int32_t ito);
      //����������� ����� ��������
      //  ifrom, ito, icount  - � ������ ������ ���������, � ������ ������ � ��������, �������
      bool copy(const Row &row, int32_t ifrom, int32_t ito, int32_t icount);
      //����������� ����� �������� �� ������� �� ������
      //  ifrom, ito, icount  - � ������ ������ ���������, � ������ ������ � ��������, �������
      //  sfrom, sto, ssize   - � ������ ����� ���������, � ����� ���� ���������, ����� �����
      bool copyb(const Row &row, int32_t ifrom, int32_t ito, int32_t icount, int32_t sfrom, int32_t sto, int32_t ssize);
      //����������� ����� �������� �� ������� �� ����� ���������
      //  ifrom, ito, icount  - � ������ ������ ���������, � ������ ������ � ��������, �������
      //  efrom, eto, esize   - � ������ �������� ���������, � ����� ������� ���������, ������� ���������
      //bool copy(const Row &row, int32_t ifrom, int32_t ito, int32_t icount, int32_t efrom, int32_t eto, int32_t ecount);


      //����������� � ������������ ������������ ������� �������������
      //����������:
      //  ��������� ������ �� ����������
      //  ������������ � ����� �������� ������ ���������
      bool convert(const Row &row);
      //bool convert(const Row &row, int32_t ifrom, int32_t ito, int32_t icount, int32_t efrom, int32_t eto, int32_t ecount);

      //����������� ��������
      //����������: ��������� ������ ������ ����������� � ������ resize(..., Unite ut)
      //            �������� �� ������������ �� ������������
      bool unite(const Rows &rows, UniteType ut);
      //����������� �� ��������
      //����������: ��������� ������ � rows ������ ����������� �������� � ����������� �� ���� ����������
      //            �������� �� ������������ ���������� �� ������������
      bool divide(Rows &rows, UniteType ut) const;

      //���������� ��������� ����������� rows
      //  umeta   [���] - ����� ������������
      //  ucount  [���] - ����� ������
      static bool metaForUnite(const Rows &rows, UniteType ut, int32_t &ucount, Meta &umeta);

      //�������������� � ����� ������������ ������� � ����� ��������
      const nbMatrix toM() const;

    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug friend operator<<(QDebug  dbg, const Row &x);
    #endif

    protected:
      Matrix* _m;   //������� � �������
      int32_t _irow;//����� ������ (���� >=0, Matrix �������� � ������ "������", "������", "���������"
                    //        ���� <0, �� � ������ "������", "������" ��� ��������� ������� � ������������
                    //
      friend class Matrix;
    private:
      inline int32_t irow() const { return (_irow & ~0x80000000); } //������ ���������� �����
    };


  public:

    Matrix();                                                 //����������� ������� MtEmpty
    Matrix(int32_t nrows, int32_t ncols, const Meta *meta);   //����������� ������ �������
    Matrix(int32_t ncols, Meta meta);                         //����������� ������� � ����� ��������
    Matrix(const Rows &rows);

    ~Matrix();

    inline operator nbMatrix&() {return _m;}
    inline operator const nbMatrix&() const {return _m;}
    inline Vbp* operator[](int32_t irow) const { return (Vbp*)_m.data[irow]; }

    Row     row(int32_t irow) const; //��������� ������ �� �����
    Rows    rows() const;            //���������� �� ������

    Vbp     at(int32_t irow, int32_t icol) const;  //������ � ���
    Vbp&    at(int32_t irow, int32_t icol);        //������ � ���

    Vbp**   data() const;
    int32_t ncols() const;            //����� ��������
    int32_t ncap() const;             //����������������� ����� ��������
    int32_t nrows() const;            //����� ������ ��������
    Meta*   meta() const;             //������������ �������
    Meta    meta(int32_t irow) const; //������������ �����

    MatrixType type() const;          //��� �������
    bool    isEmpty() const;          //�������� ������� ������� � ������
    bool    isRow(int32_t irow) const;//�������� ������� ������ � �����

    //��������� ������������ ������
    bool remeta(int32_t nrows, const Meta *meta);
    bool remeta(const Rows &rows);
    bool remeta(Meta meta);
    //��������� ������������ ����� (���������, ���� �����������)
    bool remeta1(int32_t irow, Meta meta);

    //��������� ������ ��� �������
    bool resize(int32_t ncols);
    bool reserve(int32_t capacity);

    //������������ ������ ��� �������� ������������ (ncols=0)
    void empty();
    //������ ������������ ������, ������� ������������ � ����������������� ������
    void free();

    //����������� ������ �������� ��� ������������� ������
    //����������: ����� �������� � ������ ����� ������ ���� �� ������ ��������������
    //            ������ �� ��������������
    bool copy(const Rows &rows);
    bool copy(const Matrix &m);
    //����������� ������ �������� ��� ������������� ������
    //����������: ����� �������� � ������ ����� ������ ���� �� ������ ��������������
    //            ������ �� ��������������
    bool convert(const Rows &rows);
    bool convert(const Matrix &m);


    //������ ������� � �����
    //���������: ����� ���������� ���� ��� -1
    int32_t write(QDataStream &to) const;
    //������ ������� �� ������
    //���������: ����� ���������� ���� ��� -1
    int32_t read(QDataStream &from);

    //�������� ������� ������ �� �������
    static Rows fromM(const nbMatrix &m);

  protected:
    //��������� ������ ��� ���� ������ ��������
    bool reserve1(Vbp** d, int32_t nfrom, int32_t nto, Meta mfrom, Meta mto);
    //������������ ������ ��-��� ������ ������� ��������
    void free1(Vbp** d, int32_t nfrom);

    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug friend operator<<(QDebug  dbg, const Matrix &x);
    #endif

  private:
    nbMatrix _m;        //�������
    int32_t  _ncap;     //����������������� ����� ��������
  };
}

#pragma pack()
#endif //_NB_MATRIX_H

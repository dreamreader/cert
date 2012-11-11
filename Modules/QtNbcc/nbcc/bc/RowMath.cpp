#include "md5.h"
#include "RowMath.h"

namespace Bc {
  using namespace Nb;



  bool RowMath::gen(Matrix::Row &ro){
    int32_t sz = ro.meta().size();
    int32_t ncol = ro.count();
    Vbp *d = ro.data();
    for (int32_t icol=0; icol<ncol; icol++){
      Vbp x = d[icol];
      int32_t rem = sz;
      int32_t sh  = 0;
      while (rem>0){
        Uuid id = (Uuid)Uuid::createUuid();
        x.shift(sh).copy((uint8_t*)id.data(),(rem<Uuid::uuidSIZE)?rem:Uuid::uuidSIZE);
        sh  += Uuid::uuidSIZE;
        rem -= Uuid::uuidSIZE;
      }
    }
    return true;
  }



  bool RowMath::gen(Matrix::Rows &rs){
    for (int32_t irow=0; irow<rs.count(); ++irow){
      if (!gen(rs[irow])) return false;
    }
    return true;
  }




  bool RowMath::hash(const Matrix::Rows &ris, Matrix::Row &ro, const Matrix::Row *ad, const Matrix::Row *salt){
    int32_t ysize = ro.meta().size();
    int32_t ssize = (salt) ? salt->meta().size() : 0;
    int32_t ncol = ro.count();
    Vbp *y = ro.data();
    Vbp s  = (salt) ? salt->data()[0] : (Vbp)Null;
    Vbp a  = (ad) ? ad->data()[0] : (Vbp)Null;

    Md5Hash hash;
    Md5Hash::Md5Context ctx;
    for (int32_t icol=0; icol<ncol; icol++){ //Выполнить вычисление для всех примеров
      int32_t sh  = 0;
      while (sh<ysize){
        hash.init(ctx);
        for (int32_t k=0; k<ris.count(); ++k){  //Прохешировать все входные ключи
          Vbp *x        = ris[k].data();
          int32_t xsize = ris[k].meta().size();
          hash.update(ctx,x[icol].u8,xsize);
        }
        if (s.u8) //Прохешировать соль
          hash.update(ctx,s.u8,ssize);
        hash.final(ctx);
        //Сложить с дополнением или просто скопировать
        int32_t len = ((ysize-sh)<Md5Hash::md5HASH_SIZE) ? ysize-sh : Md5Hash::md5HASH_SIZE;
        if (a.u8){
          for (int32_t j=0; j<len; j++)
            y[icol].shift(sh).u8[j] = hash.digest[j] ^ a.shift(sh).u8[j];
        } else {
          y[icol].shift(sh).copy((Vbp)&hash.digest,len);
        }
        sh  += len;
      }
    }
    return true;
  }



  bool RowMath::cat(const Matrix::Rows &ris, Matrix::Row &ro){
    return ro.unite(ris,Matrix::UtByVbp);
  }



  bool RowMath::uncat(const Matrix::Row &ri, Matrix::Rows &ros){
    return ri.divide(ros,Matrix::UtByVbp);
  }


  bool RowMath::hi(const Matrix::Row &ri, Matrix::Row &ro, int32_t hlen, const Md5Hash &h){
    Vbp* x = ri.data();
    Vbp* y = ro.data();
    int32_t sz  = ri.meta().size();
    int32_t len = ri.meta().count();
    for (int32_t i=0; i<ri.count(); ++i, ++x, ++y){
      Md5Hash h2((*x).u8,sz);
      h2.trim(hlen);
      if (h2==h) (*y).i32[0] = 0;
      else       (*y).i32[0] = len;
    }
    return true;
  }



  bool RowMath::genhi(const Matrix::Row &ri,  int32_t hlen, Md5Hash &h){
    h.create((*ri.data()).u8,ri.meta().size());
    h.trim(hlen);
    return true;
  }

}

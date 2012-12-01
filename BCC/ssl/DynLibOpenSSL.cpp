//////////////////////////////////////////////////////////////////////////
//файл:         DynLibOpenSSL.cpp
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Модуль, поддерживающий динамическую загрузку функций 
//							библиотеки OpenSSL
//////////////////////////////////////////////////////////////////////////

#include <QtGui/QApplication>
#include <qstringlist>
#include "DynLibOpenSSL.h"
#include "protocol/log.h"

// Конструктор
DynLibOpenSSL::DynLibOpenSSL(){
 
}

// Загрузка библиотеки
bool DynLibOpenSSL::load(QString pathname){
  QStringList paths = QApplication::libraryPaths();
  foreach(const QString &path, paths){
    setFileName(path+"/"+pathname);
    if (this->QLibrary::load()) break;
  }
  if (!this->QLibrary::isLoaded()){
    setFileName(pathname);
    if (!this->QLibrary::load()) return false;
  }

  //Считать все функции
  //Найти точку входа в библиотеку
	BIO_new_f = (BIO_new_type)resolve("BIO_new");
    if (!BIO_new_f) return false;
    BIO_s_file_f = (BIO_s_file_type)resolve("BIO_s_file");
    if (!BIO_s_file_f) return false;
    BIO_ctrl_f = (BIO_ctrl_type)resolve("BIO_ctrl");
    if (!BIO_ctrl_f) return false;
    PEM_read_bio_PrivateKey_f = (PEM_read_bio_PrivateKey_type)resolve("PEM_read_bio_PrivateKey");
    if (!PEM_read_bio_PrivateKey_f) return false;
    EVP_PKEY_size_f = (EVP_PKEY_size_type)resolve("EVP_PKEY_size");
    if (!EVP_PKEY_size_f) return false;
    OpenSSL_add_all_digests_f = (OpenSSL_add_all_digests_type)resolve("OpenSSL_add_all_digests");
    if (!OpenSSL_add_all_digests_f) return false;
    EVP_get_digestbyname_f = (EVP_get_digestbyname_type)resolve("EVP_get_digestbyname");
    if (!EVP_get_digestbyname_f) return false;
    EVP_MD_CTX_create_f = (EVP_MD_CTX_create_type)resolve("EVP_MD_CTX_create");
    if (!EVP_MD_CTX_create_f) return false;
    EVP_SignInit_f = (EVP_SignInit_type)resolve("EVP_DigestInit");
    if (!EVP_SignInit_f) return false;
    EVP_SignUpdate_f = (EVP_SignUpdate_type)resolve("EVP_DigestUpdate");
    if (!EVP_SignUpdate_f) return false;
    EVP_SignFinal_f = (EVP_SignFinal_type)resolve("EVP_SignFinal");
    if (!EVP_SignFinal_f) return false;
    EVP_PKEY_free_f = (EVP_PKEY_free_type)resolve("EVP_PKEY_free");
    if (!EVP_PKEY_free_f) return false;
    EVP_MD_CTX_destroy_f = (EVP_MD_CTX_destroy_type)resolve("EVP_MD_CTX_destroy");
    if (!EVP_MD_CTX_destroy_f) return false;
    ERR_load_crypto_strings_f = (ERR_load_crypto_strings_type)resolve("ERR_load_crypto_strings");
    if (!ERR_load_crypto_strings_f) return false;
    ERR_error_string_f = (ERR_error_string_type)resolve("ERR_error_string");
    if (!ERR_error_string_f) return false;
    ERR_get_error_f = (ERR_get_error_type)resolve("ERR_get_error");
    if (!ERR_get_error_f) return false;
    PEM_read_bio_X509_f = (PEM_read_bio_X509_type)resolve("PEM_read_bio_X509");
    if (!PEM_read_bio_X509_f) return false;
    d2i_X509_bio_f = (d2i_X509_bio_type)resolve("d2i_X509_bio");
    if (!d2i_X509_bio_f )return false;
    X509_get_pubkey_f = (X509_get_pubkey_type)resolve ("X509_get_pubkey");
    if (!X509_get_pubkey_f) return false;
    EVP_VerifyFinal_f = (EVP_VerifyFinal_type)resolve("EVP_VerifyFinal");
    if (!EVP_VerifyFinal_f) return false;
    X509_free_f = (X509_free_type)resolve("X509_free");
    if (!X509_free_f) return false;
    BF_set_key_f = (BF_set_key_type)resolve("BF_set_key");
    BF_cbc_encrypt_f = (BF_cbc_encrypt_type)resolve("BF_cbc_encrypt");
	
	return true;
}


// Последняя ошибка
//
//
//
//
//
QString DynLibOpenSSL::error()
{
  ERR_load_crypto_strings_f();
  return QString(ERR_error_string_f(ERR_get_error_f(),NULL));
}


// Подписать данные, сформировать ЭЦП
//
//
//
//
//
int DynLibOpenSSL::getSignData (char *keyFile, char *dataArr, int dataSize, char **signHex){

    char HEX[17] = "0123456789abcdef";
    unsigned char *sign;    // подпись - результат вычислений
    unsigned int slen;                       
    char *hex;              // подпись в шестнадцатеричном представлении
    int signHexSize;

    int i, j;
    EVP_MD *md;
    EVP_PKEY  *key;
    EVP_MD_CTX *mdctx;

    Log::write("========= ssl 1");
    // Читаем через буфер
    BIO *buf=NULL;
    Log::write("========= ssl 1.1");
    if ((buf=BIO_new_f(BIO_s_file_f())) == NULL) goto err;
    Log::write("========= ssl 1.2");
    Log::write(QString(keyFile));
    if (BIO_ctrl_f(buf, BIO_C_SET_FILENAME, BIO_CLOSE|BIO_FP_READ, keyFile) <= 0) goto err;
    Log::write("========= ssl 1.3");
    key = PEM_read_bio_PrivateKey_f(buf,NULL,NULL,NULL);
    if(key == NULL) goto err;

    Log::write("========= ssl 2");
    // определяем размер ключа
    slen = EVP_PKEY_size_f(key);

    Log::write("========= ssl 3");
    // размер подписи будет таким же
    sign = (unsigned char*) malloc(slen);
    if(sign == NULL)  goto err;

    Log::write("========= ssl 4");
    // получаем доступ к хэш-функции MD5
    OpenSSL_add_all_digests_f();
    md  = (EVP_MD*)EVP_get_digestbyname_f("MD5");
    if(md == NULL) goto err;

    Log::write("========= ssl 5");
    // формируем контекст подписи
    mdctx = EVP_MD_CTX_create_f();
    if(mdctx == NULL) goto err;

    Log::write("========= ssl 6");
    // устанавливаем хэш-функцию, которая буддет использована
    if(! EVP_SignInit_f(mdctx, md)) goto err;

    Log::write("========= ssl 7");
    // подписываем данные
    if(! EVP_SignUpdate_f(mdctx, dataArr, dataSize)) goto err;

    Log::write("========= ssl 8");
    // завершаем формирование подписи
    if(! EVP_SignFinal_f (mdctx, sign, &slen, key)) goto err;

    Log::write("========= ssl 9");
    // преобразуем результат вычислений в
    // шестнадцатеричное представление
    signHexSize = slen * 2 + 1;
    hex = (char*) malloc(signHexSize);
    if(hex == NULL)  goto err;
    j = 0;
    for (i = 0; i < slen; i++) {
        hex[j++] = HEX[sign[i] >> 4];
        hex[j++] = HEX[sign[i] & 0xF];
    }
    hex[j] = '\0';

    qDebug () << "Signature :" << hex;

    Log::write("========= ssl 10");
    // освобождаем ресурсы
    free(sign);
    Log::write("========= ssl 10.1");
    EVP_PKEY_free_f(key);
    Log::write("========= ssl 10.2");
    EVP_MD_CTX_destroy_f(mdctx);

    *signHex = hex;
    Log::write("========= ssl 11");
    return signHexSize;

    // отладочная информация при сбое
err:
    return -1;
    /*ERR_load_crypto_strings_f();
    qDebug () << "ERROR: " << ERR_error_string_f(ERR_get_error_f(),NULL);
    return ERR_get_error_f();*/
}

// Проверить ЭЦП, верифицировать данные
//
//
//
//
//
int DynLibOpenSSL::VerifySignData (char *certFile, char *signHex, int signHexSize, char *dataArr, int dataSize){

    // подпись e-port в шестнадцатеричном представлении
    unsigned char * sign;
    int slen;
    int i, j;
    char c;
    unsigned char h;
    int rzlt=0;

    BIO *buf=NULL;
    X509 *cert=NULL;
    EVP_MD *md;
    EVP_PKEY *key=NULL;
    EVP_MD_CTX *mdctx;

    // читаем файл сертификата публичного ключа e-port
    if ((buf=BIO_new_f(BIO_s_file_f())) == NULL) return -1;
    if (BIO_ctrl_f(buf, BIO_C_SET_FILENAME, BIO_CLOSE|BIO_FP_READ, certFile) <= 0) return -1;
    if ((cert = PEM_read_bio_X509_f(buf,NULL, NULL, NULL)) == NULL){
        // Указанный файл DER
        buf=NULL;
        if ((buf=BIO_new_f(BIO_s_file_f())) == NULL) return -1;
        if (BIO_ctrl_f(buf, BIO_C_SET_FILENAME, BIO_CLOSE|BIO_FP_READ, certFile) <= 0) return -1;
        cert = d2i_X509_bio_f(buf, NULL);
    }

    if(cert == NULL)  goto err;

    // извлекаем публичный ключ ЭЦП e-port
    key = X509_get_pubkey_f(cert);
    if(key == NULL)  goto err;

    // определяем размер ключа
    slen = EVP_PKEY_size_f(key);

    if (signHexSize < slen) goto err;

    // размер подписи будет таким же
    sign = (unsigned char*) malloc(slen);
    if(sign == NULL) goto err;

    // получаем данные подписи из шестнадцатеричного предстваления
    for(i = 0, j = 0; i < slen; i++){
        c = signHex[j++];
        if (c > 47 && c < 58) h = c - 48;
        else if (c > 96 && c < 103) h = c - 87;
        else if (c > 64 && c < 71)  h = c - 55;
        else return -1;
        h = h << 4;
        c = signHex[j++];
        if (c > 47 && c < 58) h = h | (c - 48);
        else if (c > 96 && c < 103) h = h | (c - 87);
        else if (c > 64 && c < 71)  h = h | (c - 55);
        else return -1;
        sign[i] = h;
    }

    // получаем доступ к хэш-функции MD5
    OpenSSL_add_all_digests_f();
    md  = (EVP_MD*)EVP_get_digestbyname_f("MD5");
    if(md == NULL) goto err;

    // формируем контекст подписи
    mdctx = EVP_MD_CTX_create_f();
    if(mdctx == NULL) goto err;

    // устанавливаем хэш-функцию, которая буддет использована
    // Использовать EVP_SignInit_f
    if(! EVP_SignInit_f(mdctx, md))  goto err;

    // инициализируем контекст подписанными данными
    // Использовать EVP_SignUpdate_f
    if(! EVP_SignUpdate_f(mdctx, dataArr, dataSize)) goto err;

    // проверяем полдпись:
    rzlt = EVP_VerifyFinal_f (mdctx, sign, slen, key);
    free(sign);
    X509_free_f(cert);
    EVP_PKEY_free_f(key);
    EVP_MD_CTX_destroy_f(mdctx);

    if (( rzlt != 0 ) && ( rzlt != 1)) goto err;

    return rzlt;
    // отладочная информация при сбое
err:
    ERR_load_crypto_strings_f();
    qDebug () << "ERROR: " << ERR_error_string_f(ERR_get_error_f(),NULL);
    return -1;

}

void DynLibOpenSSL::blowfishCrypt(unsigned char *in, unsigned char *out, unsigned char *keyBuf, int length, int mode){

    BF_KEY key;
    //unsigned char keyBuf[]="SECRETKEY!";
    unsigned char ivec[8];

    memset(ivec, 0, sizeof(ivec));
    BF_set_key_f(&key, strlen((char*)keyBuf), keyBuf);

    if (mode == 0)
    //BF_cbc_encrypt(in, out, strlen((char*)in), &key, ivec, BF_ENCRYPT);
        BF_cbc_encrypt_f(in, out, length, &key, ivec, BF_ENCRYPT);

    if (mode==1)
   // BF_cbc_encrypt(in, out, strlen((char*)in), &key, ivec, BF_DECRYPT);
        BF_cbc_encrypt_f(in, out, length, &key, ivec, BF_DECRYPT);

}

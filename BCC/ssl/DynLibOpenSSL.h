#pragma once
//////////////////////////////////////////////////////////////////////////
//файл:         DynLibOpenSSL.h
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Модуль, поддерживающий динамическую загрузку функций 
//							библиотеки OpenSSL
//////////////////////////////////////////////////////////////////////////
#ifndef _DYNLIB_OPENSSL_H
#define _DYNLIB_OPENSSL_H

#include <QtCore/QLibrary>
#include <Qdebug.h>
#include "openssl/pem.h"
#include "openssl/evp.h"
#include "openssl/x509.h"
#include "openssl/ssl.h"
#include "openssl/bio.h"
#include "openssl/err.h"
#include "openssl/blowfish.h"

typedef BIO* (*BIO_new_type)(BIO_METHOD*);
typedef BIO_METHOD* (*BIO_s_file_type)(void);
typedef long (*BIO_ctrl_type)(BIO*,int,long, const void*);
typedef EVP_PKEY* (*PEM_read_bio_PrivateKey_type)(BIO*, EVP_PKEY**, pem_password_cb*, void*);
typedef int (*EVP_PKEY_size_type)(EVP_PKEY*);
typedef void (*OpenSSL_add_all_digests_type)(void);
typedef const EVP_MD* (*EVP_get_digestbyname_type)(const char*);
typedef EVP_MD_CTX* (*EVP_MD_CTX_create_type)(void);
typedef int (*EVP_SignInit_type)(EVP_MD_CTX*, const EVP_MD*);
typedef int (*EVP_SignUpdate_type)(EVP_MD_CTX*,const void*,size_t);
typedef int (*EVP_SignFinal_type)(EVP_MD_CTX*,unsigned char*,unsigned int*,EVP_PKEY*);
typedef void (*EVP_PKEY_free_type)(EVP_PKEY*);
typedef void (*EVP_MD_CTX_destroy_type)(EVP_MD_CTX*);
typedef void (*ERR_load_crypto_strings_type)(void);
typedef char* (*ERR_error_string_type)(unsigned long, char*);
typedef unsigned long (*ERR_get_error_type)(void);
typedef X509* (*PEM_read_bio_X509_type)(BIO*, X509**, pem_password_cb*, void*);
typedef X509* (*d2i_X509_bio_type)(BIO*, X509**);
typedef EVP_PKEY* (*X509_get_pubkey_type)(X509*);
typedef int (*EVP_VerifyFinal_type)(EVP_MD_CTX*,const unsigned char*, unsigned int, EVP_PKEY*);
typedef void (*X509_free_type)(X509*);
typedef void (*BF_set_key_type)(BF_KEY*, int, const unsigned char*);
typedef void (*BF_cbc_encrypt_type)(const unsigned char*, unsigned char*, long, const BF_KEY*, unsigned char*, int);



class DynLibOpenSSL: public QLibrary {
  public:
  // Конструктор
  DynLibOpenSSL();

  QString error();

  bool load(QString pathname);
  
  // Подписать данные, сформировать ЭЦП
  int getSignData (char *keyFile, char *dataArr, int dataSize, char **signHex);
  
  // Проверить ЭЦП, верифицировать данные
  int VerifySignData (char *certFile, char *signHex, int signHexSize, char *dataArr, int dataSize);

  // Шифрование данных алгоритмом Blowfish
  void blowfishCrypt(unsigned char *in, unsigned char *out, unsigned char *keyBuf, int length, int mode);



protected:

	BIO_new_type BIO_new_f;
	BIO_s_file_type BIO_s_file_f;
	BIO_ctrl_type BIO_ctrl_f;
	PEM_read_bio_PrivateKey_type PEM_read_bio_PrivateKey_f;
	EVP_PKEY_size_type EVP_PKEY_size_f;
	OpenSSL_add_all_digests_type OpenSSL_add_all_digests_f;
	EVP_get_digestbyname_type EVP_get_digestbyname_f;
	EVP_MD_CTX_create_type EVP_MD_CTX_create_f;
	EVP_SignInit_type EVP_SignInit_f;
	EVP_SignUpdate_type EVP_SignUpdate_f;
	EVP_SignFinal_type EVP_SignFinal_f;
	EVP_PKEY_free_type EVP_PKEY_free_f;
	EVP_MD_CTX_destroy_type EVP_MD_CTX_destroy_f;
	ERR_load_crypto_strings_type ERR_load_crypto_strings_f;
	ERR_error_string_type ERR_error_string_f;
	ERR_get_error_type ERR_get_error_f;
	PEM_read_bio_X509_type PEM_read_bio_X509_f;
	d2i_X509_bio_type d2i_X509_bio_f;
	X509_get_pubkey_type X509_get_pubkey_f;
	EVP_VerifyFinal_type EVP_VerifyFinal_f;
	X509_free_type X509_free_f;
        BF_set_key_type BF_set_key_f;
        BF_cbc_encrypt_type BF_cbc_encrypt_f;
  
};

#endif // _DYNLIB_OPENSSL_H

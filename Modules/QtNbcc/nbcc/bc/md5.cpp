//////////////////////////////////////////////////////////////////////////
//файл:         md5.cpp
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Набор классов для вычисления значения проверочной суммы
//////////////////////////////////////////////////////////////////////////

#include "md5.h"

//Сдвиги
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

void MD5Transform(uint32_t state[4], const uint8_t block[64]);

// F, G, H и I элементарные функции MD5
inline uint32_t F(uint32_t x,uint32_t y,uint32_t z) { return ((x & y) | ((~x) & z)); }
inline uint32_t G(uint32_t x,uint32_t y,uint32_t z) { return ((x & z) | (y & (~z))); }
inline uint32_t H(uint32_t x,uint32_t y,uint32_t z) { return (x ^ y ^ z); }
inline uint32_t I(uint32_t x,uint32_t y,uint32_t z) { return (y ^ (x | (~z))); }

// ROTATE_LEFT циклический сдвиг x влево на n бит.
inline uint32_t RotateLeft(uint32_t x,uint16_t n) {
	return ((x << n) | (x >> (32-n)));
}

// FF, GG, HH, и II преобразования для раундов 1, 2, 3 и 4.
inline void FF(uint32_t& a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint16_t s,uint32_t ac) {
	a += F (b, c, d) + x + ac;
	a = RotateLeft(a, s);
	a += b;
}

inline void GG(uint32_t& a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint16_t s,uint32_t ac) {
	a += G (b, c, d) + x + ac;
	a = RotateLeft(a, s);
	a += b;
}

inline void HH(uint32_t& a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint16_t s,uint32_t ac) {
	a += H (b, c, d) + x + ac;
	a = RotateLeft(a, s);
	a += b;
}

inline void II(uint32_t& a,uint32_t b,uint32_t c,uint32_t d,uint32_t x,uint16_t s,uint32_t ac) {
	a += I (b, c, d) + x + ac;
	a = RotateLeft(a, s);
	a += b;
}

// Кодировать вход (uint32_t) в выход (uint8_t). 
// примечание: предполагается, что len кратно 4.
void EncodeDWORD (uint8_t *output, const uint32_t *input, uint16_t size)
{
	uint16_t i, j;
  
	for (i = 0, j = 0; j < size; i++, j += 4) {
		output[j]   = (uint8_t)(input[i] & 0xff);
		output[j+1] = (uint8_t)((input[i] >> 8)  & 0xff);
		output[j+2] = (uint8_t)((input[i] >> 16) & 0xff);
		output[j+3] = (uint8_t)((input[i] >> 24) & 0xff);
	}
}

// Декодировать входа (uint8_t) в выход (uint32_t). 
// примечание: предполагается, что len кратно 4.
void DecodeDWORD (uint32_t *output, const uint8_t *input, uint16_t size)
{
	uint16_t i, j;

	for (i = 0, j = 0; j < size; i++, j += 4)
      output[i] = ((uint32_t)input[j]) |
                  (((uint32_t)input[j+1]) << 8) |
	               (((uint32_t)input[j+2]) << 16) |
                  (((uint32_t)input[j+3]) << 24);
}

// Основное преобразование MD5.
void MD5Transform(uint32_t* state, const uint8_t* block)//[4][64]
{
  uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
  uint32_t x[16];

  DecodeDWORD (x, block, 64);

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478L); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756L); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070dbL); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceeeL); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0fafL); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62aL); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613L); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501L); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8L); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7afL); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1L); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7beL); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122L); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193L); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438eL); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821L); /* 16 */

 /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562L); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340L); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51L); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aaL); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105dL); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453L); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681L); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8L); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6L); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6L); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87L); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14edL); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905L); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8L); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9L); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8aL); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942L); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681L); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122L); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380cL); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44L); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9L); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60L); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70L); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6L); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127faL); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085L); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05L); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039L); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5L); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8L); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665L); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244L); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97L); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7L); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039L); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3L); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92L); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47dL); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1L); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4fL); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0L); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314L); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1L); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82L); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235L); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bbL); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391L); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  
  //Обнулить память
  a = b = c = d = 0xCCCCCCCC;
  memset(x,0,sizeof(x));
}

Md5Hash::Md5Hash(){
  erase();
}

Md5Hash::~Md5Hash(){
  erase();
}

Md5Hash::Md5Hash(const uint8_t *data, uint32_t size){
  create(data,size);
}

//Создание хеша по блоку данных
void Md5Hash::create(const uint8_t *data, uint32_t size){
  Md5Context ctx;
  init(ctx);
  update(ctx,data,size);
  final(ctx);
}

//Стирание хеша
void Md5Hash::erase(){
  memset(this,0,sizeof(Md5Hash));
}

//Проверка на равенство
bool Md5Hash::isEqual(const Md5Hash &hash) const {
  for (int32_t i=0; i<md5HASH_SIZE; ++i)
    if (digest[i]!=hash.digest[i]) return false;
  return true;
}

//Проверка на ноль
bool Md5Hash::isNil(void) const {
  for (int32_t i=0; i<md5HASH_SIZE; ++i){
    if (digest[i]) return false;
  }
  return true;
}

Md5Hash& Md5Hash::operator =(const Md5Hash &hash){
  memcpy(digest,hash.digest,sizeof(uint8_t)*md5HASH_SIZE);
  return *this;
}

bool Md5Hash::operator ==(const Md5Hash &hash) const {
  return isEqual(hash);
}

bool Md5Hash::operator !=(const Md5Hash &hash) const {
  return !isEqual(hash);
}

//Усечение до заданного числа бит
void Md5Hash::trim(uint32_t bitLength){
  uint32_t nByte= bitLength/8;
  uint32_t nBit = bitLength%8;
  //Установить целые байты в 0
  for (int32_t i=nByte; i<md5HASH_SIZE;++i) digest[i]=0;
  //Установить биты в 0, начиная со старшего
  if (nBit)
    for (int32_t j=7; j>=(int32_t)nBit; j++) digest[nByte] &= !(1<<j);
}

//Установка значения хэша
void Md5Hash::set(const uint8_t *data, uint32_t size){
  erase();
  memcpy(digest,data,size);
}

//Преобразование из HEX-строки в хеш-значение
//(длина строки = 32 символа)
bool Md5Hash::fromString(const QString &s){
  if (s.length() < Md5Hash::md5HASH_SIZE*2)
    return false;
  for (uint32_t i=0; i<Md5Hash::md5HASH_SIZE; ++i){
    int32_t j1 = (uint32_t)_tblX16.indexOf(s[2*i+1]);
    int32_t j2 = (uint32_t)_tblX16.indexOf(s[2*i]);
    if (j1<0 || j2<0) return false;
    digest[i] = ((uint8_t)(j2) << 4) | (uint8_t)(j1);
  }
  return true;
}

//Преобразование хеш-значения в HEX-строку
//(длина строки = 32 символа)
QString& Md5Hash::toString(QString &s) const {
  s.resize(Md5Hash::md5HASH_SIZE*2);
  QChar *c = s.data();
  for (uint32_t i=0; i<Md5Hash::md5HASH_SIZE; ++i){
    c[2*i+1]  = _tblX16[digest[i] & 0x0F];
    c[2*i]    = _tblX16[(digest[i]>>4) & 0x0F];
  }
  return s;
}

//Инициализация контекста во время отложенного вычисления
void Md5Hash::init(Md5Context &ctx){
  memset(&ctx,0,sizeof(Md5Context));
  ctx.state[0] = 0x67452301L;
  ctx.state[1] = 0xefcdab89L;
  ctx.state[2] = 0x98badcfeL;
  ctx.state[3] = 0x10325476L;
}

//Обновление хеша во время отложенного вычисления
void Md5Hash::update(Md5Context &ctx, const uint8_t *data, uint32_t size){
  while (size){
    uint32_t bsize = (size>=0x00008000)?0x00008000:size;
    update16(ctx,data,(uint16_t)bsize);
    size -= bsize;
    data += bsize;
  }
}

//Обновление хеша во время отложенного вычисления
void Md5Hash::update16(Md5Context &ctx, const uint8_t *data, uint16_t size){
  uint16_t i;
  uint16_t index;
  uint16_t partLen;

  // Число байт по модулю 64
  index = (uint16_t)((ctx.count[0] >> 3) & 0x3F);

  // Добавить число бит
  if ((ctx.count[0] += ((uint32_t)size << 3)) < ((uint32_t)size << 3))
    ctx.count[1]++;
  ctx.count[1] += ((uint32_t)size >> 29);

  partLen = 64 - index;

  // Преобразовать все, что возможно
  if (size >= partLen) {
    memcpy(&ctx.buffer[index], data, partLen);
    MD5Transform(ctx.state, (uint8_t*)ctx.buffer);

    for (i = partLen; i + 63 < size; i += 64){
      MD5Transform (ctx.state,&data[i]);
    }
    index = 0;
  } else {
    i = 0;
  }
  // Копировать оставшийся кусок в буфер
  memcpy(&ctx.buffer[index], &data[i], size-i);
}

//Завершение вычисления хеша
void Md5Hash::final(Md5Context &ctx){
  static const uint8_t PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };
  uint8_t bits[8];
  uint16_t index, padLen;
  // Сохранить количество бит
  EncodeDWORD (bits, ctx.count, 8);
  // Выровнять на длину 56 по модулю 64
  index	= (uint16_t)((ctx.count[0] >> 3) & 0x3f);
  padLen	= (index < 56) ? (56 - index) : (120 - index);
  update(ctx, PADDING, padLen);
  // Добавить длину сообщения
  update(ctx, bits, 8);
  // Сохранить состояние в подписи
  EncodeDWORD (digest, ctx.state, 16);
  // Обнулить память
  memset(bits,0,sizeof(bits));
  memset(&ctx,0,sizeof(Md5Context));
}

const QString Md5Hash::_tblX16("0123456789ABCDEF");

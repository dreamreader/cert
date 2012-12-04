#ifndef MODULEINFO_H
#define MODULEINFO_H

#include <QList>

#include "../qtnb/Uuid.h"

using namespace Nb;

static const nbUuid UUID_HANDWR_PROVIDER = {0xa9ebf452,0xcf25,0x408c,{0x86,0x47,0x5e,0x3a,0x1d,0x57,0xff,0x4e}};
static const nbUuid UUID_HANDWR_PROCESSOR = {0x702cbb42,0x7367,0x48cf,{0xbe,0xd1,0xd7,0x8d,0x17,0x74,0xba,0x52}};
static const nbUuid UUID_FINGER_PROVIDER = {0x7f4d6e5a,0x4950,0x493c,{0x84,0xf9,0xa6,0xea,0xe3,0x68,0xae,0x9d}};
static const nbUuid UUID_FINGER_PROCESSOR = {0x6b2e0b0b,0xff32,0x4db7,{0xb2,0x2f,0x2e,0x60,0xf6,0xd3,0xc6,0x8c}};
static const nbUuid UUID_NBCC = { 0x9ac0fe89, 0x596e, 0x4414, { 0x94, 0x36, 0x5, 0x12, 0x2b, 0xd9, 0x4d, 0x34 } };

static const nbUuid BIM_TYPE_HANDWR = {0xc42a41d2,0x0118,0x4ef9,{0xb7,0x1c,0xf3,0xb2,0x7d,0x1a,0x98,0xd}};
static const nbUuid BIM_TYPE_FINGER = {0xefad1eb7,0x29f3,0x4c69,{0x86,0x89,0xea,0x2c,0x93,0x8d,0x11,0x19}};

static const char FINGER_NAME[] = "Отпечаток пальца";
static const char FINGER_VENDOR[] = "ЛБНТ";
static const char FINGER_INFO[] = "-";

static const char HANDWR_NAME[] = "Рукописный образ";
static const char HANDWR_VENDOR[] = "ЛБНТ";
static const char HANDWR_INFO[] = "-";

static const char NBCC_NAME[] = "Рукописный образ";
static const char NBCC_VENDOR[] = "ЛБНТ";
static const char NBCC_INFO[] = "-";

struct ModuleInfo
{
    QString name;
    QString vendor;
    QString filename;
    Uuid module_id;
    Uuid module_type;
    Uuid bim_type;
};

typedef QList<ModuleInfo> ModuleInfos;


#endif // MODULEINFO_H

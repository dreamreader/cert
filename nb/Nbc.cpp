#include "Nbc.h"

#pragma pack(1)

Nbc::Nbc()
    : Container (this->factory)
{
    BlockContHead headBlock(Uuid::nil, 0);
    this->push(headBlock);
    update();

    Block block;
    block.type(nbBT_CONTAINER_ID);
    _id = (Uuid)Uuid::createUuid ();
    block.push(_id);
    this->push(block);
    update();
}

Nbc::~Nbc()
{

}

bool Nbc::update()
{
    BlockContHead* header;
    header = (BlockContHead *)(this->at(0));
    header->BlockContHead::update(all(), 0, this->count());
    return true;
}

bool Nbc::readFrom(QString path)
{
    QFile file (path);
    if (!file.open(QFile::ReadOnly)) {
        throw (QString::fromUtf8("Невозможно открыть биометрический контейнер"));
    }
    QDataStream in (&file);

    if (this->read(in) <= 0) {
        throw (QString::fromUtf8("Невозможно прочитать биометрический контейнер"));
    }

    return false;
}

bool Nbc::writeTo(QString path)
{
    QFile file (path);
    file.open(QFile::ReadWrite);
    QDataStream out (&file);
    int num = this->write(out);
    if (num <= 0) {
        throw (QString::fromUtf8("Невозможно записать биометрический контейнер"));
    }


    return true;
}

bool Nbc::setScheme (Scheme scheme)
{
    Block block;
    block.type(BtCScheme);
    block.push (scheme);
    this->push (block);
    this->update ();
    return true;
}

Scheme Nbc::getScheme ()
{
    Scheme::Factory factory;
    Scheme result (factory);
    Blocks blocks;
    int pos = 0;
    if (findBlocks (blocks, BtCScheme)) {
        blocks[0]->pop (pos, result);
    }
    return result;
}

QString Nbc::getUsername()
{
    QString result;
    Blocks blocks;
    Data raw_string;
    int pos = 0;
    if (findBlocks (blocks, nbBT_USER_NAME)) {
        blocks[0]->pop (pos, raw_string, true);
        result = raw_string.toString();
    }
    return result;
}

bool Nbc::setUsername(QString name)
{
    Block block;
    block.type(nbBT_USER_NAME);
    int32_t size = 0;
    Data raw_string;
    raw_string.fromString(name, size, true);
    block.push (raw_string, true);
    this->push (block);
    this->update ();
    return true;
}

QString Nbc::getName()
{
    QString result;
    Blocks blocks;
    Data raw_string;
    int pos = 0;
    if (findBlocks (blocks, nbBT_NBC_NAME)) {
        blocks[0]->pop (pos, raw_string, true);
        result = raw_string.toString();
    }
    return result;
}

bool Nbc::setName(QString name)
{
    Block block;
    block.type(nbBT_NBC_NAME);
    int32_t size = 0;

    Data raw_string;
    raw_string.fromString(name, size, true);
    block.push (raw_string, true);
    this->push (block);
    this->update ();
    return true;
}


Uuids Nbc::getSlotInfo (SlotId slot)
{
    Uuids result;
    Blocks blocks;
    Uuid type_id, provider_id, processor_id;
    int32_t n, slot_id;
    if (!findBlocks (blocks, nbBT_SLOT_ID)) {
        return result;
    }

    for (int i = 0; i != blocks.size(); i++) {
        int32_t pos = 0;
        blocks[i]->pop (pos, type_id);
        blocks[i]->pop (pos, provider_id);
        blocks[i]->pop (pos, processor_id);
        blocks[i]->pop (pos, n);
        blocks[i]->pop (pos, slot_id);

        if (slot_id == (int32_t)slot) {
            result.push_back (provider_id);
            result.push_back (processor_id);
            break;
        }
    }
    return result;
}

bool Nbc::setSlotInfo (SlotId slot, Uuids uuids)
{
    Block block;
    block.type(nbBT_SLOT_ID);
    block.push (nbUUID_NIL);
    block.push (uuids[0]);
    block.push (uuids[1]);
    block.push ((int32_t)0);
    block.push ((int32_t)slot);
    this->push (block);
    this->update ();
    return true;
}

bool Nbc::findBlocks (Blocks &blocks, int type)
{
    bool found = false;
    for (int i = 0; i != _blocks.size(); i++) {
        if (_blocks[i]->type() == type) {
            blocks.push_back(_blocks[i]);
            found = true;
        }
    }
    return found;
}

bool Nbc::readFrom(Data data)
{
    QByteArray buf;
    QDataStream stream(&buf, QIODevice::ReadWrite);
    data.write(stream, false);
    stream.device()->seek(0);
    this->read(stream, -1);
    return true;
}

bool Nbc::writeTo(Data &data)
{
    QByteArray buf;
    QDataStream stream(&buf, QIODevice::ReadWrite);
    this->write (stream, true);
    stream.device()->seek(0);
    data.read (stream, true);
    return true;
}

size_t Nbc::getSchemeNumber ()
{
    Blocks blocks;
    int32_t result;
    int pos = 0;
    if (!findBlocks (blocks, nbBT_SCHEME_NUMBER)) {
        return -1;
    }
    blocks[0]->pop (pos, result);
    return result;
}

bool Nbc::setSchemeNumber (size_t number)
{
    Block block;
    block.type(nbBT_SCHEME_NUMBER);
    block.push ((int32_t)number);
    this->push (block);
    this->update ();
    return true;
}

bool Nbc::getBimPattern (SlotId sid, Uuid bimType, Matrix &pattern)
{
    Uuid id;
    Matrix result;
    Blocks blocks;
    SlotId sid2;

    if (!findBlocks (blocks, nbBT_PATTERN)) {
        return false;
    }

    for (int i = 0; i != blocks.size(); i++) {
        int32_t pos = 0;
        blocks[i]->pop (pos, (int16_t&) sid2);
        if (sid != sid2) continue;
        blocks[i]->pop (pos, id);
        if (id != bimType) continue;
        blocks[i]->pop (pos, result);
        pattern.remeta(result.meta(0));
        pattern.resize(result.ncols());
        pattern.copy (result);
        return true;
    }
    return false;
}

bool Nbc::setBimPattern (SlotId sid, Uuid bimType, Matrix pattern)
{
    Block block;
    block.type(nbBT_PATTERN);
    block.push((int16_t)sid);
    block.push (bimType);
    block.push (pattern);
    this->push (block);
    this->update ();
    return true;
}

bool Nbc::getCode (Matrix &code)
{
    Blocks blocks;
    Matrix result;
    if (!findBlocks (blocks, nbBT_CODE)) {
        return false;
    }
    int pos = 0;
    blocks[0]->pop (pos, result);

    code.remeta(result.meta(0));
    code.resize(result.ncols());
    code.copy(result);
    return true;
}

bool Nbc::setCode (Matrix &code)
{
    Block block;
    block.type(nbBT_CODE);
    block.push (code);
    this->push (block);
    this->update ();

    return true;
}

Uuid Nbc::getId ()
{
    Blocks blocks;
    Uuid result;
    int pos = 0;
    if (findBlocks (blocks, nbBT_CONTAINER_ID)) {
        blocks[0]->pop (pos, result);
    }
    return result;
}

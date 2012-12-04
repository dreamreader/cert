#include "SchemeConstructor.h"

Nbc SchemeConstructor::construct (int index)
{
    Scheme::Factory factory;
    Scheme s(factory);
    Nbc nbc;

    Uuids uidsHandwr;
    uidsHandwr.push_back(UUID_HANDWR_PROVIDER);
    uidsHandwr.push_back(UUID_HANDWR_PROCESSOR);
    Uuids uidsFinger;
    uidsFinger.push_back(UUID_FINGER_PROVIDER);
    uidsFinger.push_back(UUID_FINGER_PROCESSOR);


      Meta metaHw(480,MfR32,MtC);
      Meta metaFp(256,MfR32,MtC);
      Meta metaKey(256,MfI1,MtDO);

      s.free();
      switch (index){
      case 0: //Рукописный ввод
        s.push(StIn,        metaHw)
         .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)
         .out(1);
        nbc.setSlotInfo(s.idOf(0), uidsHandwr);
        break;
      case 1: //Отпечаток пальца
        s.push(StIn,        metaFp)
         .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)
         .out(1);
        nbc.setSlotInfo(s.idOf(0), uidsFinger);
        break;
      case 2: //Рукописный ввод и отпечаток
        s.push(StIn,        metaHw) //0
         .push(StIn,        metaFp) //1
         .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)  //2
         .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<1)  //3
         .push(BtConnector, metaKey,  SlotTypes()<<StIn<<StIn,SlotNums()<<2<<3,BfBackwardTrain|BfGenerated) //4
         .out(4);
        nbc.setSlotInfo(s.idOf(0), uidsHandwr);
        nbc.setSlotInfo(s.idOf(1), uidsFinger);
        break;
      case 3: //Рукописный ввод или отпечаток
        s.push(StIn,        metaHw) //0
         .push(StIn,        metaFp) //1
         .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)  //2
         .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<1)  //3
         .push(BtCondition, metaKey,  SlotTypes()<<StIn<<StIn,SlotNums()<<2<<3,BfBackwardTrain) //4
         .out(4);
        nbc.setSlotInfo(s.idOf(0), uidsHandwr);
        nbc.setSlotInfo(s.idOf(1), uidsFinger);
        break;
      case 4: //Рукописный ввод и рукописный ввод
        s.push(StIn,        metaHw) //0
         .push(StIn,        metaHw) //1
         .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)  //2
         .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<1)  //3
         .push(BtConnector, metaKey,  SlotTypes()<<StIn<<StIn,SlotNums()<<2<<3,BfBackwardTrain|BfGenerated) //4
         .out(4);
        nbc.setSlotInfo(s.idOf(0), uidsHandwr);
        nbc.setSlotInfo(s.idOf(1), uidsHandwr);
        break;
      case 5: //Отпечаток и отпечаток
        s.push(StIn,        metaFp) //0
         .push(StIn,        metaFp) //1
         .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)  //2
         .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<1)  //3
         .push(BtConnector, metaKey,  SlotTypes()<<StIn<<StIn,SlotNums()<<2<<3,BfBackwardTrain|BfGenerated) //4
         .out(4);
        nbc.setSlotInfo(s.idOf(0), uidsFinger);
        nbc.setSlotInfo(s.idOf(1), uidsFinger);
        break;
      case 6: //Рукописный ввод или рукописный ввод
          s.push(StIn,        metaHw) //0
           .push(StIn,        metaHw) //1
           .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)  //2
           .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<1)  //3
           .push(BtCondition, metaKey,  SlotTypes()<<StIn<<StIn,SlotNums()<<2<<3,BfBackwardTrain) //4
           .out(4);
          nbc.setSlotInfo(s.idOf(0), uidsHandwr);
          nbc.setSlotInfo(s.idOf(1), uidsHandwr);
          break;
      case 7: //Отпечаток или отпечаток
          s.push(StIn,        metaFp) //0
           .push(StIn,        metaFp) //1
           .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)  //2
           .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<1)  //3
           .push(BtCondition, metaKey,  SlotTypes()<<StIn<<StIn,SlotNums()<<2<<3,BfBackwardTrain) //4
           .out(4);
          nbc.setSlotInfo(s.idOf(0), uidsFinger);
          nbc.setSlotInfo(s.idOf(1), uidsFinger);
          break;
      default:
        throw (QString::fromUtf8("Ошибка формирования схемы преобразования"));
       }
      nbc.setScheme (s);
      nbc.setSchemeNumber(index);
      return nbc;
}

#ifndef DCONREADER_H
#define DCONREADER_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <QSerialPort>
#include "autostopthread.h"
#include "src_libmodbus/modbus.h"
#include "modbusreader.h"

//#define COM_PORT "COM1"
//#define DCON_BAUD_RATE 38400  //hardcoded
//baudrate 9600 =30-31 requests per second

#define PARITY 'N'    //'N' even paruty for new registrator
#define BYTESIZE 8
#define STOPBITS 1


//7017F DCON

//#define ICP_7017_ADDRESS 1     //all hardcoded
//#define ICP_7018_NUM_CHANNELS 8


//=============================================================================
//declared at modbusreader.h
/*
enum ValueStatus
{
    ValueOk=0,
    ValueErrorRead,
    ValueNotReading
};

Q_DECLARE_METATYPE(ValueStatus)
*/
//=============================================================================

class DconReader : public AutoStopThread
{
    Q_OBJECT

public:
    DconReader()
    {
        //ctx = NULL;
        //sec_to = 1;
        //usec_to = 0;
        numReads=0;
        numErrorReads=0;



        connect(this,SIGNAL(finished()),this,SLOT(Release()));
    }

    virtual ~DconReader() {}

    bool Init(QString serialport="", int i7017address=1);
    bool Connect();
    bool TestConnection(QString serPort, int i7016Addr=1);



    void StartPoll();
    void StopPoll();
    void SetComPort(QString serPort);

private:
    QString serialPort;
    QSerialPort port;

    virtual void run();



    //modbus_t *ctx;
    //uint32_t sec_to;
    //uint32_t usec_to;

    //uint16_t regTrm251_1[200];
    //uint16_t regTrm251_2[200];
    //uint16_t regOWEN_AI[200];
    //uint16_t regOWEN_2AI[200];
    //uint16_t regICP_7018[200];

    uint numReads;
    uint numErrorReads;


private slots:
    void Release();

signals:
    /*
    void readTrm251_1(float newValue,float newStep, ValueStatus newValueStatus);
    void readTrm251_2(float newValue,float newStep, ValueStatus newValueStatus);
    void readOwen2ai(float newValue1,float newValue2, ValueStatus newValueStatus);
*/
    //void readTrm251_1(double newValue, ValueStatus newValueStatus);
    //void readTrm251_2(double newValue, ValueStatus newValueStatus);
    //void readOWEN_AI_ch1(double newValue, ValueStatus newValueStatus);
    //void readOWEN_AI_ch2(double newValue, ValueStatus newValueStatus);

    //void readOWEN_2AI_ch1(double newValue, ValueStatus newValueStatus);
    //void readOWEN_2AI_ch2(double newValue, ValueStatus newValueStatus);

    //void readOWEN_AI_ch3(double newValue, ValueStatus newValueStatus);
    //void readOWEN_AI_ch4(double newValue, ValueStatus newValueStatus);
    //void readOWEN_AI_ch5(double newValue, ValueStatus newValueStatus);
    //void readOWEN_AI_ch6(double newValue, ValueStatus newValueStatus);
    //void readOWEN_AI_ch7(double newValue, ValueStatus newValueStatus);
    //void readOWEN_AI_ch8(double newValue, ValueStatus newValueStatus);

    void readICP_7017_ch1(double newValue, ValueStatus newValueStatus);
    void readICP_7017_ch2(double newValue, ValueStatus newValueStatus);
    void readICP_7017_ch3(double newValue, ValueStatus newValueStatus);
    void readICP_7017_ch4(double newValue, ValueStatus newValueStatus);
    void readICP_7017_ch5(double newValue, ValueStatus newValueStatus);
    void readICP_7017_ch6(double newValue, ValueStatus newValueStatus);
    void readICP_7017_ch7(double newValue, ValueStatus newValueStatus);
    void readICP_7017_ch8(double newValue, ValueStatus newValueStatus);
};

#endif // DCONREADER_H

#ifndef DCONTHREADREADER_H
#define DCONTHREADREADER_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include "autostopthread.h"
//#include <QtSerialPort/QtSerialPort>
#include <QSerialPort>
//#define COM_PORT "COM1"
#define BAUD_RATE 115200
//baudrate 9600 =30-31 requests per second

#define PARITY 'N'
#define BYTESIZE 8
#define STOPBITS 1

//адрес для чтения значения мВ с преобразователя, DCON 1 value
//Этот адрес не менять, в программе по ДКОНу жестко прописано адрес 1, скорость 115200
#define M7016_ADDRESS 1


//=============================================================================

struct CalPoint
{
    CalPoint(){}
    CalPoint(double akey, double aval): key(akey),val(aval) {}
    bool operator <(const CalPoint &other) const {return (key < other.key);}
    double key;
    double val;
};

//=============================================================================

class DconThreadReader : public AutoStopThread
{
    Q_OBJECT

public:
    DconThreadReader()
    {
        numReads=0;
        numErrorReads=0;
        zero_offset=0.0;
        connect(this,SIGNAL(finished()),this,SLOT(Release()));
    }

    virtual ~DconThreadReader();

    bool Init(QString serPort="", int i7016Addr=1, int avgPnts=0, double offset=0.0);

    bool Connect();
    bool TestConnection(QString serPort, int i7016Addr=1);
    void StartPoll();
    void StopPoll();
    void SetComPort(QString serPort);
    bool Calibrate(double x, double &y);
    double Average(double newValue);
    uint numReads;
    uint numErrorReads;

    void SetVectorCalPoints(QVector<CalPoint> calPnts);
    QVector<CalPoint> GetVectorCalPoints();

    double ApplyOffset(double calibratedValue);
    void SetZeroOffset();
    void SetInitZeroOffset(double offset);

    double zero_offset;

private:
    int i7015Address;
    QString serialPort;
    QSerialPort port;

    virtual void run();


    QVector<CalPoint> calPoints;
    float value_calibrated_offseted;

    int avgPoints;
    QVector<double> avg_vect;


    int rc;

private slots:
    void Release();

signals:
        void valueReaded(float newValue);//, float newValue2);


};

#endif // DCONTHREADREADER_H

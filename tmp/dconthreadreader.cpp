#include "dconthreadreader.h"


//=======================================================================
DconThreadReader::~DconThreadReader()
{
    Release();
}

//=======================================================================
double DconThreadReader::Average(double newValue)
{
    avg_vect.push_back(newValue);
    if (avg_vect.size() > avgPoints) avg_vect.pop_front();

    double summ=0;
    for(int i=0;i<avg_vect.size();++i)
    {
        summ+=avg_vect[i];
    }
    if (avg_vect.size()>0) return summ / avg_vect.size();

    return 0;
}

//=======================================================================
bool DconThreadReader::Init(QString serPort, int i7016Addr, int avgPnts, double offset)
{
    if (serPort!="") serialPort=serPort;
    if (i7016Addr!=0) i7015Address=i7016Addr;
    if (avgPnts!=0) avgPoints=avgPnts;

    zero_offset=offset;

    avg_vect.clear();

    stopping=false;


return true;
}
//=======================================================================
void DconThreadReader::SetComPort(QString serPort)
{
    if (serPort!="") serialPort=serPort;
}
//=======================================================================

void DconThreadReader::SetVectorCalPoints(QVector<CalPoint> calPnts)
{
    calPoints=calPnts;
}
//=======================================================================
QVector<CalPoint> DconThreadReader::GetVectorCalPoints()
{
    return calPoints;
}

//=======================================================================
bool DconThreadReader::TestConnection(QString serPort, int i7016Addr)
{

    QSerialPort serial;

    qDebug() << "Open Com Port:"+serPort;

    serial.setPortName(serPort);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.open(QIODevice::ReadWrite);
    //serial.


    bool readed_ok=false;

    for(int k=0;k<3;++k) //3 попытки
    {
        // #AAN(CR)   (CR)==0x0D=='\r'
        char query[256]="#01\r";//"#010\r";  <<-- для одноканального  модуля номер канала не используется
        char ans[256];
        int cr='\r';
        //qDebug() << query << "  len:" << strlen(query) << "    " << cr;
        //qDebug() <<
        serial.write(query,strlen(query));
        serial.waitForBytesWritten(500);

        //Формат ответа = >(7char)(cr) == 9 bytes
        //>+027.84(cr)
        for (int i=0;i<10;++i)
        {
            serial.waitForReadyRead(100);
            if (serial.bytesAvailable()==9) break;
        }

        //qDebug() << "\nanslen:" << serial.bytesAvailable();

        uint bytes_readed=0;
        if (serial.bytesAvailable()>0)
        {
            bytes_readed=serial.read(ans,serial.bytesAvailable());
        }

        ans[bytes_readed]='\0';

        //qDebug() << ans;

        if (bytes_readed==9)
        {
            readed_ok=true;
            break;
        }

    }


    serial.close();


    return readed_ok;

}

//=======================================================================
bool DconThreadReader::Connect()
{
    port.setPortName(serialPort);



    port.setBaudRate(QSerialPort::Baud115200);
    port.setDataBits(QSerialPort::Data8);
    port.setParity(QSerialPort::NoParity);
    port.setStopBits(QSerialPort::OneStop);
    port.setFlowControl(QSerialPort::NoFlowControl);
    return port.open(QIODevice::ReadWrite);
}
//=======================================================================
void DconThreadReader::StartPoll()
{
    stopping=false;
    start();
}
//=======================================================================
void DconThreadReader::StopPoll()
{
    stopping=true;
    wait();
}
//=======================================================================
void DconThreadReader::run()
{

   int errors=0;


    Connect();



    for(;;)
    {
        // #AA(CR)   (CR)==0x0D=='\r'

       //чтение одного канала
        char query[256]="#01\r";//"#010\r";  <<-- для одноканального  модуля номер канала не используется
        char ans[256];
        int cr='\r';
        //qDebug() << query << "  len:" << strlen(query) << "    " << cr;
        port.write(query,strlen(query));
        port.waitForBytesWritten(500);

        //msleep(200);
        for (int i=0;i<10;++i)
        {
            port.waitForReadyRead(100);
            if (port.bytesAvailable()==9) break;
        }
        //serial.waitForReadyRead(1000);

        //qDebug() << "\nanslen:" << port.bytesAvailable();

        uint bytes_readed=0;
        if (port.bytesAvailable()==9)
        {
            bytes_readed=port.read(ans,port.bytesAvailable());
        }

        //чтение каналов конец

        ans[bytes_readed]='\0';

        //qDebug() << ans;

        if (bytes_readed==9)
        {
            numReads++;
            //printf("Data received is : %d %d \n\n\n",reg[0],reg[1]);
            float readed_value;
            //float readed_value2;
            char tempvalue[16];
            //char tempvalue2[16];
            strncpy(tempvalue,&ans[1],7);
            tempvalue[7]='\0';
            //strncpy(tempvalue2,&ans[8],7);
            //tempvalue2[7]='\0';
            //qDebug() << "\ndcon: " << tempvalue << "  " << tempvalue2;
            readed_value=QString(tempvalue).toFloat();
            //readed_value2=QString(tempvalue2).toFloat();

            //qDebug() << "\nfloat: " << readed_value << "  " << readed_value2;
            emit valueReaded(readed_value); //, readed_value2);
            errors=0;
        }
        else
        {
            numErrorReads++;
            errors++;
            //emit valueReaded(0.0);//,0.0);
            if (errors>5)
            {
                msleep(200);
                Release();
                msleep(200);
                Connect();
                msleep(200);
                errors=0;
            }
        }



        /*

        //чтение более одного канала
        char query[256]="#01\r";//\r\n"};
        char ans[256];
        int cr='\r';
        //qDebug() << query << "  len:" << strlen(query) << "    " << cr;
        //qDebug() <<
        port.write(query,strlen(query));
        port.waitForBytesWritten(500);

        //msleep(200);
        //Формат ответа = > (7char)*6channels (cr) == 44 bytes
        //>+027.84+9999.9+9999.9+9999.9+9999.9+9999.9(cr)
        for (int i=0;i<10;++i)
        {
            port.waitForReadyRead(100);
            if (port.bytesAvailable()==44) break;
        }
        //serial.waitForReadyRead(1000);

        //qDebug() << "\nanslen:" << port.bytesAvailable();

        uint bytes_readed=0;
        if (port.bytesAvailable()==44)
        {
            bytes_readed=port.read(ans,port.bytesAvailable());
        }

        //чтение каналов конец

        ans[bytes_readed]='\0';

        //qDebug() << ans;


        if (bytes_readed==44)
        {
            numReads++;
            //printf("Data received is : %d %d \n\n\n",reg[0],reg[1]);
            float readed_value;
            float readed_value2;
            char tempvalue[16];
            char tempvalue2[16];
            strncpy(tempvalue,&ans[1],7);
            tempvalue[7]='\0';
            strncpy(tempvalue2,&ans[8],7);
            tempvalue2[7]='\0';
            //qDebug() << "\ndcon: " << tempvalue << "  " << tempvalue2;
            readed_value=QString(tempvalue).toFloat();
            readed_value2=QString(tempvalue2).toFloat();

            //qDebug() << "\nfloat: " << readed_value << "  " << readed_value2;
            emit valueReaded(readed_value, readed_value2);
            errors=0;
        }
        else
        {
            numErrorReads++;
            errors++;
            //emit valueReaded(0.0,0.0);
            if (errors>5)
            {
                msleep(200);
                Release();
                msleep(200);
                Connect();
                msleep(200);
                errors=0;
            }
        }
        */

        msleep(85); //Registrator needs small delay between polls, 1 ms is OK too
                   //чтоб не тормозил ГУИ



        if (CheckThreadStop()) return;
    }

}
//=======================================================================

bool DconThreadReader::Calibrate(double x, double &y)
{

    if (calPoints.size()<2) return false;

    double x1,x2,y1,y2;

    if (x <= calPoints[0].key)
    {
        x1=calPoints[0].key;
        x2=calPoints[1].key;
        y1=calPoints[0].val;
        y2=calPoints[1].val;
        //   x-x1
        //y=------- * (y2-y1) + y1
        //   x2-x1
        y=(x-x1)*(y2-y1)/(x2-x1) + y1;
        return true;
    }

    if (x > calPoints[calPoints.size()-1].key)
    {
        x1=calPoints[calPoints.size()-2].key;
        x2=calPoints[calPoints.size()-1].key;
        y1=calPoints[calPoints.size()-2].val;
        y2=calPoints[calPoints.size()-1].val;
        //   x-x1
        //y=------- * (y2-y1) + y1
        //   x2-x1
        y=(x-x1)*(y2-y1)/(x2-x1) + y1;
        return true;
    }

    for (int i=0;i<calPoints.size()-1;++i)
    {
        if (x>calPoints[i].key && x<=calPoints[i+1].key)
        {
            x1=calPoints[i].key;
            x2=calPoints[i+1].key;
            y1=calPoints[i].val;
            y2=calPoints[i+1].val;
            //   x-x1
            //y=------- * (y2-y1) + y1
            //   x2-x1
            y=(x-x1)*(y2-y1)/(x2-x1) + y1;
            return true;
        }

    }

    return false;
}

//=======================================================================

double DconThreadReader::ApplyOffset(double calibratedValue)
{
    value_calibrated_offseted=calibratedValue-zero_offset;
    return value_calibrated_offseted;
}
//=======================================================================
void DconThreadReader::SetZeroOffset()
{
    zero_offset=value_calibrated_offseted+zero_offset;
}
//=======================================================================
void DconThreadReader::SetInitZeroOffset(double offset)
{
    zero_offset=offset;
}

//=======================================================================
void DconThreadReader::Release()
{
    port.close();
   // qDebug() << "Release POrt";
    /* Close the connection */

}

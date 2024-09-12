#include "dconreader.h"

//=======================================================================
bool DconReader::Init(QString serialport, int i7017address)
{
    if (serialport!="") serialPort=serialport;
    stopping=false;
    return true;
}
//======================================================================
void DconReader::SetComPort(QString serPort)
{
    if (serPort!="") serialPort=serPort;
}
//=======================================================================

//=======================================================================
void DconReader::StartPoll()
{
    stopping=false;
    qDebug() << "starting dcon poll";
    start();
}
//=======================================================================
void DconReader::StopPoll()
{
    stopping=true;
    wait();
}
//=======================================================================

//=======================================================================
bool DconReader::Connect()
{

    qDebug() << "connecting to " << serialPort << " ...";
    port.setPortName(serialPort);



    port.setBaudRate(QSerialPort::Baud38400);
    port.setDataBits(QSerialPort::Data8);
    port.setParity(QSerialPort::NoParity);
    port.setStopBits(QSerialPort::OneStop);
    port.setFlowControl(QSerialPort::NoFlowControl);
    return port.open(QIODevice::ReadWrite);
}
//=======================================================================
bool DconReader::TestConnection(QString serPort, int i7016Addr)
{

    QSerialPort serial;

    qDebug() << "Open Com Port:"+serPort;

    serial.setPortName(serPort);
    serial.setBaudRate(QSerialPort::Baud38400);
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

//==========================================================================
void DconReader::run()
{

    int errorsICP_7017=0;


    Connect();



    for(;;)
    {
 /*
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

*/



        //чтение более одного канала
        char query[256]="#01\r";//\r\n"};
        char ans[256];
        int cr='\r';
        //qDebug() << query << "  len:" << strlen(query) << "    " << cr;
        //qDebug() <<
        //port.flush();
        port.clear();
        port.write(query,strlen(query));
        port.waitForBytesWritten(300);

        msleep(150);
        //Формат ответа = > (7char)*6channels (cr) == 44 bytes
         //Формат ответа = > (7char)*8channels (cr) == 58 bytes

        //>+027.84+9999.9+9999.9+9999.9+9999.9+9999.9(cr)
        for (int i=0;i<10;++i)
        {
            port.waitForReadyRead(100);
            if (port.bytesAvailable()==58) break;
        }
        //serial.waitForReadyRead(1000);

        //qDebug() << "\nanslen:" << port.bytesAvailable();

        uint bytes_readed=0;
        if (port.bytesAvailable()==58)
        {
            bytes_readed=port.read(ans,port.bytesAvailable());
        }

        //чтение каналов конец

        ans[bytes_readed]='\0';

        //qDebug() << ans;


        if (bytes_readed==58)
        {
            numReads++;
            //printf("Data received is : %d %d \n\n\n",reg[0],reg[1]);
            float readed_icp_7017_channels[8];
            char tempvalue[16];

            for(int i=0;i<8;++i)
            {
                strncpy(tempvalue,&ans[1+i*7],7);
                tempvalue[7]='\0';
                //strncpy(tempvalue2,&ans[8],7);
                //tempvalue2[7]='\0';
                qDebug() << "\ndcon: ch" << i << "=" << tempvalue;
                readed_icp_7017_channels[i]=QString(tempvalue).toFloat();
                qDebug() << "\nfloat: ch" << i << "=" << readed_icp_7017_channels[i];
            }

            if (!qIsNaN(readed_icp_7017_channels[0])) emit readICP_7017_ch1(readed_icp_7017_channels[0], ValueStatus::ValueOk);
            if (!qIsNaN(readed_icp_7017_channels[1])) emit readICP_7017_ch2(readed_icp_7017_channels[1], ValueStatus::ValueOk);
            if (!qIsNaN(readed_icp_7017_channels[2])) emit readICP_7017_ch3(readed_icp_7017_channels[2], ValueStatus::ValueOk);
            if (!qIsNaN(readed_icp_7017_channels[3])) emit readICP_7017_ch4(readed_icp_7017_channels[3], ValueStatus::ValueOk);
            if (!qIsNaN(readed_icp_7017_channels[4])) emit readICP_7017_ch5(readed_icp_7017_channels[4], ValueStatus::ValueOk);
            if (!qIsNaN(readed_icp_7017_channels[5])) emit readICP_7017_ch6(readed_icp_7017_channels[5], ValueStatus::ValueOk);
            if (!qIsNaN(readed_icp_7017_channels[6])) emit readICP_7017_ch7(readed_icp_7017_channels[6], ValueStatus::ValueOk);
            if (!qIsNaN(readed_icp_7017_channels[7])) emit readICP_7017_ch8(readed_icp_7017_channels[7], ValueStatus::ValueOk);
            errorsICP_7017=0;
        }
        else
        {
            numErrorReads++;
            errorsICP_7017++;
            //emit valueReaded(0.0,0.0);
            if (errorsICP_7017>5)
            {
                emit readICP_7017_ch1(qQNaN(), ValueStatus::ValueErrorRead);
                emit readICP_7017_ch2(qQNaN(), ValueStatus::ValueErrorRead);
                emit readICP_7017_ch3(qQNaN(), ValueStatus::ValueErrorRead);
                emit readICP_7017_ch4(qQNaN(), ValueStatus::ValueErrorRead);
                emit readICP_7017_ch5(qQNaN(), ValueStatus::ValueErrorRead);
                emit readICP_7017_ch6(qQNaN(), ValueStatus::ValueErrorRead);
                emit readICP_7017_ch7(qQNaN(), ValueStatus::ValueErrorRead);
                emit readICP_7017_ch8(qQNaN(), ValueStatus::ValueErrorRead);

                msleep(200);
                Release();
                msleep(200);
                Connect();
                msleep(200);
                errorsICP_7017=0;
            }
        }



        msleep(200);  //sleep должен біть после всех emit


        if (CheckThreadStop())
        {
            qDebug() << "stopping dcon reader thread";
            Release();
            return;
        }
    }
}
//=======================================================================

//=======================================================================
void DconReader::Release()
{
    /* Close the connection */
    if (port.isOpen())
    {
        port.close();
    }
}

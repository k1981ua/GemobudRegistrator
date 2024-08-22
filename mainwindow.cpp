#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_dialogconfig.h"
#include "fmdlg.h"
#include "modbusreader.h"
#include "dconreader.h"

#include <QPainter>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QTextTable>


//ModbusReader mbReader;
DconReader dconReader;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<ValueStatus>();

    //ui->buttonReset->setVisible(false);
    ui->buttonStartStop->setVisible(false);

    QDir appDir(qApp->applicationDirPath());
    appDir.mkdir("reports");
    appDir.mkdir("png");
    appDir.mkdir("csv");


    //connect(ui->lineEditOperatorPoint_1,SIGNAL(textEdited(QString)),this,SLOT(ValueChanged(QString)));
    //connect(ui->lineEditOperatorPoint_2,SIGNAL(textEdited(QString)),this,SLOT(ValueChanged(QString)));

    //double operatorInputMin=Y_MOVEMENT_RANGE_MIN;
    //double operatorInputMax=Y_MOVEMENT_RANGE_MAX;

    //temperature == channel

    temperature_1.SetConfigFileName(qApp->applicationDirPath()+"/"+"channel_1.ch");
    temperature_2.SetConfigFileName(qApp->applicationDirPath()+"/"+"channel_2.ch");
    temperature_3.SetConfigFileName(qApp->applicationDirPath()+"/"+"channel_3.ch");
    temperature_4.SetConfigFileName(qApp->applicationDirPath()+"/"+"channel_4.ch");
    temperature_5.SetConfigFileName(qApp->applicationDirPath()+"/"+"channel_5.ch");
    temperature_6.SetConfigFileName(qApp->applicationDirPath()+"/"+"channel_6.ch");
    temperature_7.SetConfigFileName(qApp->applicationDirPath()+"/"+"channel_7.ch");
    temperature_8.SetConfigFileName(qApp->applicationDirPath()+"/"+"channel_8.ch");

    temperature_1.LoadChannelConfigFile();
    temperature_2.LoadChannelConfigFile();
    temperature_3.LoadChannelConfigFile();
    temperature_4.LoadChannelConfigFile();
    temperature_5.LoadChannelConfigFile();
    temperature_6.LoadChannelConfigFile();
    temperature_7.LoadChannelConfigFile();
    temperature_8.LoadChannelConfigFile();

    if (qApp->arguments().count()>1)
    {
        iniFile=qApp->arguments().last();
    }
    else
    {
       iniFile="main.ini";
    }

    LoadIniFile(qApp->applicationDirPath()+"/"+iniFile);

    connect(&dconReader, &DconReader::readICP_7017_ch1, &temperature_1, &AnalogInputChannel::RawValueReaded);
    connect(&dconReader, &DconReader::readICP_7017_ch2, &temperature_2, &AnalogInputChannel::RawValueReaded);
    connect(&dconReader, &DconReader::readICP_7017_ch3, &temperature_3, &AnalogInputChannel::RawValueReaded);
    connect(&dconReader, &DconReader::readICP_7017_ch4, &temperature_4, &AnalogInputChannel::RawValueReaded);
    connect(&dconReader, &DconReader::readICP_7017_ch5, &temperature_5, &AnalogInputChannel::RawValueReaded);
    connect(&dconReader, &DconReader::readICP_7017_ch6, &temperature_6, &AnalogInputChannel::RawValueReaded);
    connect(&dconReader, &DconReader::readICP_7017_ch7, &temperature_7, &AnalogInputChannel::RawValueReaded);
    connect(&dconReader, &DconReader::readICP_7017_ch8, &temperature_8, &AnalogInputChannel::RawValueReaded);


    qDebug() << "port:" << comPort;

    //if (QFile(comPort).exists())
    {
        qDebug() << "port exist";
        dconReader.Init(comPort);
        dconReader.StartPoll();
    }



    //dialogConfig.SetModbusReader(&mbReader);

    hashAnalogInputChannels[QString("channel_1") + "(" + temperature_1.GetChName()+")"]=&temperature_1;
    hashAnalogInputChannels[QString("channel_2") + "(" + temperature_2.GetChName()+")"]=&temperature_2;
    hashAnalogInputChannels[QString("channel_3") + "(" + temperature_3.GetChName()+")"]=&temperature_3;
    hashAnalogInputChannels[QString("channel_4") + "(" + temperature_4.GetChName()+")"]=&temperature_4;
    hashAnalogInputChannels[QString("channel_5") + "(" + temperature_5.GetChName()+")"]=&temperature_5;
    hashAnalogInputChannels[QString("channel_6") + "(" + temperature_6.GetChName()+")"]=&temperature_6;
    hashAnalogInputChannels[QString("channel_7") + "(" + temperature_7.GetChName()+")"]=&temperature_7;
    hashAnalogInputChannels[QString("channel_8") + "(" + temperature_8.GetChName()+")"]=&temperature_8;

    connect(ui->buttonExit,     SIGNAL(clicked()),this,SLOT(ButtonExit()));
    connect(ui->buttonConfig,   SIGNAL(clicked()),this,SLOT(ViewDialogConfig()));
    connect(ui->buttonReset,    SIGNAL(clicked()),this,SLOT(ButtonReset()));
    connect(ui->buttonStartStop,SIGNAL(clicked()),this,SLOT(ButtonStartStop()));
    connect(ui->buttonReports,  SIGNAL(clicked()),this,SLOT(ButtonReports()));
    connect(ui->buttonTrendZoom,  SIGNAL(toggled(bool)),this,SLOT(ButtonTrendZoomOnOff(bool)));


    connect(&timer1000ms,&QTimer::timeout,this,&MainWindow::Timer1000ms);

    //connect(&timer3000ms,SIGNAL(timeout()),this,SLOT(Timer3000ms()));

 //   connect(ui->buttonTableResultSave,QPushButton::clicked,[&](){dialogTableResult.setModal(true); dialogTableResult.show();});
 //   connect(&dialogTableResult,SIGNAL(createTableReport()),this,SLOT(CreateTableReport()));

    orig_palette=ui->lineEditValueTemperature_1->palette();


    //ГРАФИК 1
    // Инициализируем объект полотна для графика ...
    wGraphic_1 = new QCustomPlot();
    wGraphic_1->setBackground(QColor(221,221,221));//Qt::lightGray);
    wGraphic_1->axisRect()->setBackground(QColor(255,251,240));  //оригинал - слоновая кость - 255,251,240

    ui->verticalLayoutGraphic_1->addWidget(wGraphic_1);
    wGraphic_1->xAxis->setLabel("Час, сек.");
    wGraphic_1->xAxis->setRange(X_RANGEPRETEST_MIN,X_RANGEPRETEST_MAX);
    wGraphic_1->xAxis->setTickStep(X_TICKSTEP);
    wGraphic_1->xAxis->setAutoTickStep(false);

     //wGraphic->setba


     //шкалы слева,
     wGraphic_1->yAxis->setLabel("Тепловий потік, кВт/м²");//+calEU);
     wGraphic_1->yAxis->setRange(Y_HEAT_RANGE_MIN,Y_HEAT_RANGE_MAX);
     wGraphic_1->yAxis->setAutoTickStep(false);
     wGraphic_1->yAxis->setTickStep(Y_HEAT_TICKSTEP);
     //wGraphic->yAxis->setTickLabelColor(Qt::transparent);
     wGraphic_1->yAxis->setVisible(true);

     //шкалы справа, гр.С
     wGraphic_1->yAxis2->setLabel("Температура, °C");//+calEU);
     wGraphic_1->yAxis2->setRange(Y_TEMPERATURE_RANGE_MIN,Y_TEMPERATURE_RANGE_MAX);
     wGraphic_1->yAxis2->setAutoTickStep(false);
     wGraphic_1->yAxis2->setTickStep(Y_TEMPERATURE_TICKSTEP);
     //wGraphic->yAxis2->setTickLabelColor(Qt::transparent);
     wGraphic_1->yAxis2->setVisible(true);




     // Инициализируем график Temperature 1 и привязываем его к Осям
     graphicTemperature_1 = new QCPGraph(wGraphic_1->xAxis, wGraphic_1->yAxis);
     wGraphic_1->addPlottable(graphicTemperature_1);  // Устанавливаем график на полотно
     QPen penTemperature_1=graphicTemperature_1->pen();
     penTemperature_1.setColor(Qt::red);
     penTemperature_1.setWidth(2);
     graphicTemperature_1->setPen(penTemperature_1); // Устанавливаем цвет графика
     graphicTemperature_1->setAntialiased(false);         // Отключаем сглаживание, по умолчанию включено
     graphicTemperature_1->setLineStyle(QCPGraph::lsLine);



     // Инициализируем график Temperature 2 и привязываем его к Осям
     graphicTemperature_2 = new QCPGraph(wGraphic_1->xAxis, wGraphic_1->yAxis);
     wGraphic_1->addPlottable(graphicTemperature_2);  // Устанавливаем график на полотно
     QPen penTemperature_2=graphicTemperature_2->pen();
     penTemperature_2.setColor(Qt::blue);
     penTemperature_2.setWidth(2);
     graphicTemperature_2->setPen(penTemperature_2); // Устанавливаем цвет графика
     graphicTemperature_2->setAntialiased(false);         // Отключаем сглаживание, по умолчанию включено
     graphicTemperature_2->setLineStyle(QCPGraph::lsLine);


     // Инициализируем график Temperature 3 и привязываем его к Осям
     graphicTemperature_3 = new QCPGraph(wGraphic_1->xAxis, wGraphic_1->yAxis);
     wGraphic_1->addPlottable(graphicTemperature_3);  // Устанавливаем график на полотно
     QPen penTemperature_3=graphicTemperature_3->pen();
     penTemperature_3.setColor(Qt::darkGreen);
     penTemperature_3.setWidth(2);
     graphicTemperature_3->setPen(penTemperature_3); // Устанавливаем цвет графика
     graphicTemperature_3->setAntialiased(false);         // Отключаем сглаживание, по умолчанию включено
     graphicTemperature_3->setLineStyle(QCPGraph::lsLine);


     // Инициализируем график Temperature 4 и привязываем его к Осям
     graphicTemperature_4 = new QCPGraph(wGraphic_1->xAxis, wGraphic_1->yAxis);
     wGraphic_1->addPlottable(graphicTemperature_4);  // Устанавливаем график на полотно
     QPen penTemperature_4=graphicTemperature_4->pen();
     penTemperature_4.setColor(Qt::magenta);
     penTemperature_4.setWidth(2);
     graphicTemperature_4->setPen(penTemperature_4); // Устанавливаем цвет графика
     graphicTemperature_4->setAntialiased(false);         // Отключаем сглаживание, по умолчанию включено
     graphicTemperature_4->setLineStyle(QCPGraph::lsLine);


     // Инициализируем график Temperature 5 и привязываем его к Осям
     graphicTemperature_5 = new QCPGraph(wGraphic_1->xAxis, wGraphic_1->yAxis2);
     wGraphic_1->addPlottable(graphicTemperature_5);  // Устанавливаем график на полотно
     QPen penTemperature_5=graphicTemperature_5->pen();
     penTemperature_5.setColor(QColor(0xff,0x80,0x00));
     penTemperature_5.setWidth(2);
     graphicTemperature_5->setPen(penTemperature_5); // Устанавливаем цвет графика
     graphicTemperature_5->setAntialiased(false);         // Отключаем сглаживание, по умолчанию включено
     graphicTemperature_5->setLineStyle(QCPGraph::lsLine);


     // Инициализируем график Temperature 6 и привязываем его к Осям
     graphicTemperature_6 = new QCPGraph(wGraphic_1->xAxis, wGraphic_1->yAxis2);
     wGraphic_1->addPlottable(graphicTemperature_6);  // Устанавливаем график на полотно
     QPen penTemperature_6=graphicTemperature_6->pen();
     penTemperature_6.setColor(Qt::darkBlue);
     penTemperature_6.setWidth(2);
     graphicTemperature_6->setPen(penTemperature_6); // Устанавливаем цвет графика
     graphicTemperature_6->setAntialiased(false);         // Отключаем сглаживание, по умолчанию включено
     graphicTemperature_6->setLineStyle(QCPGraph::lsLine);


     // Инициализируем график Temperature 7 и привязываем его к Осям
     graphicTemperature_7 = new QCPGraph(wGraphic_1->xAxis, wGraphic_1->yAxis2);
     wGraphic_1->addPlottable(graphicTemperature_7);  // Устанавливаем график на полотно
     QPen penTemperature_7=graphicTemperature_7->pen();
     penTemperature_7.setColor(QColor(0x00,0xBF,0xFF));
     penTemperature_7.setWidth(2);
     graphicTemperature_7->setPen(penTemperature_7); // Устанавливаем цвет графика
     graphicTemperature_7->setAntialiased(false);         // Отключаем сглаживание, по умолчанию включено
     graphicTemperature_7->setLineStyle(QCPGraph::lsLine);


     // Инициализируем график Temperature 8 и привязываем его к Осям
     graphicTemperature_8 = new QCPGraph(wGraphic_1->xAxis, wGraphic_1->yAxis2);
     wGraphic_1->addPlottable(graphicTemperature_8);  // Устанавливаем график на полотно
     QPen penTemperature_8=graphicTemperature_8->pen();
     penTemperature_8.setColor(Qt::darkGray);
     penTemperature_8.setWidth(2);
     graphicTemperature_8->setPen(penTemperature_8); // Устанавливаем цвет графика
     graphicTemperature_8->setAntialiased(false);         // Отключаем сглаживание, по умолчанию включено
     graphicTemperature_8->setLineStyle(QCPGraph::lsLine);






     // Подключаем сигнал от Оси X об изменении видимого диапазона координат
     //  к СЛОТу для переустановки формата времени оси.

     //connect(wGraphic_1->xAxis, SIGNAL(rangeChanged(QCPRange)),
     //        this, SLOT(slotRangeChanged(QCPRange)));

     wGraphic_1->setInteraction(QCP::iRangeZoom,false);   // Выключаем взаимодействие удаления/приближения
     wGraphic_1->setInteraction(QCP::iRangeDrag, false);  // Выключаем взаимодействие перетаскивания графика
     //wGraphic_1->axisRect()->setRangeDrag(Qt::Horizontal);   // Включаем перетаскивание только по горизонтальной оси
     //wGraphic_1->axisRect()->setRangeZoom(Qt::Horizontal);   // Включаем удаление/приближение только по горизонтальной оси
     wGraphic_1->replot();




     ui->lineEditValueTemperature_1->setStyleSheet("QLineEdit{border: 3px solid red; border-radius:10px;}");
     ui->lineEditValueTemperature_2->setStyleSheet("QLineEdit{border: 3px solid blue; border-radius:10px;}");
     ui->lineEditValueTemperature_3->setStyleSheet("QLineEdit{border: 3px solid darkgreen; border-radius:10px;}");
     ui->lineEditValueTemperature_4->setStyleSheet("QLineEdit{border: 3px solid magenta; border-radius:10px;}");
     ui->lineEditValueTemperature_5->setStyleSheet("QLineEdit{border: 3px solid #ff8000; border-radius:10px;}");
     ui->lineEditValueTemperature_6->setStyleSheet("QLineEdit{border: 3px solid darkblue; border-radius:10px;}");
     ui->lineEditValueTemperature_7->setStyleSheet("QLineEdit{border: 3px solid #00bfff; border-radius:10px;}");
     ui->lineEditValueTemperature_8->setStyleSheet("QLineEdit{border: 3px solid darkgray; border-radius:10px;}");

      ButtonReset();
      ui->labelTemperature_1->setText(temperature_1.GetChName());
      ui->labelTemperature_2->setText(temperature_2.GetChName());
      ui->labelTemperature_3->setText(temperature_3.GetChName());
      ui->labelTemperature_4->setText(temperature_4.GetChName());
      ui->labelTemperature_5->setText(temperature_5.GetChName());
      ui->labelTemperature_6->setText(temperature_6.GetChName());
      ui->labelTemperature_7->setText(temperature_5.GetChName());
      ui->labelTemperature_8->setText(temperature_6.GetChName());

 //     wGraphic_1->yAxis->setLabel("Температура , гр.С");
 //     wGraphic_1->yAxis->setLabel(temperature_1.GetChName()+", "+temperature_1.GetEU());
 //     wGraphic_2->yAxis->setLabel(movement_2.GetChName()+", "+movement_2.GetEU());

      QFont font=wGraphic_1->xAxis->labelFont();
      font.setPointSize(12);
      wGraphic_1->xAxis->setLabelFont(font);
      wGraphic_1->yAxis->setLabelFont(font);
      wGraphic_1->yAxis2->setLabelFont(font);


      graphicTemperature_1->setVisible(true);    ui->checkBoxTemperature1->setChecked(true);
      graphicTemperature_2->setVisible(true);    ui->checkBoxTemperature2->setChecked(true);
      graphicTemperature_3->setVisible(false);   ui->checkBoxTemperature3->setChecked(false);
      graphicTemperature_4->setVisible(false);   ui->checkBoxTemperature4->setChecked(false);
      graphicTemperature_5->setVisible(false);   ui->checkBoxTemperature5->setChecked(false);
      graphicTemperature_6->setVisible(false);   ui->checkBoxTemperature6->setChecked(false);
      graphicTemperature_7->setVisible(false);   ui->checkBoxTemperature7->setChecked(false);
      graphicTemperature_8->setVisible(false);   ui->checkBoxTemperature8->setChecked(false);


      connect(ui->checkBoxTemperature1,QCheckBox::toggled,this,[&](bool checked){graphicTemperature_1->setVisible(checked);wGraphic_1->replot();});
      connect(ui->checkBoxTemperature2,QCheckBox::toggled,this,[&](bool checked){graphicTemperature_2->setVisible(checked);wGraphic_1->replot();});
      connect(ui->checkBoxTemperature3,QCheckBox::toggled,this,[&](bool checked){graphicTemperature_3->setVisible(checked);wGraphic_1->replot();});
      connect(ui->checkBoxTemperature4,QCheckBox::toggled,this,[&](bool checked){graphicTemperature_4->setVisible(checked);wGraphic_1->replot();});
      connect(ui->checkBoxTemperature5,QCheckBox::toggled,this,[&](bool checked){graphicTemperature_5->setVisible(checked);wGraphic_1->replot();});
      connect(ui->checkBoxTemperature6,QCheckBox::toggled,this,[&](bool checked){graphicTemperature_6->setVisible(checked);wGraphic_1->replot();});
      connect(ui->checkBoxTemperature7,QCheckBox::toggled,this,[&](bool checked){graphicTemperature_7->setVisible(checked);wGraphic_1->replot();});
      connect(ui->checkBoxTemperature8,QCheckBox::toggled,this,[&](bool checked){graphicTemperature_8->setVisible(checked);wGraphic_1->replot();});
     // connect(ui->checkBoxTemperature1,SIGNAL(toggled(bool)),this,SLOT(CheckBoxTemperature1Changed(bool)));



      timer1000ms.start(3000);  //дадим время для первого опроса датчиков, далее будет установлено 1000 мс.

      startPreTestDT=QDateTime::currentDateTime();



      csvFileName=qApp->applicationDirPath()+"/csv/"+startPreTestDT.toString("yyyy.MM.dd_hh.mm.ss")+".csv";




    //dialogConfig




    AddCsvMessageColumns();

}
//=====================================================================
//=======================================================================================
MainWindow::~MainWindow()
{
    delete ui;
}
//=====================================================================
void MainWindow::AddCsvMessageColumns()
{
    //Logging to file - make hat
    QFile csvfile(csvFileName);

    if (csvfile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QString record="DateTime;T1;T2;T3;T4;T5;T6;T7;T8;Message\n";
        csvfile.write( record.toStdString().c_str());
        csvfile.close();
    }
}
//=====================================================================
void MainWindow::AddCsvMessage(QString message)
{

    QString strDateTime;
    QDateTime dt;
    dt=QDateTime::currentDateTime();
    strDateTime.sprintf("%.2u.%.2u.%.4u %.2u:%.2u:%.2u.%.3u",
                        dt.date().day(),dt.date().month(),dt.date().year(),
                        dt.time().hour(),dt.time().minute(),dt.time().second(),dt.time().msec());

    //Logging to file

    QFile csvfile(csvFileName);

    if (csvfile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        QString record;


        record.sprintf("%s;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;",strDateTime.toStdString().c_str(),
                      temperature_1.GetValue(),
                      temperature_2.GetValue(),
                      temperature_3.GetValue(),
                      temperature_4.GetValue(),
                      temperature_5.GetValue(),
                      temperature_6.GetValue(),
                      temperature_7.GetValue(),
                      temperature_8.GetValue());



        if (message!="")
        {
            record+=message+";\n";
        }
        else
        {
            record+=";\n";
        }




        csvfile.write( record.toStdString().c_str());
        csvfile.close();
    }


}
//======================================================================
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
      QLineEdit *lineEdit=qobject_cast<QLineEdit *>(obj);

        if (event->type() == QEvent::MouseButtonDblClick)
        {
            lineEdit->setReadOnly(false);
            return true;
        }
        else
        {
            return QObject::eventFilter(obj, event);
            //return false;
        }

}

//=======================================================================================
void MainWindow::LoadIniFile(QString iniFileName)
{
    iniFile=iniFileName;
    QSettings settings(iniFileName,QSettings::IniFormat);
    settings.beginGroup("main");

#ifdef LINUX
    comPort=settings.value("comPort","/dev/ttyUSB0").toString();
#else
    comPort=settings.value("comPort","COM1").toString();
#endif


}
//=======================================================================================
void MainWindow::SaveIniFileComPort()
{
    QSettings settings(iniFile,QSettings::IniFormat);
    settings.beginGroup("main");

    settings.setValue("comPort",comPort);
}
//=======================================================================================

void MainWindow::SaveIniFileOperator()
{
    /*
    QSettings settings(iniFile,QSettings::IniFormat);
    settings.beginGroup("main");

    settings.setValue("maxTemperature",maxTemperature);
    settings.setValue("interfaceStabConditions",interfaceStabConditions);
    settings.setValue("interfaceTavg",interfaceTavg);
    settings.setValue("interfaceT_Tavg",interfaceT_Tavg);
    settings.setValue("interfaceTreg",interfaceTreg);
*/
}
//=======================================================================================

//=======================================================================================
void MainWindow::ButtonTrendZoomOnOff(bool toggled)
{
    //wGraphic_1->setInteraction(QCP::iRangeDrag, true);
    if (toggled)
    {
        wGraphic_1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iRangeZoom);//QCP::iSelectPlottables | QCP::iSelectAxes);
        wGraphic_1->xAxis->setAutoTickStep(true);
        wGraphic_1->yAxis->setAutoTickStep(true);

    }
    else
    {


        wGraphic_1->setInteractions(NULL);



        double newMinRange;
        if (runningMode==ModePreTest)
        {
            wGraphic_1->xAxis->setRange(X_RANGEPRETEST_MIN,X_RANGEPRETEST_MAX);//xInterval);
            newMinRange=X_RANGEPRETEST_MIN;
        }
        else
        {
            wGraphic_1->xAxis->setRange(X_RANGETEST_MIN,X_RANGETEST_MAX);//xInterval);
            newMinRange=X_RANGETEST_MIN;
        }



        wGraphic_1->xAxis->setTickStep(X_TICKSTEP);
        wGraphic_1->xAxis->setAutoTickStep(false);

        wGraphic_1->yAxis->setRange(Y_HEAT_RANGE_MIN,Y_HEAT_RANGE_MAX);
        wGraphic_1->yAxis->setAutoTickStep(false);
        wGraphic_1->yAxis->setTickStep(Y_HEAT_TICKSTEP);

        wGraphic_1->yAxis2->setRange(Y_TEMPERATURE_RANGE_MIN,Y_TEMPERATURE_RANGE_MAX);
        wGraphic_1->yAxis2->setAutoTickStep(false);
        wGraphic_1->yAxis2->setTickStep(Y_TEMPERATURE_TICKSTEP);


        //автоизменение шкалы по Х
        if ((graphicTemperature_1->data()->size()>0) && (graphicTemperature_1->data()->last().key >= wGraphic_1->xAxis->range().upper))
        {
            double newMaxRange=wGraphic_1->xAxis->range().upper + ( (int)((graphicTemperature_1->data()->last().key - wGraphic_1->xAxis->range().upper)/100.0) + 1)*100;
            wGraphic_1->xAxis->setRange(newMinRange,newMaxRange);

        }


        wGraphic_1->replot();




    }

}

//=======================================================================================
void MainWindow::ViewDialogConfig()
{
    //QMessageBox::information(NULL,"ButonDialogConfig","pressed");
    dialogConfig.SetAnalogInputChannels(hashAnalogInputChannels);
    dialogConfig.SetComPort(comPort);

    dialogConfig.setModal(true);

    if (dialogConfig.exec()==QDialog::Accepted)
    {
        if (comPort!=dialogConfig.GetComPort())
        {
            comPort=dialogConfig.GetComPort();
            dconReader.StopPoll();
            //if (!mbReader.TestConnection(comPort))
            //{
            //    QMessageBox::critical(this,"Error","Немає підключення");
                //return;
            //}
            dconReader.SetComPort(comPort);
            dconReader.StartPoll();

        }

        SaveIniFileComPort();

    }
}
//=======================================================================================
void MainWindow::slotRangeChanged(const QCPRange &newRange)
{
/*
    float range_x=newRange.upper - newRange.lower;
    //if (range_x > stopInterval)
    //{
        //wGraphic->xAxis->setRange(0,stopInterval);
    //}
    //чтоб умещалось 10 точек
    if (range_x>100) wGraphic_1->xAxis->setAutoTickStep(true);
    else wGraphic_1->xAxis->setAutoTickStep(false);

    if (range_x<=100 && range_x>10) wGraphic_1->xAxis->setTickStep(5.0);
    if (range_x<=10 && range_x>5) wGraphic_1->xAxis->setTickStep(1.0);
    if (range_x<=5 && range_x>0) wGraphic_1->xAxis->setTickStep(0.5);

    //wGraphic->yAxis->rescale(true);   //void QCPAxis::rescale ( bool  onlyVisiblePlottables = false)
    //wGraphic->replot();
*/
}
//=======================================================================================
double MainWindow::calcAverage(QVector<double> vec)
{
    if (vec.empty()) {
        return 0.0;
    }
    return std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();

    //QVector::
}
//=======================================================================================
//=======================================================================================

//=======================================================================================
bool MainWindow::calcAvgMinMaxRegress(const QVector<QCPData> &data, double &avg, double &min, double &max, double &regress, double &regress_koeff_a, double &regress_koeff_b)
{
    double accuT1=0.0;
    double avgT1=0.0;

    if (data.size()==0) return false;

    double minT1=data.first().value;
    double maxT1=data.first().value;

    //for regression
    double S1=data.size();
    double S2=0.0;   //sum(t_i)
    double S3=0.0;
    double S4=0.0;
    double S5=0.0;



    foreach(QCPData cpdata, data)
    {
        //

        accuT1+=cpdata.value;

        if (minT1>cpdata.value) minT1=cpdata.value;
        if (maxT1<cpdata.value) maxT1=cpdata.value;

        S2+=cpdata.key;
        S3+=cpdata.value;
        S4+=cpdata.key * cpdata.key;
        S5+=cpdata.value * cpdata.key;

    }
    avgT1=accuT1 / data.size();


    if ((S1*S4 - S2*S2) == 0.0) return false;


    //в присланном Геннадием документе для рассчета линейной регрессии перепутаны а и в коэффициенты,
    //смотреть рассчет коэффициентов в http://mathprofi.ru/linejnyj_koefficient_korrelyacii.html
    //T- temperature, t- time
    //T(t)=a*t+b
    double b = (S3*S4 - S5*S2) / (S1*S4 - S2*S2);
    double a = (S1*S5 - S2*S3) / (S1*S4 - S2*S2);



    //regression = (a*t_last+b) - (a*t_first + b) ==  a*t_last - a*t_first
    double regressT1= (a*data.last().key + b) - (a*data.first().key + b);

    avg=avgT1;
    min=minT1;
    max=maxT1;
    regress=regressT1;
    regress_koeff_a=a;
    regress_koeff_b=b;
    return true;

}


//=======================================================================================
void MainWindow::Timer1000ms()
{

    timer1000ms.setInterval(1000);

    ui->labelTemperature_1->setText(temperature_1.GetChName());
    ui->labelTemperature_2->setText(temperature_2.GetChName());
    ui->labelTemperature_3->setText(temperature_3.GetChName());
    ui->labelTemperature_4->setText(temperature_4.GetChName());
    ui->labelTemperature_5->setText(temperature_5.GetChName());
    ui->labelTemperature_6->setText(temperature_6.GetChName());
    ui->labelTemperature_7->setText(temperature_7.GetChName());
    ui->labelTemperature_8->setText(temperature_8.GetChName());
    //wGraphic_1->yAxis->setLabel(movement_1.GetChName()+", "+movement_1.GetEU());
    //wGraphic_2->yAxis->setLabel(movement_2.GetChName()+", "+movement_2.GetEU());

    ui->lineEditValueTemperature_1->setText(temperature_1.GetValueString(1));
    ui->lineEditValueTemperature_2->setText(temperature_2.GetValueString(1));
    ui->lineEditValueTemperature_3->setText(temperature_3.GetValueString(1));
    ui->lineEditValueTemperature_4->setText(temperature_4.GetValueString(1));
    ui->lineEditValueTemperature_5->setText(temperature_5.GetValueString(1));
    ui->lineEditValueTemperature_6->setText(temperature_6.GetValueString(1));
    ui->lineEditValueTemperature_7->setText(temperature_7.GetValueString(1));
    ui->lineEditValueTemperature_8->setText(temperature_8.GetValueString(1));


/*
    if (cmdButton==StartCmd)
    {
         ui->buttonStartStop->setIcon(QIcon(":/icons/player_stop.png"));
         ui->buttonStartStop->setText("Стоп");
         runningMode=ModeTest;
         //controlPoints.clear();
         infoText="СТАРТ ВИПРОБУВАННЯ   ";
         startTestDT=QDateTime::currentDateTime();
         startTestDT_str=startTestDT.toString("yyyy.MM.dd_hh.mm.ss");
         infoText+=" "+startTestDT.toString("hh:mm:ss dd.MM.yy");

         AddCsvMessage("start test");

         //infoText+=QString("\nЧАС: 00:00:00");
         //ui->listWidgetInfo->clear();
         //ui->listWidgetInfo->addItem(QTime::currentTime().toString()+ QString(" Started Test Mode"));
         //ui->listWidgetInfo->addItem(QTime::currentTime().toString()+ QString(" Wait temperature:"));
         wGraphic_1->xAxis->setRange(X_RANGETEST_MIN,X_RANGETEST_MAX);//xInterval);
         wGraphic_1->yAxis->setRange(Y_TEMPERATURE_RANGE_MIN,Y_TEMPERATURE_RANGE_MAX);
         //wGraphic->yAxis2->setRange(0,Y_TEMPERATURE_RANGE);
         //graphicMovement_1->clearData();
         //graphicMovement_2->clearData();
         //graphicTemperature_1->clearData();
         //graphicTemperature_2->clearData();
         //graphicTemperature_3->clearData();
         //graphicTemperature_4->clearData();
         //graphicTemperature_5->clearData();
         //graphicTemperature_6->clearData();


         //перешкалировать температуры 1,2,3,4 по Х, чтобы получить шкалу на графике -600...1800,  т.е. от -10мин до +30минут
         //


         double seconds_from_start=startPreTestDT.msecsTo(startTestDT)/1000.0;

         //QList<QCPData> temp_1_data;
         temp_1_data.clear();
         foreach(QCPData cpdata, graphicTemperature_1->data()->values())
         {
            cpdata.key=cpdata.key-seconds_from_start;
            temp_1_data.append(cpdata);
         }
         //QList<QCPData> temp_2_data;
         temp_2_data.clear();
         foreach(QCPData cpdata, graphicTemperature_2->data()->values())
         {
            cpdata.key=cpdata.key-seconds_from_start;
            temp_2_data.append(cpdata);
         }
         QList<QCPData> temp_3_data;
         foreach(QCPData cpdata, graphicTemperature_3->data()->values())
         {
            cpdata.key=cpdata.key-seconds_from_start;
            temp_3_data.append(cpdata);
         }
         QList<QCPData> temp_4_data;
         foreach(QCPData cpdata, graphicTemperature_4->data()->values())
         {
            cpdata.key=cpdata.key-seconds_from_start;
            temp_4_data.append(cpdata);
         }

         graphicTemperature_1->clearData();
         foreach(QCPData cpdata, temp_1_data)
         {
            graphicTemperature_1->addData(cpdata);
         }

         graphicTemperature_2->clearData();
         foreach(QCPData cpdata, temp_2_data)
         {
            graphicTemperature_2->addData(cpdata);
         }

         graphicTemperature_3->clearData();
         foreach(QCPData cpdata, temp_3_data)
         {
            graphicTemperature_3->addData(cpdata);
         }

         graphicTemperature_4->clearData();
         foreach(QCPData cpdata, temp_4_data)
         {
            graphicTemperature_4->addData(cpdata);
         }

         //то же графики регрессов
         QList<QCPData> regr_1_data;
         foreach(QCPData cpdata, graphicRegress_1->data()->values())
         {
            cpdata.key=cpdata.key-seconds_from_start;
            regr_1_data.append(cpdata);
         }
         QList<QCPData> regr_2_data;
         foreach(QCPData cpdata, graphicRegress_2->data()->values())
         {
            cpdata.key=cpdata.key-seconds_from_start;
            regr_2_data.append(cpdata);
         }

         graphicRegress_1->clearData();
         foreach(QCPData cpdata, regr_1_data)
         {
            graphicRegress_1->addData(cpdata);
         }

         graphicRegress_2->clearData();
         foreach(QCPData cpdata, regr_2_data)
         {
            graphicRegress_2->addData(cpdata);
         }


         //startTestDT=QDateTime::currentDateTime();
         //startTestTemperature=value_grC_offseted;
         //sound_start->play();
         //QProcess::startDetached(QString("aplay ")+QApplication::applicationDirPath() + "/start.wav");
         cmdButton=NoCmd;
         ui->buttonReset->setEnabled(false);
         ui->buttonConfig->setEnabled(false);
         ui->buttonExit->setEnabled(false);


         //return;

    } //if (cmdButton==StartCmd)
*/


    if (runningMode==ModePreTest)
    {
         //добавляем данные на график
         double seconds_from_start;

         seconds_from_start=startPreTestDT.msecsTo(QDateTime::currentDateTime())/1000.0;
         //double minutes_from_start=seconds_from_start/60.0;

         //graphicMovement_1->addData(minutes_from_start,movement_1.GetValue());
         //graphicMovement_2->addData(minutes_from_start,movement_2.GetValue());

         graphicTemperature_1->addData(seconds_from_start,temperature_1.GetValue());
         graphicTemperature_2->addData(seconds_from_start,temperature_2.GetValue());
         graphicTemperature_3->addData(seconds_from_start,temperature_3.GetValue());
         graphicTemperature_4->addData(seconds_from_start,temperature_4.GetValue());
         graphicTemperature_5->addData(seconds_from_start,temperature_5.GetValue());
         graphicTemperature_6->addData(seconds_from_start,temperature_6.GetValue());
         graphicTemperature_7->addData(seconds_from_start,temperature_7.GetValue());
         graphicTemperature_8->addData(seconds_from_start,temperature_8.GetValue());


         if (!ui->buttonTrendZoom->isChecked())
         {
             //автоизменение шкалы по Х
             if (seconds_from_start >= wGraphic_1->xAxis->range().upper)
             {
                double newMaxRange=wGraphic_1->xAxis->range().upper + 100;
                wGraphic_1->xAxis->setRange(0,newMaxRange);

             }
         }




        /*
        //автоизменение шкалы вверх - увеличиваем так, чтоб график не біл на верхней границе
        if (ppm_pwm1*1.03>Y_PRESSURE_RANGE && ppm_pwm1*1.03 > wGraphic->yAxis->range().upper)        {
            wGraphic->yAxis->setRange(wGraphic->yAxis->range().lower, ppm_pwm1*1.03);
            //wGraphic->yAxis->rescale();      // Масштабируем график по данным
        }
        //автоизменение шкалы вниз
        if (ppm_pwm1 < wGraphic->yAxis->range().lower)
        {
            wGraphic->yAxis->setRange(ppm_pwm1,wGraphic->yAxis->range().upper);
            //wGraphic->yAxis->rescale();      // Масштабируем график по данным
        }

        //автоизменение шкалы вверх - увеличиваем так, чтоб график не біл на верхней границе
        if (ppm_pwm2*1.03>Y_TEMPERATURE_RANGE && ppm_pwm2*1.03 > wGraphic->yAxis2->range().upper)
        {
            wGraphic->yAxis2->setRange(wGraphic->yAxis2->range().lower, ppm_pwm2*1.03);
            //wGraphic->yAxis->rescale();      // Масштабируем график по данным
        }
        //автоизменение шкалы вниз
        if (ppm_pwm2 < wGraphic->yAxis2->range().lower)
        {
            wGraphic->yAxis2->setRange(ppm_pwm2,wGraphic->yAxis2->range().upper);
            //wGraphic->yAxis->rescale();      // Масштабируем график по данным
        }
        */

        /*
        //автоизменение шкалы по Х


        if (seconds_from_start>X_RANGEVIEW)
        {
            wGraphic->xAxis->setRange(seconds_from_start-X_RANGEVIEW,seconds_from_start);
            graphicPpm_pwm1->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
            graphicPpm_pwm2->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
            graphicPpm_pwm3->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
            graphicPressure_1->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
            graphicPressure_2->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
            graphicAirFlow_1->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
            graphicAirFlow_2->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
            graphicHeatFlow->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
            graphicForce->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);

            //new in GemobudMaskTest3
            graphicTemperature_1->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
            graphicTemperature_2->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
            graphicPpm_O2->removeDataBefore(seconds_from_start-X_RANGEVIEW-1);
        }
        */


         int viewRunningSecs=startPreTestDT.secsTo(QDateTime::currentDateTime());
         QString viewRunningStr="";
         if (viewRunningSecs / 3600 < 10) viewRunningStr+="0";
         viewRunningStr+=QString::number(viewRunningSecs / 3600) + ":";

         viewRunningSecs=viewRunningSecs % 3600;
         if (viewRunningSecs / 60 < 10) viewRunningStr+="0";
         viewRunningStr+=QString::number(viewRunningSecs / 60) + ":";

         viewRunningSecs=viewRunningSecs % 60;
         if (viewRunningSecs < 10) viewRunningStr+="0";
         viewRunningStr+=QString::number(viewRunningSecs);



         //calc
         //double aveT=calcAverage(graphicTemperature_1->data()->values().toVector());






         AddCsvMessage("");

         // Tavg=(750±5)°C  |T-Tavg|≤10°C  Treg≤2°C  на протязі 10 хв.




        wGraphic_1->replot();           // Отрисовываем график










    } //if (runningMode==ModePreTest)


/*
    if (runningMode==ModeTest)
    {
        //добавляем данные на график
        double seconds_from_start;

        seconds_from_start=startTestDT.msecsTo(QDateTime::currentDateTime())/1000.0;
        //double minutes_from_start=seconds_from_start/60.0;

        //graphicMovement_1->addData(minutes_from_start,movement_1.GetValue());
        //graphicMovement_2->addData(minutes_from_start,movement_2.GetValue());

        graphicTemperature_1->addData(seconds_from_start,temperature_1.GetValue());
        graphicTemperature_2->addData(seconds_from_start,temperature_2.GetValue());
        graphicTemperature_3->addData(seconds_from_start,temperature_3.GetValue());
        graphicTemperature_4->addData(seconds_from_start,temperature_4.GetValue());

        AddCsvMessage();

        if (!ui->buttonTrendZoom->isChecked())
        {
            //автоизменение шкалы по Х
            if (seconds_from_start >= wGraphic_1->xAxis->range().upper)
            {
                double newMaxRange=wGraphic_1->xAxis->range().upper + 100;
                wGraphic_1->xAxis->setRange(0,newMaxRange);

            }
        }






        int testRunningSecs=startTestDT.secsTo(QDateTime::currentDateTime());
        QString testRunningStr="";
        if (testRunningSecs / 3600 < 10) testRunningStr+="0";
        testRunningStr+=QString::number(testRunningSecs / 3600) + ":";

        testRunningSecs=testRunningSecs % 3600;
        if (testRunningSecs / 60 < 10) testRunningStr+="0";
        testRunningStr+=QString::number(testRunningSecs / 60) + ":";

        testRunningSecs=testRunningSecs % 60;
        if (testRunningSecs < 10) testRunningStr+="0";
        testRunningStr+=QString::number(testRunningSecs);





        //для ожидания первого интервала и потом добавляем пять минут и ждем следующий, последний будет на 60 минуте;
        //double seconds_end_current_interval=1800; // 30 minutes
        //double secons_add_to_switch_next_interval=300;   //5 minutes
        //double secons_last_interval=3600;   //60 minutes


        if (seconds_from_start>=seconds_end_first_interval+num_interval*secons_add_to_switch_next_interval)
        {

            //взять данные за последние 10 минут, рассчитать среднее, регрессию, построить графики регрессий
            //данные за последние 10 минут
            //QList<QCPData> temp_1_data;
            temp_1_data.clear();
            foreach(QCPData cpdata, graphicTemperature_1->data()->values())
            {
                if (cpdata.key >= seconds_from_start - 600) temp_1_data.append(cpdata);
            }
            //QList<QCPData> temp_2_data;
            temp_2_data.clear();
            foreach(QCPData cpdata, graphicTemperature_2->data()->values())
            {
                if (cpdata.key >= seconds_from_start - 600) temp_2_data.append(cpdata);
            }


            if (temp_1_data.size()==0 || temp_2_data.size()==0)
            {
                return;
            }

            //double accuT1=0.0;
            double avgT1=0.0;
            double minT1=0.0;//temp_1_data.first().value;
            double maxT1=0.0;//temp_1_data.first().value;
            double regressT1=0.0;
            double regressT1_koeff_a=0.0;
            double regressT1_koeff_b=0.0;


            double avgT2=0.0;
            double minT2=0.0;
            double maxT2=0.0;
            double regressT2=0.0;
            double regressT2_koeff_a=0.0;
            double regressT2_koeff_b=0.0;

            calcAvgMinMaxRegress(temp_1_data,avgT1,minT1,maxT1,regressT1,regressT1_koeff_a,regressT1_koeff_b);
            calcAvgMinMaxRegress(temp_2_data,avgT2,minT2,maxT2,regressT2,regressT2_koeff_a,regressT2_koeff_b);

            AddCsvMessage("",true,avgT1,std::max(fabs(maxT1-avgT1),fabs(minT1-avgT1)),regressT1,
                          avgT2,std::max(fabs(maxT2-avgT2),fabs(minT2-avgT2)),regressT2);

            // Tavg=(750±5)°C  |T-Tavg|≤10°C  Treg≤2°C  на протязі 10 хв.

            //QString temp1StabilizationInfo,temp2StabilizationInfo;

            if (((seconds_from_start-600>=0) && (fabs(regressT1)<=2.0)))
            {
                temp1_TestStabilized=true;
                temp1TestStabilizationInfo=temperature_1.GetChName() +  " - стабілізації досягнуто.";
            }
            else
            {
                temp1_TestStabilized=false;
                temp1TestStabilizationInfo=temperature_1.GetChName()+":  Treg="+QString::number(regressT1,'f',3);
            }

            if (((seconds_from_start-600>=0) && (fabs(regressT2)<=2.0)))
            {
                temp2_TestStabilized=true;
                temp2TestStabilizationInfo=temperature_2.GetChName() +  " - стабілізації досягнуто.";
            }
            else
            {
                temp2_TestStabilized=false;
                temp2TestStabilizationInfo=temperature_2.GetChName()+":  Treg="+QString::number(regressT2,'f',3);
            }


            //ui->labelInfo->setText(infoText+QString("\nЧАС: ") + testRunningStr+"\n");

            //ui->labelInfo->setText(ui->labelInfo->text()+QString("ОЧІКУЄМ ")+QString::number(30+num_interval*5)+" хвилину...");






            //graphicTestRegress_1->clearData();
            graphicTestRegress_1[num_interval]->addData(temp_1_data.first().key, regressT1_koeff_a*temp_1_data.first().key + regressT1_koeff_b);
            graphicTestRegress_1[num_interval]->addData(temp_1_data.last().key,  regressT1_koeff_a*temp_1_data.last().key  + regressT1_koeff_b);

            //graphicRegress_2->clearData();
            graphicTestRegress_2[num_interval]->addData(temp_2_data.first().key, regressT2_koeff_a*temp_2_data.first().key + regressT2_koeff_b);
            graphicTestRegress_2[num_interval]->addData(temp_2_data.last().key,  regressT2_koeff_a*temp_2_data.last().key  + regressT2_koeff_b);



            //проверяем на последний интервал
            if (num_interval>=7)
            {
                runningMode=ModeTestStopped;//STOP
                testStopReason=QString("ЗАВЕРШЕНО... Стабілізації не досягнуто за 60 хв.");
                AddCsvMessage("stop test - stabilization did not occur within 60 minutes");
            }


            if(temp1_TestStabilized &&  temp2_TestStabilized)
            {
                runningMode=ModeTestStopped; //STOP
                testStopReason=QString("ЗАВЕРШЕНО... Стабілізація досягнута на ")+QString::number(30+num_interval*5)+" хв.";
                AddCsvMessage("stop test - stabilization occurred at "+ QString::number(30+num_interval*5) +" minutes");
            }


            //переключаем интервал
            num_interval++;
            //seconds_end_current_interval=seconds_end_current_interval+secons_add_to_switch_next_interval;



        }

        QString plainText=infoText+QString("\nЧАС: ") + testRunningStr+"\n"+
                                        QString("ОЧІКУЄМ ")+QString::number(30+num_interval*5)+" хвилину..."+"\n"+
                                        temp1TestStabilizationInfo+"\n"+
                            temp2TestStabilizationInfo+"\n"+
                            testStopReason;
        ui->labelInfo->setText(plainText.replace("\n","<br>"));


        if (runningMode==ModeTestStopped)
        {
            ui->buttonTrendZoom->setChecked(false);
            CreateTestReport();
        }



        //если достигнуто условие окончания
        //для режима Test (Испытание) - нажатие кнопки СТОП
        //для режима Test (Испытание) - резкое падение давления на 20% за 1сек.


        if (cmdButton==StopCmd)
        {

            ui->buttonStartStop->setIcon(QIcon(":/icons/player_play.png"));
            ui->buttonStartStop->setText("Старт");
            runningMode=ModeTestStopped;
            //sound_stop->play();

            //QProcess::startDetached(QString("aplay ")+QApplication::applicationDirPath() + "/stop.wav");
            testStopReason=QString("ЗАВЕРШЕНО... Зупинено оператором");

            QString plainText=infoText+QString("\nЧАС: ") + testRunningStr+"\n"+
                                        QString("ОЧІКУЄМ ")+QString::number(30+num_interval*5)+" хвилину..."+"\n"+
                                        temp1TestStabilizationInfo+"\n"+
                                temp2TestStabilizationInfo+"\n"+
                                testStopReason;
            ui->labelInfo->setText(plainText.replace("\n","<br>"));


            AddCsvMessage("stop test -  stopped by the operator");


            //get 1st graph

            //wGraphic_1->replot();
            //wGraphic_1->savePng(qApp->applicationDirPath()+"/png/"+startTestDT_str+"_1.png",640,480);
            //QImage img_1(qApp->applicationDirPath()+"/png/"+startTestDT_str+"_1.png");

            //get 2nd graph
            //wGraphic_2->replot();
            //wGraphic_2->savePng(qApp->applicationDirPath()+"/png/"+startTestDT_str+"_2.png",640,480);
            //QImage img_2(qApp->applicationDirPath()+"/png/"+startTestDT_str+"_2.png");


            //img_2 = img_2.convertToFormat(QImage::Format_Grayscale16 ,Qt::AutoColor | Qt::ThresholdDither);
            //img_2.save(qApp->applicationDirPath()+"/png/"+startTestDT_str+"_2_ttt.png");


            ui->buttonTrendZoom->setChecked(false);
            CreateTestReport();
            //createReport(qApp->applicationDirPath()+"/reports/"+startTestDT_str, startTestDT.toString("dd.MM.yyyy hh:mm:ss") ,
            //             testRunningStr, img_1, temperature_1.GetValue(), temperature_2.GetValue());

            ui->buttonReset->setEnabled(true);
            ui->buttonConfig->setEnabled(true);
            ui->buttonExit->setEnabled(true);


        }


        wGraphic_1->replot();           // Отрисовываем график

    }   //     if(runningMode==ModeTest)
*/










}

//=======================================================================================


//=======================================================================================
void MainWindow::ButtonReset()
{


    //graphicMovement_1->clearData();
    //graphicMovement_2->clearData();
    graphicTemperature_1->clearData();
    graphicTemperature_2->clearData();
    graphicTemperature_3->clearData();
    graphicTemperature_4->clearData();
    graphicTemperature_5->clearData();
    graphicTemperature_6->clearData();
    graphicTemperature_7->clearData();
    graphicTemperature_8->clearData();


    wGraphic_1->xAxis->setRange(X_RANGEPRETEST_MIN,X_RANGEPRETEST_MAX);//xInterval);
    wGraphic_1->yAxis->setRange(Y_HEAT_RANGE_MIN, Y_HEAT_RANGE_MAX);
    wGraphic_1->yAxis2->setRange(Y_TEMPERATURE_RANGE_MIN,Y_TEMPERATURE_RANGE_MAX);


    startTestDT=QDateTime::currentDateTime();
    startPreTestDT=QDateTime::currentDateTime();
    //ui->labelInfo->setText("ОЧІКУЄМ СТАРТ");
    //mbReader.SetDOState(DO_Command::DO_ON_CMD);



    wGraphic_1->replot();
}
//=======================================================================================
void MainWindow::ButtonStartStop()
{
    //команда на старт
    if (runningMode==ModePreTest || runningMode==ModeTestStopped)
    {
        cmdButton=StartCmd;
    }

    //команда на стоп
    if (runningMode==ModeTest)
    {
        if (QMessageBox::question(NULL,"Припинення тесту","Зупинити тест та сформувати звіт?",QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes)
        {
            cmdButton=StopCmd;
        }
    }
}
//=======================================================================================
void MainWindow::ButtonExit()
{
    //QProcess::startDetached("sudo shutdown -h now");
    close();

}
//=======================================================================================
void MainWindow::ButtonReports()
{

    //if (!isCalibrationPageEnabled)
    //{

        FMDlg dlg(tr("Reports"), qApp->applicationDirPath()+"/reports/", this);

        //dlg.setLocalPath("reports");
        //dlg.setFilter(QStringList() << "*.odf");
        //dlg.enablePreviewButton();

        dlg.launch();
    //}
    //else
    //{
    //    FMDlg dlg(tr("Calibration Reports"), qApp->applicationDirPath()+"/calibration_reports/", this);
    //    dlg.launch();
    //}

}
//=======================================================================================
void MainWindow::ValueChanged(QString str)
{
/*
    //  QLineEdit *snd=(QLineEdit *)sender();
    float operatorPointMin=0;
    float operatorPointMax=50;

    QPalette palette_green = ui->lineEditOperatorPoint_1->palette();
    QPalette palette_red = ui->lineEditOperatorPoint_1->palette();

    palette_green.setColor( QPalette::Text, Qt::black );
    palette_red.setColor( QPalette::Text, Qt::red );
 //   p.setColor(QPalette::Base, Qt::red);
 //   p.setColor(QPalette::Background, Qt::red);
  //    snd->setPalette( palette );


      if (ui->lineEditOperatorPoint_1->text().length()==0 ||
         ( ui->lineEditOperatorPoint_1->text().toFloat()>=operatorPointMin && ui->lineEditOperatorPoint_1->text().toFloat()<=operatorPointMax))
      {
        ui->lineEditOperatorPoint_1->setPalette(palette_green);
      }
      else
      {
        ui->lineEditOperatorPoint_1->setPalette(palette_red);
      }
*/
 // QMessageBox::information(this,"Test","ddasfsd",QMessageBox::Ok);

}
//=======================================================================================
//================================================================================================
//
//Reports
//
//================================================================================================
void MainWindow::setCurrentBlockAlignment(QTextCursor &cursor,
                              Qt::Alignment alignment) {
  QTextBlockFormat tmp = cursor.blockFormat();
  tmp.setAlignment(alignment);
  cursor.setBlockFormat(tmp);
}
//================================================================================================
QTextCharFormat MainWindow::charFormat(int pointSize,
                           bool bold) {
  QTextCharFormat tmp;
  QFont font;
  font.setFamily("Times");
  tmp.setFont(font);
  tmp.setFontPointSize(pointSize);
  tmp.setFontWeight(bold ? QFont::Bold : QFont::Normal);
  return tmp;
}
//================================================================================================

void MainWindow::createReport(const QString &fileName,
                              const QString &startDT,
                              const QString &timeDT,
                              const QImage &plot_movement_1,
                              const double X_1,
                              const double X_2)
{

  QTextDocument *document = new QTextDocument();




  //QFont font;
  //font.setFamily("Helvetica");//("Swiss");//("Times New Roman");//("Times");

  QTextCursor cursor(document);

//  QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");

//  setCurrentBlockAlignment(cursor, Qt::AlignLeft);
//  cursor.insertText(date + '\n', charFormat(11, false));//12

//  cursor.insertBlock();
//  setCurrentBlockAlignment(cursor, Qt::AlignCenter);
//  cursor.insertText(company, charFormat(11, false));//12

  cursor.insertBlock();
  setCurrentBlockAlignment(cursor, Qt::AlignCenter);
  cursor.insertText("Звіт по випробуванню", charFormat(16, true));

  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n

  cursor.insertBlock();
  setCurrentBlockAlignment(cursor, Qt::AlignLeft);
  cursor.insertText("Початок випробування: " + startDT , charFormat(14, true));//12
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n

  cursor.insertText("Час випробування: " + timeDT , charFormat(14, true));//12
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n


  //TEST 1

  cursor.insertBlock();
  setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
  cursor.insertText("Тест 1", charFormat(14, true));//12
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n

//  cursor.insertText("Умова закінчення: " + stopCondition , charFormat(14, true));//12
//  cursor.movePosition(QTextCursor::End);
//  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n
\
  cursor.insertBlock();
  setCurrentBlockAlignment(cursor, Qt::AlignLeft);
  cursor.insertText("Заміри верхнього рівня пластичного шару:", charFormat(12, true));//12
  cursor.movePosition(QTextCursor::End);
  //cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n


/*
  //table
  cursor.movePosition(QTextCursor::End);
  cursor.insertTable(levelup_points_1.size()+1,4);

  //первая строка - 4 ячейки
  setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
  cursor.insertText("Час, хв.", charFormat(12, true));
  cursor.movePosition(QTextCursor::NextCell);

  setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
  cursor.insertText("Замір опер., мм", charFormat(12, true));
  cursor.movePosition(QTextCursor::NextCell);

  setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
  cursor.insertText("Усадка, мм", charFormat(12, true));
  cursor.movePosition(QTextCursor::NextCell);

  setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
  cursor.insertText("Температура, С", charFormat(12, true));


  foreach(OperatorPoint pnt, levelup_points_1)
  {

      cursor.movePosition(QTextCursor::NextCell);
      setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
      cursor.insertText(QString::number(pnt.time_min,'f',0));

      cursor.movePosition(QTextCursor::NextCell);
      setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
      cursor.insertText(QString::number(pnt.operatorEnteredPoint,'f',1));

      cursor.movePosition(QTextCursor::NextCell);
      setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
      cursor.insertText(QString::number(pnt.movement,'f',1));

      cursor.movePosition(QTextCursor::NextCell);
      setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
      cursor.insertText(QString::number(pnt.temperature,'f',1));
  }


  cursor.movePosition(QTextCursor::End);
  cursor.insertBlock();
  setCurrentBlockAlignment(cursor, Qt::AlignLeft);
  //cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n

  cursor.insertText("Заміри нижнього рівня пластичного шару:", charFormat(12, true));//12
  cursor.movePosition(QTextCursor::End);
  //cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n

  //table
  cursor.movePosition(QTextCursor::End);
  cursor.insertTable(leveldown_points_1.size()+1,4);
  setCurrentBlockAlignment(cursor, Qt::AlignHCenter);

  //первая строка - 4 ячейки
  cursor.insertText("Час, хв.", charFormat(12, true));
  cursor.movePosition(QTextCursor::NextCell);

  setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
  cursor.insertText("Замір опер., мм", charFormat(12, true));
  cursor.movePosition(QTextCursor::NextCell);

  setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
  cursor.insertText("Усадка, мм", charFormat(12, true));
  cursor.movePosition(QTextCursor::NextCell);

  setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
  cursor.insertText("Температура, С", charFormat(12, true));


  foreach(OperatorPoint pnt, leveldown_points_1)
  {

      cursor.movePosition(QTextCursor::NextCell);
      setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
      cursor.insertText(QString::number(pnt.time_min,'f',0));

      cursor.movePosition(QTextCursor::NextCell);
      setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
      cursor.insertText(QString::number(pnt.operatorEnteredPoint,'f',1));

      cursor.movePosition(QTextCursor::NextCell);
      setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
      cursor.insertText(QString::number(pnt.movement,'f',1));

      cursor.movePosition(QTextCursor::NextCell);
      setCurrentBlockAlignment(cursor, Qt::AlignHCenter);
      cursor.insertText(QString::number(pnt.temperature,'f',1));
  }

  cursor.movePosition(QTextCursor::End);
  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n

  cursor.insertText(QString("X (значення усадки по закінченню) = ") + QString::number(X_1,'f',1) + " мм", charFormat(12, true));


  QVector<OperatorPoint> all_points_1 = levelup_points_1 + leveldown_points_1;

  double max_difference_1=0.0;
  foreach (OperatorPoint point, all_points_1)
  {
      double difference=getLinearApproximatedValue(levelup_points_1, point.time_min) -
                        getLinearApproximatedValue(leveldown_points_1, point.time_min);

      if (max_difference_1 < difference) max_difference_1 = difference;
  }

  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n
  cursor.insertText(QString("Y (максимальна різниця верхніого і нижнього рівнів пл. шару) = ") + QString::number(max_difference_1,'f',1) + " мм", charFormat(12, true));
  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n
*/
  //graphic
  cursor.movePosition(QTextCursor::End);
  cursor.insertBlock();

  setCurrentBlockAlignment(cursor, Qt::AlignCenter);


  //adding image to document

  //cursor.insertImage(plot_movement_1);

  document->addResource(QTextDocument::ImageResource, QUrl("plot_movement_1.png"), plot_movement_1);
  QTextImageFormat imageFormat_1;
  imageFormat_1.setQuality(100);
  imageFormat_1.setName("plot_movement_1.png");
  cursor.insertImage(imageFormat_1);
  cursor.movePosition(QTextCursor::End);





  cursor.insertText(QObject::tr("\n\n"), charFormat(12, true));//casey - line \n
  cursor.movePosition(QTextCursor::End);

  //TESTS END





  cursor.insertText(QObject::tr("\n\n"), charFormat(12, true));//casey - line \n
  cursor.movePosition(QTextCursor::End);

  qDebug() << fileName;
  QTextDocumentWriter writer(fileName+".odt");
  if (!writer.write(document)) qDebug() << "errro write";

  qDebug() << writer.supportedDocumentFormats();

  //QDesktopServices::openUrl(QUrl(fileName+".odt"));

  //QTextEdit  *te = new QTextEdit();
  //te->setDocument(document);
  //te->showMaximized();

  delete document;

}

//=================================================================================================
double MainWindow::getLinearApproximatedValue(QVector<OperatorPoint> points, double x)
{

    if (points.size()==0)
    {
        return 0.0;
    }

    if (points.size()==1)
    {
        return points[0].operatorEnteredPoint;
    }

    double y, x1,x2,y1,y2;

    //key==time_min
    //val==operatorEnteredPoint

    if (x <= points[0].time_min)
    {
        x1=points[0].time_min;
        x2=points[1].time_min;
        y1=points[0].operatorEnteredPoint;
        y2=points[1].operatorEnteredPoint;
        //   x-x1
        //y=------- * (y2-y1) + y1
        //   x2-x1
        y=(x-x1)*(y2-y1)/(x2-x1) + y1;
        return y;
    }

    if (x > points[points.size()-1].time_min)
    {
        x1=points[points.size()-2].time_min;
        x2=points[points.size()-1].time_min;
        y1=points[points.size()-2].operatorEnteredPoint;
        y2=points[points.size()-1].operatorEnteredPoint;
        //   x-x1
        //y=------- * (y2-y1) + y1
        //   x2-x1
        y=(x-x1)*(y2-y1)/(x2-x1) + y1;
        return y;
    }

    for (int i=0;i<points.size()-1;++i)
    {
        if (x>points[i].time_min && x<=points[i+1].time_min)
        {
            x1=points[i].time_min;
            x2=points[i+1].time_min;
            y1=points[i].operatorEnteredPoint;
            y2=points[i+1].operatorEnteredPoint;
            //   x-x1
            //y=------- * (y2-y1) + y1
            //   x2-x1
            y=(x-x1)*(y2-y1)/(x2-x1) + y1;
            return y;
        }

    }

    return 0.0;
}
//=======================================================================================
void MainWindow::CreateTestReport()
{

    QDateTime dtReport=QDateTime::currentDateTime();
    QString fileName=qApp->applicationDirPath()+"/reports/"+dtReport.toString("yyyy.MM.dd_hh.mm.ss")+".odt";


    QTextDocument *document = new QTextDocument();




  //QFont font;
  //font.setFamily("Helvetica");//("Swiss");//("Times New Roman");//("Times");

  QTextCursor cursor(document);

//  QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");

//  setCurrentBlockAlignment(cursor, Qt::AlignLeft);
//  cursor.insertText(date + '\n', charFormat(11, false));//12

//  cursor.insertBlock();
//  setCurrentBlockAlignment(cursor, Qt::AlignCenter);
//  cursor.insertText(company, charFormat(11, false));//12


//


  cursor.insertBlock();
  setCurrentBlockAlignment(cursor, Qt::AlignCenter);
  cursor.insertText("Звіт про результати випробування згідно ДСТУ EN ISO 1182:2022\n", charFormat(14, true));
  //cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n
  cursor.insertText("No00....... (або 17102300......)\n", charFormat(14, true));
  //cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n

  //cursor.insertBlock();
  setCurrentBlockAlignment(cursor, Qt::AlignLeft);
  cursor.insertText("Звіт сформовано: " + dtReport.toString("dd.MM.yyyy hh:mm:ss")+"\n" , charFormat(12, true));//12
  //cursor.movePosition(QTextCursor::End);
  //cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n


  //cursor.insertText("Умова закінчення: " + testStopReason.replace("ЗАВЕРШЕНО...","") , charFormat(12, true));//12  //просто уберем из строки префикс "ЗАВЕРШЕНО..."
  cursor.movePosition(QTextCursor::End);
//  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n

/*
  cursor.insertBlock();
  setCurrentBlockAlignment(cursor, Qt::AlignLeft);
  cursor.insertText("Заміри температури стінок пічі:", charFormat(12, true));//12
*/


  //cursor.movePosition(QTextCursor::End);
  //cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n

  //get 1st graph

  wGraphic_1->replot();
  wGraphic_1->savePng(qApp->applicationDirPath()+"/png/"+startTestDT_str+"_1.png",640,480);
  QImage img_1(qApp->applicationDirPath()+"/png/"+startTestDT_str+"_1.png");


  //graphic
  cursor.movePosition(QTextCursor::End);
  cursor.insertBlock();

  setCurrentBlockAlignment(cursor, Qt::AlignCenter);


  //adding image to document

  //cursor.insertImage(plot_movement_1);

  document->addResource(QTextDocument::ImageResource, QUrl("plot_img_1.png"), img_1);
  QTextImageFormat imageFormat_1;
  imageFormat_1.setQuality(100);
  imageFormat_1.setName("plot_img_1.png");
  cursor.insertImage(imageFormat_1);
  cursor.movePosition(QTextCursor::End);


//  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n
//  cursor.movePosition(QTextCursor::End);



//=====T1
  double T1f=temperature_1.GetValue();
  double T1f_sum=0.0;
  int T1f_cnt=0;
  double T1max=0.0;

  foreach(QCPData cpdata, graphicTemperature_1->data()->values())
  {
        if (cpdata.key >= 0)
        {
            if (cpdata.value>T1max) T1max=cpdata.value;
        }

        //среднее значение за последнюю минуту
        if (cpdata.key>=graphicTemperature_1->data()->values().last().key-60)
        {
            T1f_sum+=cpdata.value;
            T1f_cnt++;
        }
  }
  if (T1f_cnt>0) T1f=T1f_sum/T1f_cnt;

//=====T2
  double T2f=temperature_2.GetValue();
  double T2f_sum=0.0;
  int T2f_cnt=0;
  double T2max=0.0;

  foreach(QCPData cpdata, graphicTemperature_2->data()->values())
  {
        if (cpdata.key >= 0)
        {
            if (cpdata.value>T2max) T2max=cpdata.value;
        }

        //среднее значение за последнюю минуту
        if (cpdata.key>=graphicTemperature_2->data()->values().last().key-60)
        {
            T2f_sum+=cpdata.value;
            T2f_cnt++;
        }
  }
  if (T2f_cnt>0) T2f=T2f_sum/T2f_cnt;


//=====T3
  double T3f=temperature_3.GetValue();
  double T3f_sum=0.0;
  int T3f_cnt=0;
  double T3max=0.0;

  foreach(QCPData cpdata, graphicTemperature_3->data()->values())
  {
        if (cpdata.key >= 0)
        {
            if (cpdata.value>T3max) T3max=cpdata.value;
        }

        //среднее значение за последнюю минуту
        if (cpdata.key>=graphicTemperature_3->data()->values().last().key-60)
        {
            T3f_sum+=cpdata.value;
            T3f_cnt++;
        }
  }
  if (T3f_cnt>0) T3f=T3f_sum/T3f_cnt;


//=====T4
  double T4f=temperature_4.GetValue();
  double T4f_sum=0.0;
  int T4f_cnt=0;
  double T4max=0.0;

  foreach(QCPData cpdata, graphicTemperature_4->data()->values())
  {
        if (cpdata.key >= 0)
        {
            if (cpdata.value>T4max) T4max=cpdata.value;
        }

        //среднее значение за последнюю минуту
        if (cpdata.key>=graphicTemperature_4->data()->values().last().key-60)
        {
            T4f_sum+=cpdata.value;
            T4f_cnt++;
        }
  }
  if (T4f_cnt>0) T4f=T4f_sum/T4f_cnt;

  cursor.insertBlock();
  setCurrentBlockAlignment(cursor, Qt::AlignLeft);

  //cursor.insertHtml("T1 = " + QString::number(temp1_PreTestTavg_stabilized,'f',2)+" °C<br>");// + , charFormat(14, true));//12
  //cursor.insertHtml("T2 = " + QString::number(temp2_PreTestTavg_stabilized,'f',2)+" °C<br>");// + , charFormat(14, true));//12
  cursor.insertHtml("(що є середніми значеннями температури за 10 хв проміжку часу стабілізації, визначеного у 7.2.4)<br>");

  cursor.insertHtml("T1<sub>max</sub> = " + QString::number(T1max,'f',2)+" °C<br>");// , charFormat(14, true));//12
  cursor.insertHtml("T2<sub>max</sub> = " + QString::number(T2max,'f',2)+" °C<br>");// , charFormat(14, true));//12
  cursor.insertHtml("(що є дискретними значеннями за максимальної температури в будь-якій точці упродовж усього проміжку часу випробування)<br>");


  cursor.insertHtml("T1<sub>f</sub> = " + QString::number(T1f,'f',2)+" °C<br>");// , charFormat(14, true));//12
  cursor.insertHtml("T2<sub>f</sub> = " + QString::number(T2f,'f',2)+" °C<br>");// , charFormat(14, true));//12
  cursor.insertHtml("(що є середніми значеннями температури упродовж останньої хвилини проміжку часу випробування відповідно до 7.4.7)<br>");

  cursor.insertHtml("&Delta;T1 = T1<sub>max</sub>-T1<sub>f</sub> = " + QString::number(T1max-T1f,'f',2)+" °C<br>");// , charFormat(14, true));//12
  cursor.insertHtml("&Delta;T2 = T2<sub>max</sub>-T2<sub>f</sub> = " + QString::number(T2max-T2f,'f',2)+" °C<br>");// , charFormat(14, true));//12
  cursor.insertHtml("&Delta;T = (&Delta;T1 - &Delta;T2) / 2  = " + QString::number((T1max-T1f+T2max-T2f)/2.0,'f',2)+" °C<br>");// , charFormat(14, true));//12


  if (ui->checkBoxTemperature3->isChecked() || ui->checkBoxTemperature4->isChecked()) cursor.insertHtml("<br>");

  if (ui->checkBoxTemperature3->isChecked())
  {
      cursor.insertHtml("TC<sub>max</sub> = " + QString::number(T3max,'f',2)+" °C<br>");// , charFormat(14, true));//12
      cursor.insertHtml("(максимальна температура за показами термопари, встановленої в центрі зразка)<br>");
      cursor.insertHtml("TC<sub>final</sub> = " + QString::number(T3f,'f',2)+" °C<br>");// , charFormat(14, true));//12
      cursor.insertHtml("(кінцева температура за показами термопари, встановленої в центрі зразка,)<br>");
  }

  if (ui->checkBoxTemperature4->isChecked())
  {
      cursor.insertHtml("TS<sub>max</sub> = " + QString::number(T4max,'f',2)+" °C<br>");// , charFormat(14, true));//12
      cursor.insertHtml("(максимальна температуру за показами термопари, встановленої на поверхні зразка)<br>");
      cursor.insertHtml("TS<sub>final</sub> = " + QString::number(T4f,'f',2)+" °C<br>");// , charFormat(14, true));//12
      cursor.insertHtml("(кінцева температуру за показами термопари, встановленої на поверхні зразка)<br>");
  }

  if (ui->checkBoxTemperature3->isChecked())
  {
    cursor.insertHtml("&Delta;TC = TC<sub>max</sub>-TC<sub>final</sub> = " + QString::number(T3max-T3f,'f',2)+" °C<br>");// , charFormat(14, true));//12
  }

  if (ui->checkBoxTemperature4->isChecked())
  {
    cursor.insertHtml("&Delta;TS = TS<sub>max</sub>-TS<sub>final</sub> = " + QString::number(T4max-T4f,'f',2)+" °C<br>");// , charFormat(14, true));//12
  }




  cursor.movePosition(QTextCursor::End);
//  cursor.insertText(QObject::tr("\n"), charFormat(12, true));//casey - line \n





  qDebug() << fileName;
  QTextDocumentWriter writer(fileName);
  if (!writer.write(document)) qDebug() << "errro write";

  qDebug() << writer.supportedDocumentFormats();

  //QDesktopServices::openUrl(QUrl(fileName+".odt"));

  //QTextEdit  *te = new QTextEdit();
  //te->setDocument(document);
  //te->showMaximized();

  delete document;





    QMessageBox::information(this,"Збереження в файл",QString("Результати збережені в файл\n")+fileName);
}


//=======================================================================================
void MainWindow::closeEvent(QCloseEvent *event)
{
    /*
    if (ui->buttonPowerOn->isChecked())
    {
        QMessageBox::critical(this,"Увага!!!","Перед закриттям програми вимкніть нагрів печі!!!");
        event->ignore();
    }
*/

}
//=======================================================================================
//=======================================================================================
//=======================================================================================
//=======================================================================================

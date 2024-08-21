#ifndef DIALOGCONFIG_H
#define DIALOGCONFIG_H

#include <QDialog>
#include <QHash>
#include <QtSerialPort/QSerialPortInfo>
#include "dialogcalibr.h"
#include "modbusreader.h"
#include "analoginputchannel.h"

namespace Ui {
class DialogConfig;
}

class DialogConfig : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfig(QWidget *parent = 0);
    ~DialogConfig();
    void SetAnalogInputChannels(QHash<QString, AnalogInputChannel *> hashaichannels);
    void SetModbusReader(ModbusReader *pmbRdr);

    void SetComPort(QString port);
    QString GetComPort();

    Ui::DialogConfig *ui;

private:

    ModbusReader *pmbReader;
    QHash<QString, AnalogInputChannel *> hashAnalogInputChannels;
    DialogCalibr dialogCalibr;

private slots:
    void ButtonCalibr();
    void ButtonOpenCsvFile();
    void ButtonConfigAdmin();
    void ButtonConfigAdminBack();
signals:
    void buttonPageCalibr(bool toggled);

};

#endif // DIALOGCONFIG_H

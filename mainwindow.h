#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QHash>
#include <QMap>

#include "qcustomplot.h"
#include "modbusreader.h"
#include "analoginputchannel.h"
#include "dialogcalibr.h"
#include "dialogconfig.h"
#include "dialogtableresult.h"


#define X_RANGEPRETEST_MIN 0  //seconds стартовая ширина полотна графика по х для режима перед тестов 0 minutes
#define X_RANGEPRETEST_MAX 1800  //seconds стартовая ширина полотна графика по х для режима перед тестов 30 minutes

#define X_RANGETEST_MIN -600  //seconds стартовая ширина полотна графика по х для режима тестов -10 minutes
#define X_RANGETEST_MAX 1800  //seconds стартовая ширина полотна графика по х для режима тестов 30 minutes

//#define X_RANGEVIEW 3600   //стартовая ширина полотна графика по х для режима просмотра
#define X_TICKSTEP 200  //20  //деления на шкале интервал 5 минут

#define Y_HEAT_RANGE_MAX 150  //стартовая ширина полотна графика по y  units mm
#define Y_HEAT_RANGE_MIN 0
#define Y_HEAT_TICKSTEP 10

#define Y_TEMPERATURE_RANGE_MAX 1300  //стартовая ширина полотна графика по y  units mm
#define Y_TEMPERATURE_RANGE_MIN 0
#define Y_TEMPERATURE_TICKSTEP 100

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


enum RunningMode
{
    ModePreTest=0,
    ModeTest,
    ModeTestStopped
};

enum CommandButton
{
    NoCmd=0,
    StartCmd,
    StopCmd
};

struct OperatorPoint
{
    double time_min;
    double operatorEnteredPoint;
    double movement;
    double temperature;
};

//=====================================================================================
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    RunningMode runningMode=ModePreTest;

    CommandButton cmdButton;

    QDateTime startPreTestDT;
    QString startPreTestDT_str;
    QDateTime startTestDT;
    QString startTestDT_str;

    QString csvFileName;



//    QVector<OperatorPoint> levelUpPoints_1;
//    QVector<OperatorPoint> levelDownPoints_1;
//    QVector<OperatorPoint> levelUpPoints_2;
//    QVector<OperatorPoint> levelDownPoints_2;

    QPalette orig_palette;

    QCustomPlot *wGraphic_1;

    QCPGraph *graphicTemperature_1;
    QCPGraph *graphicTemperature_2;
    QCPGraph *graphicTemperature_3;
    QCPGraph *graphicTemperature_4;
    QCPGraph *graphicTemperature_5;
    QCPGraph *graphicTemperature_6;
    QCPGraph *graphicTemperature_7;
    QCPGraph *graphicTemperature_8;


    bool eventFilter(QObject *obj, QEvent *event);


    DialogConfig dialogConfig;


    QString iniFile;
    QString comPort;

    QTimer timer1000ms;

    AnalogInputChannel temperature_1;
    AnalogInputChannel temperature_2;
    AnalogInputChannel temperature_3;
    AnalogInputChannel temperature_4;
    AnalogInputChannel temperature_5;
    AnalogInputChannel temperature_6;
    AnalogInputChannel temperature_7;
    AnalogInputChannel temperature_8;

    QHash<QString, AnalogInputChannel *> hashAnalogInputChannels;

    void LoadIniFile(QString iniFileName);
    void SaveIniFileComPort();
    void SaveIniFileOperator();

    //reports
    void setCurrentBlockAlignment(QTextCursor &cursor,
                                  Qt::Alignment alignment);
    QTextCharFormat charFormat(int pointSize,
                               bool bold);

    void createReport(const QString &fileName,
                                const QString &startDT,
                                const QString &timeDT,
                                const QImage &plot_movement_1,
                                const double X_1,
                                const double X_2
                                );





    double getLinearApproximatedValue(QVector<OperatorPoint> points, double x);

private slots:

    void Timer1000ms();
    void AddCsvMessageColumns();
    void AddCsvMessage(QString message="");

    double calcAverage(QVector<double> vec);

    bool calcAvgMinMaxRegress(const QVector<QCPData> &data, double &avg, double &min, double &max, double &regress, double &regress_koeff_a, double &regress_koeff_b);



    void ButtonExit();
    void ButtonReset();
    void ButtonStartStop();
    void ButtonReports();
    void ButtonTrendZoomOnOff(bool toggled);
    void ViewDialogConfig();




    //void CheckBoxTemperature1Changed(bool newState);
    //void CheckBoxTemperature2Changed(bool newState);
    //void CheckBoxTemperature3Changed(bool newState);
    //void CheckBoxTemperature4Changed(bool newState);

    void CreateTestReport();


    void ValueChanged(QString str);

    void slotRangeChanged (const QCPRange &newRange);
    void closeEvent(QCloseEvent *event);

};
//======================================================================================
class MyValidator : public QDoubleValidator
{
    public:
    MyValidator(double bottom, double top, int decimals, QObject * parent) :
    QDoubleValidator(bottom, top, decimals, parent)
    {
    }

    QValidator::State validate(QString &s, int &i) const
    {
        if (s.isEmpty() || s == "-") {
            return QValidator::Intermediate;
        }

        //QLocale locale;

        QChar decimalPoint = '.';//locale.decimalPoint();

        int charsAfterPoint = s.length() - s.indexOf(decimalPoint) -1;

        if (charsAfterPoint > decimals() && s.indexOf(decimalPoint) != -1) {
            return QValidator::Invalid;
        }

        bool ok;
        double d = s.toDouble(&ok);//locale.toDouble(s, &ok);

        if (ok && d >= bottom() && d <= top()) {
            return QValidator::Acceptable;
        } else {
            return QValidator::Invalid;
        }
    }
};

#endif // MAINWINDOW_H

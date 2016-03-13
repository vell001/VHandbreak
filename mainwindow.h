#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QtSerialPort>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QSettings>

#include "stdafx.h"

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "public.h"
#include "vjoyinterface.h"

#include "Math.h"

// Default device ID (Used when ID not specified)
#define DEV_ID		1
const QString KEY_port = "port";
const QString KEY_baudrate = "baudrate";
const QString KEY_devId = "devId";

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_comboBoxCOM_currentIndexChanged(const QString &arg1);

    void on_btnStart_clicked();

    void on_comboBoxBaudRates_currentIndexChanged(const QString &arg1);
    void readSerialData();

    void on_btnStop_clicked();

private:
    Ui::MainWindow *ui;
    QSettings settings;
    JOYSTICK_POSITION_V2 iReport; // The structure that holds the full position data
    bool isStop;
    UINT DevID;
    QSerialPortInfo COMPort;
    qint32 boudRate;
    QSerialPort serial;
    int hx; // hardware x value
    int hxMax = 960;
    int hxMin = 840;
};

#endif // MAINWINDOW_H

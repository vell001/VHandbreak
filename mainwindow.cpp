#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings("vell001", "handbreak")
{
    ui->setupUi(this);
    isStop = true;

    if (!vJoyEnabled()) {
        qDebug() << "vjoy not enabled";
    } else {
        qDebug() << "vjoy enabled";
        qDebug() << "Vendor: " << * static_cast<wchar_t *>(GetvJoyManufacturerString())
                 << "\nProduct :"<< * static_cast<wchar_t *>(GetvJoyProductString())
                 <<"\nVersion Number:"<< * static_cast<wchar_t *>(GetvJoySerialNumberString()) <<"\n";
    }

    DevID = DEV_ID;

    // Get the status of the vJoy device before trying to acquire it
    VjdStat status = GetVJDStatus(DevID);

    switch (status)
    {
    case VJD_STAT_OWN:
        qDebug() << "vJoy device "<<DevID<<" is already owned by this feeder\n";
        break;
    case VJD_STAT_FREE:
        qDebug() << "vJoy device "<<DevID<<" is free\n";
        break;
    case VJD_STAT_BUSY:
        _tprintf("vJoy device %d is already owned by another feeder\nCannot continue\n", DevID);
        return;
    case VJD_STAT_MISS:
        _tprintf("vJoy device %d is not installed or disabled\nCannot continue\n", DevID);
        return;
    default:
        _tprintf("vJoy device %d general error\nCannot continue\n", DevID);
        return;
    };

    QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList) {
        ui->comboBoxCOM->addItem(serialPortInfo.portName() + ":" + serialPortInfo.description());
    }
    QString portValue = settings.value(KEY_port,"").toString();
    ui->comboBoxCOM->setCurrentText(settings.value(KEY_port,"").toString());
//    ui->comboBoxCOM->setCurrentText("COM15:Bluetooth 链接上的标准串行");

    QList<qint32> rates = QSerialPortInfo::standardBaudRates();
    foreach (qint32 rete, rates) {
        ui->comboBoxBaudRates->addItem(QString::number(rete));
    }
    ui->comboBoxBaudRates->setCurrentText(settings.value(KEY_baudrate,"").toString());

    for(int id = 1; id <= 16; id ++) {
        ui->comboBoxDEVS->addItem(QString::number(id));
    }

    connect(&serial, SIGNAL(readyRead()), this, SLOT(readSerialData()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBoxCOM_currentIndexChanged(const QString &comStr)
{
    settings.setValue(KEY_port, comStr);
    qDebug() << "selected " << comStr;
    COMPort = QSerialPortInfo(comStr.mid(0, comStr.indexOf(":")));
    qDebug() << "selected " << COMPort.portName();
}


void MainWindow::on_comboBoxBaudRates_currentIndexChanged(const QString &rateStr)
{
    settings.setValue(KEY_baudrate, rateStr);
    qDebug() << "selected " << rateStr;
    boudRate = rateStr.toInt();
}

void MainWindow::on_btnStart_clicked()
{
    // 读取串口参数
    serial.setPort(COMPort);
    serial.setBaudRate(boudRate);

    if(!serial.open(QIODevice::ReadWrite)) {
        qDebug() << "串口无法打开：" << COMPort.portName() << ":" << boudRate;
        return;
    }

    iReport.bDevice = (BYTE)DevID;

    isStop = false;
    ui->btnStart->setEnabled(false);
}

void MainWindow::on_btnStop_clicked()
{
    if (serial.isOpen()){
        serial.close();
    }

    isStop = true;
    ui->btnStart->setEnabled(true);
}

void MainWindow::readSerialData() {
    QByteArray data;
    int hxAvg = hxMax - hxMin;
    while (!isStop && serial.canReadLine())
    {
        data = serial.readLine();
        // 做简单的提取，以后升级正则
        int i = data.indexOf(":")+1;
        int j = data.indexOf("]");
        hx = data.mid(i,j-i).toInt();

        // 将电位器读取的值转换到vjoy的值
        int tmpx = ((float)(hx-hxMin)/hxAvg) * 32768;

        if(tmpx < 1) tmpx = 1;
        if(tmpx > 32768) tmpx = 32768;

        if (!SetAxis(tmpx,DevID, HID_USAGE_SL0))
        {
            qDebug() << "Feeding vJoy device number "<< DevID <<" failed";
            AcquireVJD(DevID);
        }
    }
}


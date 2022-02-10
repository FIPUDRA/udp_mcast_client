 #include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkInterface>
#include <QNetworkDatagram>
#include <string>
#include <QFile>
#include <QTextStream>
#include <QVector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QUdpSocket(this);
    qDebug() << socket->bind(QHostAddress::AnyIPv4, 32768);
    for(auto& iface : QNetworkInterface::allInterfaces()) {
            QNetworkInterface::InterfaceFlags flags = iface.flags();
            if(flags.testFlag(QNetworkInterface::CanMulticast)){
                qDebug() << iface;
                socket->joinMulticastGroup(QHostAddress("239.1.2.3"), iface);
            }
    }
    connect(socket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);
    this->ui->customplot->addGraph();
    writeInFile();
}

void MainWindow::readPendingDatagrams()
{
    while (socket->hasPendingDatagrams()&(this->ui->okButton->text() == "Stop")) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        QString tempDatagram = static_cast<QString>(datagram.data().toHex());

        int currentChannel = this->ui->comboBox->currentIndex();

        QString num = tempDatagram.mid(4,4);
        QString data = tempDatagram.mid(32+(currentChannel*8),8);

        Char_to_Float ctof;
        ctof.i = data.toUInt(nullptr,16);

//        if (this->ui->intButton->isChecked())
//            this->ui->textEdit->append(QString::number(num.toInt(nullptr,16)) +"\t"+ QString::number(ctof.i));
//        else if (this->ui->floatButton->isChecked())
//            this->ui->textEdit->append(QString::number(num.toInt(nullptr,16)) +"\t"+ QString::number(ctof.f));

//        QString str = QString::number(num.toInt(nullptr,16)) +","+ QString::number(ctof.f) +","+ QString::number(ctof.i)+"\n";
//        writeInFile(str);
        buildAGraph(num.toInt(nullptr,16),ctof);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_okButton_clicked()
{
    if (this->ui->okButton->text() == "Start"){
        this->ui->okButton->setText("Stop");
        readPendingDatagrams();
    }
    else {
        this->ui->okButton->setText("Start");
        socket->pauseMode();
    }
}

void MainWindow::on_cancelButton_clicked()
{
    close();
}

void MainWindow::writeInFile(QString str)
{
    QFile fileOut("recivedDatagrams.csv");
    if(fileOut.open(QIODevice::Append | QIODevice::Text)){
        QTextStream writeStream(&fileOut);
        writeStream << str;
        fileOut.close();
    }
    else
    {
        qDebug() << "writeStream ERROR";
    }
}

void MainWindow::writeInFile()
{
    QFile fileOut("recivedDatagrams.csv");
    if(fileOut.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream writeStream(&fileOut);
        writeStream <<"";
        fileOut.close();
    }
    else
    {
        qDebug() << "writeStream ERROR";
    }
}

void MainWindow::buildAGraph(int xValue, Char_to_Float yValue)
{

    static QVector<double> x(this->ui->horizontalSlider->value(),0);
    static QVector<double> y(this->ui->horizontalSlider->value(),0);
    static long int min = 0, max = 0;

    while (x.size() >= (this->ui->horizontalSlider->value())){
        x.pop_front();
        y.pop_front();
    }
    x.push_back((double)xValue);
    if (this->ui->intButton->isChecked()){
        y.push_back((double)yValue.i);
    }
    if (this->ui->floatButton->isChecked()){
        y.push_back((double)yValue.f);
    }
//    qDebug()<< x.last() << "\tcut\tsizeX: " << x.size()<< "\tfirstIX: " << x.indexOf(x.first())  << "\tlastIX: " << x.indexOf(x.last()) << "\t|\tsizeY: " << y.size() << "\tfirstIY: " << y.indexOf(y.first()) << " " << y.first() << "\tlastIY: " << y.indexOf(y.last()) << " " << y.last();
    if (y.last() > max) max=y.last();
    if (y.last() < min) min=y.last();



    this->ui->customplot->graph(0)->setData(x,y);
    this->ui->customplot->xAxis->setRange(x.first(), x.last());
    this->ui->customplot->yAxis->setRange(min, max);
    this->ui->customplot->graph(0)->rescaleAxes();
    this->ui->customplot->replot();
}

void MainWindow::on_clearButton_clicked()
{
    this->ui->customplot->clearGraphs();
    this->ui->textEdit->clear();
}

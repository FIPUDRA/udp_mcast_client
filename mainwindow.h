#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void readPendingDatagrams();
private slots:
    void on_okButton_clicked();

    void on_cancelButton_clicked();

    void on_clearButton_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket* socket = nullptr;
    union Char_to_Float
    {
        float f;
        unsigned long int i;
        unsigned char c[4];
    };

    void writeInFile(QString str);
    void writeInFile();

    void buildAGraph(int xValue,  Char_to_Float yValue);
};
#endif // MAINWINDOW_H

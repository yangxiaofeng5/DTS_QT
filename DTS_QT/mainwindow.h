#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <qcustomplot.h> // the header file of QCustomPlot. Don't forget to add it to your project, if you use an IDE, so it gets compiled.
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QMouseEvent>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void serialport_init();
    void read_serialport();     //读串口数据
    void find_serialport();     //查找串口

    void RealtimeDataShow(QCustomPlot *customPlot);

private slots:
    void onMouseMove(QMouseEvent* event);

    void realtimeDataSlot();

    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_actionHelp_triggered();

    void checkbox_change();

    void on_onlineButton_clicked();

    void on_offlineButton_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_saveButton_clicked();

    void on_checkBox_2_stateChanged(int arg1);

    void on_checkBox_full_stateChanged(int arg1);

    void on_checkBox_axis_stateChanged(int arg1);

    void on_checkBox_scroll_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QString demoName;
    QTimer dataTimer;           //时间
    QSerialPort *serialport;    //串口类

    QCPItemText *textItem;


};
#endif // MAINWINDOW_H

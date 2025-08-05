#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtNetwork>
#include <QHostAddress>
#include <mqtt/qmqtt.h>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);// 主窗口构造函数。
    ~MainWindow();// 主窗口析构函数。

    void clientMqtt();// MQTT 客户端初始化函数。
private:
    Ui::MainWindow *ui;// 主窗口界面对象。
    QMQTT::Client *client; // 创建QMQTT 客户端

private slots:
   void onMQTT_Received(QMQTT::Message);// 处理 MQTT 消息接收的槽函数。
   void onMQTT_subscribed( QString, quint8);// 处理 MQTT 订阅成功的槽函数。
   void onMQTT_unSubscribed( QString );// 处理 MQTT 取消订阅的槽函数。
   void sendTopic(QString data);// 发布 MQTT 主题消息的函数。
   void on_pushButton_clicked();// 处理按钮1点击事件的槽函数。
   void on_pushButton_2_clicked();// 处理按钮2点击事件的槽函数。
   void on_pushButton_3_clicked();// 处理按钮3点击事件的槽函数。
   void on_pushButton_4_clicked();
   void on_pushButton_5_clicked();
   void on_pushButton_6_clicked();
   void on_pushButton_7_clicked();
   void on_pushButton_8_clicked();
};
#endif // MAINWINDOW_H

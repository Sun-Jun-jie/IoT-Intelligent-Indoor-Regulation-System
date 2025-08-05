#include "mainwindow.h"
#include "ui_mainwindow.h"
//物联22409 孙浚杰 072240913
// JSON 字符串，消息载荷。
QString aa = "\"/sys/k1ozvJXrRxK/QT_dev/thing/service/property/set\" payload: \"{\"deviceType\":\"CustomCategory\",\"iotId\":\"2JIEMH9lq6mV9IiMrkZh000000\",\"requestId\":\"null\",\"checkFailedData\":{},\"productKey\":\"a1MiW5W8ufE\",\"gmtCreate\":1690348115173,\"deviceName\":\"stm32\",\"items\":{\"tu_d\":{\"time\":1690348115150,\"value\":20},\"tu\":{\"time\":1690348115150,\"value\":38},\"temp\":{\"time\":1690348115150,\"value\":30},\"humi_d\":{\"time\":1690348115150,\"value\":30},\"co2\":{\"time\":1690348115150,\"value\":0},\"humi\":{\"time\":1690348115150,\"value\":55},\"lux_d\":{\"time\":1690348115150,\"value\":1000},\"co2_d\":{\"time\":1690348115150,\"value\":60},\"temp_d\":{\"time\":1690348115150,\"value\":20},\"temp_u\":{\"time\":1690348115150,\"value\":30},\"lux\":{\"time\":1690348115150,\"value\":284}}}\"";
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    clientMqtt();// 调用 MQTT 客户端初始化函数。
}

MainWindow::~MainWindow()
{
    delete ui;
}
// 定义MQTT 客户端初始化函数。
void MainWindow::clientMqtt(){
    client = new QMQTT::Client(); // 初始化QMQTT客户指针

    QHostAddress host("k1ozvJXrRxK.iot-as-mqtt.cn-shanghai.aliyuncs.com"); // MQTT代理服务器 IP
    QByteArray password = "your password"; // 密码，1改成自己的。
    quint16 port = 1883; // 代理服务器端口
    QString deviceId = "your deviceId"; // 连接 ID，2改成自己的。
    QString productId = "your productId";   //连接名称，3ProductKey改成自己的。

    client->setKeepAlive(120); // 设置 MQTT 保持活动间隔。心跳
    client->setHostName("k1ozvJXrRxK.iot-as-mqtt.cn-shanghai.aliyuncs.com");//4ProductKey改成自己的。
    client->setPort(port); // 设置 EMQ 代理服务器端口
    client->setClientId(deviceId); // 设备 ID
    client->setUsername(productId); // 产品 ID
    client->setPassword(password);
    client->cleanSession();
    client->setVersion(QMQTT::MQTTVersion::V3_1_1); // 设置mqtt版本

    client->connectToHost(); // 连接 EMQ 代理服务器
    client->subscribe("your topic");//5ProductKey改成自己的。

    connect(client,&QMQTT::Client::received,this,&MainWindow::onMQTT_Received);

}
// 定义MQTT 客户端接收函数。
void MainWindow::onMQTT_Received(QMQTT::Message message) {
    QString jsonStr = message.payload();
    qDebug() << jsonStr;

    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());

    if (!doc.isNull() && doc.isObject()) {
        QJsonObject obj = doc.object();

        // 检查 "items" 键是否存在并且是对象
        if (obj.contains("items") && obj["items"].isObject()) {
            QJsonObject itemsObj = obj["items"].toObject();

            // 提取温度值
            if (itemsObj.contains("temp") && itemsObj["temp"].isObject()) {
                QJsonObject tempObj = itemsObj["temp"].toObject();
                float tempValue = tempObj["value"].toDouble();
                qDebug() << "提取出了温度:" << tempValue;
                ui->textEdit_2->setText(QString::number(tempValue) + " ℃");
            } else {
                qDebug() << "温度数据不可用";
                //ui->textEdit_2->setText("N/A");
            }

            // 提取湿度值
            if (itemsObj.contains("humi") && itemsObj["humi"].isObject()) {
                QJsonObject humiObj = itemsObj["humi"].toObject();
                int humiValue = humiObj["value"].toInt();
                qDebug() << "提取出了湿度:" << humiValue;
                ui->textEdit->setText(QString::number(humiValue) + " %");
            } else {
                qDebug() << "湿度数据不可用";
                //ui->textEdit->setText("N/A");
            }

            // 提取二氧化碳值
            if (itemsObj.contains("co2") && itemsObj["co2"].isObject()) {
                QJsonObject humiObj = itemsObj["co2"].toObject();
                int co2value = humiObj["value"].toInt();
                qDebug() << "提取出了CO2:" << co2value;
                ui->textEdit_4->setText(QString::number(co2value) + " ppm");
            } else {
                qDebug() << "co2数据不可用";
                //ui->textEdit_4->setText("N/A");
            }

            // 提取光照值
            if (itemsObj.contains("light") && itemsObj["light"].isObject()) {
                QJsonObject lightObj = itemsObj["light"].toObject();
                int lightValue = lightObj["value"].toInt();
                qDebug() << "提取出了光照:" << lightValue;
                ui->textEdit_3->setText(QString::number(lightValue) + " lux");
            } else {
                qDebug() << "光照数据不可用";
                //ui->textEdit_3->setText("N/A");
            }

            // 提取舒适度值
            if (itemsObj.contains("comfort") && itemsObj["comfort"].isObject()) {
                QJsonObject humiObj = itemsObj["comfort"].toObject();
                float comfort = humiObj["value"].toDouble();
                qDebug() << "提取出了comfort:" << comfort;
                ui->textEdit_5->setText(QString::number(comfort));
            } else {
                qDebug() << "舒适度数据不可用";
                //ui->textEdit_4->setText("N/A");
            }

        } else {
            qDebug() << "JSON 数据中没有 'items' 对象";
        }
    } else {
        qDebug() << "无效的 JSON 数据";
    }
}


void MainWindow::onMQTT_subscribed(QString topic, quint8 qos) {
    qDebug() << "\n订阅topic = " << topic << "Qos = " << qos << " 成功!";
}

void MainWindow::onMQTT_unSubscribed(QString topic) {
    qDebug() << "\nonMQTT_unSubscribed:" << topic;
}
// 将数据发布到 MQTT 主题。
void MainWindow::sendTopic(QString data){
    client->publish(QMQTT::Message(136,"/k1ozvJXrRxK/QT_dev/user/update",data.toUtf8()));//7ProductKey改成自己的。
}

// 风扇 开/关
void MainWindow::on_pushButton_clicked()
{
    static bool fanState = 0;
    fanState = !fanState;
    if(fanState)
        sendTopic("9101"); //910 风扇 01 开
    else
        sendTopic("9100"); //910 风扇 00 关
}

// 加湿器 开/关
void MainWindow::on_pushButton_2_clicked()
{
    static bool humiState = false;
    humiState = !humiState;
    if(humiState)
        sendTopic("201");
    else
        sendTopic("200");
}

// 空调 关
void MainWindow::on_pushButton_3_clicked()
{
    sendTopic("600");
}

// 空调 制冷
void MainWindow::on_pushButton_4_clicked()
{
    sendTopic("601");
}

// 空调 制热
void MainWindow::on_pushButton_5_clicked()
{
    sendTopic("602");
}

// 环境调控 自动/手动
void MainWindow::on_pushButton_6_clicked()
{
    static bool autoMode = false;
    autoMode = !autoMode;
    if(autoMode)
        sendTopic("301");
    else
        sendTopic("300");
}

// 补光 开/关
void MainWindow::on_pushButton_7_clicked()
{
    static bool lightState = false;
    lightState = !lightState;
    if(lightState)
        sendTopic("9401");
    else
        sendTopic("9400");
}

void MainWindow::on_pushButton_8_clicked()
{
    static bool buzz = false;
    buzz = !buzz;
    if(buzz)
        sendTopic("501");
    else
        sendTopic("500");
}

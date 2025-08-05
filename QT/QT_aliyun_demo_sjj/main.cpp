#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);// 创建 Qt 应用程序对象。
    MainWindow w;// 创建主窗口对象。
    w.show();// 显示主窗口。
    return a.exec();// 启动 Qt 应用程序事件循环，等待用户交互和应用程序事件。
}

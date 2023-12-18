#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include <QObject>
#include <QTcpSocket>

class NetWorkThread : public QObject
{
    Q_OBJECT
public:
    explicit NetWorkThread(QObject *parent = nullptr);

    //连接服务器
    void connectServer();

    //发送消息 通用
    void sendMsg(QString msg);



    QTcpSocket *m_tcp;

signals:
    void connectOK();
    void connectNO();
    void readMsgYES(QString msg);
};

#endif // NETWORKTHREAD_H

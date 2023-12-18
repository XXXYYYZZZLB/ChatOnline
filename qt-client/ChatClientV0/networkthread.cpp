#include "networkthread.h"
#include <QDebug>
#include <QHostAddress>

NetWorkThread::NetWorkThread(QObject *parent)
    : QObject{parent}
{
    m_tcp = new QTcpSocket(this);

    connect(m_tcp,&QTcpSocket::connected,this,[=](){
        emit connectOK();
    });
    connect(m_tcp,&QTcpSocket::disconnected,this,[=](){
        m_tcp->abort();
        m_tcp->close();
        m_tcp->deleteLater();
        emit connectNO();
    });
    connect(m_tcp,&QTcpSocket::readyRead,this,[=](){
        QString msg = m_tcp->readAll();
        emit readMsgYES(msg);
    });
}


void NetWorkThread::connectServer(){
    qDebug()<<"连接服务器";
    QString ip = "1.92.95.24";
    unsigned short port = 6000;
    m_tcp->connectToHost(QHostAddress(ip),port);

}

void NetWorkThread::sendMsg(QString msg){
    qDebug()<<"发送消息";
    m_tcp->write(msg.toUtf8());
}

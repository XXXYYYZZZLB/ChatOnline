#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDebug>
#include <QMessageBox>
#include "json.h"
#include "publicmsg.h"
#include <string>

using namespace std;
using json = nlohmann::json;

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowFlags (Qt::FramelessWindowHint);
    QImage *img=new QImage;
    img->load("C:\\Users\\libinnn\\Desktop\\loginbg.png");
    ui->label_bg->setPixmap(QPixmap::fromImage(*img));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

//登录
void LoginDialog::on_pushButton_clicked()
{
    //string s = qstr.toStdString();
    //QString qstr2 = QString::fromStdString(s);

    QString useridStr = ui->lineEdit_userid->text();
    QString qPwd = ui->lineEdit_pwd->text();
    bool ok;
    int userid = useridStr.toInt(&ok);
    if(!ok){
        QMessageBox::critical(this, tr("注意"),  tr("用户序号必须是数字"),QMessageBox::Ok,  QMessageBox::Ok);
        return;
    }
    string pwd = qPwd.toStdString();
    json request;
    request["msgid"] = LOGIN_MSG;
    request["id"] = userid;
    request["password"] = pwd;

    QString qRequestStr = QString::fromStdString(request.dump());
    //QString msg = R"({"msgid":4,"password":"11111","id":22})";
    qDebug()<<qRequestStr;
    emit login(qRequestStr);

}

//取消
void LoginDialog::on_pushButton_2_clicked()
{
    done(0);
}

//注册
void LoginDialog::on_pushButton_3_clicked()
{
    done(2);
}

void LoginDialog::loginOK(){
    //登录成功，退出这个界面
    done(1);
}

void LoginDialog::loginNO(QString errMsg){
    QMessageBox::critical(this, tr("提示"),  tr(errMsg.toStdString().c_str()),QMessageBox::Ok,  QMessageBox::Ok);
    ui->lineEdit_userid->setText("");
    ui->lineEdit_pwd->setText("");
}





#include "additemdialog.h"
#include "ui_additemdialog.h"
#include "publicmsg.h"
#include "currentuserinfo.h"
#include "json.h"

#include <QMessageBox>

AddItemDialog::AddItemDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addItemDialog)
{
    ui->setupUi(this);
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
}

void AddItemDialog::setInfo(const QString& titleText,const QString& labelText,const QString& pushbuttonText){
    m_titleText = titleText;
    m_labelText = labelText;
    m_pushbuttonText = pushbuttonText;

    ui->lb_title->setText(m_titleText);
    ui->lb_label->setText(m_labelText);
    ui->pb_lable->setText(m_pushbuttonText);
}

void AddItemDialog::setMsgFlag(int msgFlag){
    m_msgFlag = msgFlag;
}

void AddItemDialog::on_pb_lable_clicked()
{
    //发送数据
    nlohmann::json addMsgJs;
    addMsgJs["msgid"] = m_msgFlag;
    addMsgJs["id"] = CurrentUserInfo::GetInstance().getUserId();
    if(m_msgFlag == ADD_FRIEND_MSG){
        QString friendIdStr = ui->lineEdit->text();
        bool ok;
        int friendid = friendIdStr.toInt(&ok);
        if(!ok){
            QMessageBox::critical(this, tr("注意"),  tr("friendid必须是数字"),QMessageBox::Ok,  QMessageBox::Ok);
            return;
        }
        addMsgJs["friendid"] = friendid;
    }else if(m_msgFlag == ADD_GROUP_MSG){
        QString groupIdStr = ui->lineEdit->text();
        bool ok;
        int groupid = groupIdStr.toInt(&ok);
        if(!ok){
            QMessageBox::critical(this, tr("注意"),  tr("groupid必须是数字"),QMessageBox::Ok,  QMessageBox::Ok);
            return;
        }
        addMsgJs["groupid"] = groupid;
    }else if(m_msgFlag == CREATE_GROUP_MSG){
        QString groupname = ui->lineEdit->text();
        addMsgJs["groupname"] = groupname.toStdString();
        addMsgJs["groupdesc"] = "groupdesc";
    }

    QString addMsg = QString::fromStdString(addMsgJs.dump());
    emit clickAddItem(addMsg);
    ui->pb_lable->setDisabled(true);
    done(1);
}

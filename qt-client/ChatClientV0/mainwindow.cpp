#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkthread.h"
#include "logindialog.h"
#include <QString>
#include <QHostAddress>
#include <QDebug>
#include <QThread>
#include "json.h"
#include "publicmsg.h"
#include "msgcontent.h"
#include "currentuserinfo.h"

#include <QMessageBox>


using namespace std;
using json = nlohmann::json;


static string getCurrentTime()
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
            (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initStartUi();
    initSubNetThread();

    emit connServer();
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initStartUi(){
    this->resize( QSize( 800, 500 ));
    disableAll();
}


void MainWindow::initSubNetThread(){


    QThread *t = new QThread;
    m_worker = new NetWorkThread;
    m_worker->moveToThread(t);

    //连接服务器
    connect(this,&MainWindow::connServer,m_worker,&NetWorkThread::connectServer);
    //发送消息
    connect(this,&MainWindow::sendMsgToServer,m_worker,&NetWorkThread::sendMsg);
    //服务器连接成功
    connect(m_worker,&NetWorkThread::connectOK,this,[=](){
        connServerDisplay();
    });
    //服务器连接失败
    connect(m_worker,&NetWorkThread::connectNO,this,[=](){
        qDebug()<<"断开连接";
        disableAll();
        ui->pb_connserver->setDisabled(false);
        ui->lb_loginstate->setText("断开");
        ui->lb_serverstate->setText("丢失连接");
        t->quit();
        t->wait();
        m_worker->deleteLater();
        t->deleteLater();
    });
    connect(m_worker,&NetWorkThread::readMsgYES,this,&MainWindow::reciveMsgFromServer);


    //开启线程
    t->start();
}

void MainWindow::disableAll(){
    ui->pb_login->setDisabled(true);
    ui->pb_logout->setDisabled(true);
    ui->le_userid->setDisabled(true);
    ui->le_pwd->setDisabled(true);
    ui->pb_addfriend->setDisabled(true);
    ui->pb_addgroup->setDisabled(true);
    ui->pb_creategroup->setDisabled(true);
    ui->pb_sendmsg->setDisabled(true);
    ui->pb_clearmsg->setDisabled(true);
    ui->te_sendmsg->setDisabled(true);
}

void MainWindow::connServerDisplay(){
    ui->le_userid->setDisabled(false);
    ui->le_pwd->setDisabled(false);
    ui->pb_login->setDisabled(false);
    ui->lb_serverstate->setText("已连接");
    ui->pb_connserver->setDisabled(true);

    //聊天界面清除
    ui->tb_allmsg->clear();

    m_currentUserFriendList.clear();
    m_currentUserGroupList.clear();
}
void MainWindow::loginDisplay(){
    ui->le_pwd->setDisabled(true);
    ui->le_userid->setDisabled(true);
    ui->pb_login->setDisabled(true);
    ui->pb_logout->setDisabled(false);
    ui->pb_addfriend->setDisabled(false);
    ui->pb_addgroup->setDisabled(false);
    ui->pb_creategroup->setDisabled(false);
    ui->pb_sendmsg->setDisabled(false);
    ui->pb_clearmsg->setDisabled(false);
    ui->te_sendmsg->setDisabled(false);
    ui->lb_loginstate->setText("在线");
    ui->lb_username->setText(m_name);


    //渲染好友和群组
    updateFriendlist();
    updateGrouplist();
}

void MainWindow::reciveMsgFromServer(QString msg){
    qDebug()<<"收到数据"<<msg;
    //在此给消息进行分流
    string strMsg = msg.toStdString();
    json js = json::parse(strMsg);
    int msgId = js["msgid"];

    //登录ACK
    if(msgId == LOGIN_MSG_ACK){
        loginMsgHandler(js);
    }else if(msgId == REG_MSG_ACK){//注册ACK
        qDebug()<<"注册成功";
    }else if(msgId == ONE_CHAT_MSG){//点对点消息
        oneChatMsgHandler(js);
    }else if(msgId == ADD_FRIEND_MSG_ACK){//添加好友消息
        addFriendMsgHandler(js);
    }else if(msgId == CREATE_GROUP_MSG_ACK){//创建加入群组回复
        addOrCreateGroupMsgHandler(js);
    }else if(msgId == GROUP_CHAT_MSG){//群聊天
        groupChatMsgHandler(js);
    }else{
        qDebug()<<"未知消息";
    }



}

void MainWindow::on_pb_login_clicked()
{
    QString useridStr = ui->le_userid->text();
    QString qPwd = ui->le_pwd->text();
    bool ok;
    int userid = useridStr.toInt(&ok);
    if(!ok){
        QMessageBox::critical(this, tr("提示"),  tr("用户序号必须是数字"),QMessageBox::Ok,  QMessageBox::Ok);
        return;
    }
    string pwd = qPwd.toStdString();

    json request;
    request["msgid"] = LOGIN_MSG;
    request["id"] = userid;
    request["password"] = pwd;

    QString qRequestStr = QString::fromStdString(request.dump());
    emit sendMsgToServer(qRequestStr);
    qDebug()<<qRequestStr;
}


void MainWindow::on_pb_connserver_clicked()
{
    qDebug()<<"重新连接";
    emit connServer();
}


void MainWindow::updateFriendlist(){
    ui->lv_friend->clear();
    m_friendid2Itemindex.clear();
    m_index2friendid.clear();
    int index = 0;
    for(auto fd:m_currentUserFriendList){
        QIcon icon(":/friendicon.png");
        QListWidgetItem *item = new QListWidgetItem(icon,QString::fromStdString(fd.getName()));
        qDebug()<<QString::fromStdString(fd.getName());
        ui->lv_friend->addItem(item);
        m_friendid2Itemindex[fd.getId()] = index;
        m_index2friendid[index] = fd.getId();
        index++;
    }
}
void MainWindow::updateGrouplist(){
    ui->lv_group->clear();
    m_groupid2Itemindex.clear();
    m_index2groupid.clear();
    int index = 0;
    for(auto gp:m_currentUserGroupList){
        QIcon icon(":/groupicon.png");
        QListWidgetItem *item = new QListWidgetItem(icon,QString::fromStdString(gp.getName()));
        qDebug()<<QString::fromStdString(gp.getName());
        ui->lv_group->addItem(item);
        m_groupid2Itemindex[gp.getId()] = index;
        m_index2groupid[index] = gp.getId();
        index++;
    }
}

void MainWindow::oneChatMsgHandler(const nlohmann::json &jsmsg){
    qDebug()<<"oneChatMsgHandler";
    string time = jsmsg["time"].get<string>();
    int  friendid = jsmsg["id"].get<int>();
    string name = jsmsg["name"].get<string>();
    string msg = jsmsg["msg"].get<string>();

    qDebug()<<QString::fromStdString(msg);
    MsgContent msginfo(friendid,name,msg,time);
    if(m_friendAllMsg.count(friendid)){
        //不是第一次发来消息
        m_friendAllMsg[friendid].push_back(msginfo);


    }else {
        m_friendAllMsg[friendid] = std::vector<MsgContent>(0);
        m_friendAllMsg[friendid].push_back(msginfo);
    }

    //如果当前打开的就是这个id的聊天框
    if(friendid == m_nowFriendIdChat && m_nowShowChat < 0){
        updateFriendChatView(friendid);
    }else{
        //把对应的聊天名称加上提醒标记
        int index = m_friendid2Itemindex[friendid];
        QString tempName = ui->lv_friend->item(index)->text();
        if(tempName.right(1) != "*"){
            ui->lv_friend->item(index)->setText(tempName+"*");
        }
    }
}


void MainWindow::groupChatMsgHandler(const nlohmann::json &jsmsg){
    qDebug()<<"groupChatMsgHandler";
    string time = jsmsg["time"].get<string>();
    int  groupid = jsmsg["groupid"].get<int>();
    string name = jsmsg["name"].get<string>();
    string msg = jsmsg["msg"].get<string>();

    qDebug()<<QString::fromStdString(msg);
    MsgContent msginfo(0,name,msg,time);
    if(m_groupAllMsg.count(groupid)){
        //不是第一次发来消息
        m_groupAllMsg[groupid].push_back(msginfo);

    }else {
        m_groupAllMsg[groupid] = std::vector<MsgContent>(0);
        m_groupAllMsg[groupid].push_back(msginfo);
    }

    //如果当前打开的就是这个id的聊天框
    if(groupid == m_nowGroupIdChat && m_nowShowChat > 0){
        updateGroupChatView(groupid);
    }else{
        //把对应的聊天名称加上提醒标记
        int index = m_groupid2Itemindex[groupid];
        QString tempName = ui->lv_group->item(index)->text();
        if(tempName.right(1) != "*"){
            ui->lv_group->item(index)->setText(tempName+"*");
        }
    }
}

void MainWindow::loginMsgHandler(const nlohmann::json &jsmsg){
    if(jsmsg["errno"].get<int>() == 0){//登录成功
        m_id = jsmsg["id"].get<int>();
        m_name = QString::fromStdString(jsmsg["name"]);
        //存储全局变量
        CurrentUserInfo::GetInstance().setUserId(m_id);
        CurrentUserInfo::GetInstance().setUserName(m_name.toStdString());

        //准备其他数据
        if (jsmsg.contains("friends"))
        {
            m_currentUserFriendList.clear();
            vector<string> vec = jsmsg["friends"];
            for (string &str : vec){
                json jss = json::parse(str);
                User user;
                user.setId(jss["id"].get<int>());
                user.setName(jss["name"]);
                user.setState(jss["state"]);
                auto it = std::find(m_currentUserFriendList.begin(), m_currentUserFriendList.end(), user);
                if (it == m_currentUserFriendList.end()) {
                    m_currentUserFriendList.push_back(user);
                }
            }
        }
        if (jsmsg.contains("groups")){
            m_currentUserGroupList.clear();
            vector<string> vec1 = jsmsg["groups"];
            for (string &groupstr : vec1){
                json grpjs = json::parse(groupstr);
                Group group;
                group.setId(grpjs["id"].get<int>());
                group.setName(grpjs["groupname"]);
                group.setDesc(grpjs["groupdesc"]);

                vector<string> vec2 = grpjs["users"];
                for (string &userstr : vec2){
                    GroupUser user;
                    json jss = json::parse(userstr);
                    user.setId(jss["id"].get<int>());
                    user.setName(jss["name"]);
                    user.setState(jss["state"]);
                    user.setRole(jss["role"]);
                    group.getUsers().push_back(user);
                }
                m_currentUserGroupList.push_back(group);
            }
        }
        loginDisplay();
    }else{
        //登陆失败
        string errMSg = jsmsg["errmsg"];
        QMessageBox::critical(this, tr("登陆失败"),  tr(errMSg.c_str()),QMessageBox::Ok,  QMessageBox::Ok);
        //emit loginIsNO(errMSg);
    }
}

void MainWindow::addOrCreateGroupMsgHandler(const nlohmann::json &jsmsg){
    if (jsmsg.contains("groups")){
        m_currentUserGroupList.clear();
        vector<string> vec1 = jsmsg["groups"];
        for (string &groupstr : vec1){
            json grpjs = json::parse(groupstr);
            Group group;
            group.setId(grpjs["id"].get<int>());
            group.setName(grpjs["groupname"]);
            group.setDesc(grpjs["groupdesc"]);

            vector<string> vec2 = grpjs["users"];
            for (string &userstr : vec2){
                GroupUser user;
                json jss = json::parse(userstr);
                user.setId(jss["id"].get<int>());
                user.setName(jss["name"]);
                user.setState(jss["state"]);
                user.setRole(jss["role"]);
                group.getUsers().push_back(user);
            }
            m_currentUserGroupList.push_back(group);
        }
    }
    updateGrouplist();
}

void MainWindow::addFriendMsgHandler(const nlohmann::json &jsmsg){
    if (jsmsg.contains("friends"))
    {
        m_currentUserFriendList.clear();
        vector<string> vec = jsmsg["friends"];
        for (string &str : vec){
            json jss = json::parse(str);
            User user;
            user.setId(jss["id"].get<int>());
            user.setName(jss["name"]);
            user.setState(jss["state"]);

            auto it = std::find(m_currentUserFriendList.begin(), m_currentUserFriendList.end(), user);
            if (it == m_currentUserFriendList.end()) {
                m_currentUserFriendList.push_back(user);
            }

        }
    }
    updateFriendlist();
}

void MainWindow::on_pb_logout_clicked()
{
    json request;
    request["msgid"] = LOGINOUT_MSG;
    request["id"] = m_id;

    QString qRequestStr = QString::fromStdString(request.dump());
    emit sendMsgToServer(qRequestStr);
    connServerDisplay();
    updateFriendlist();
    updateGrouplist();
}

void MainWindow::addOrCerateItem(int itemNum){
    QString dlgTitleText = "";
    QString dlgLableText = "";
    QString dlgButtonText = "";

    m_addItemGlg = new AddItemDialog();
    if(itemNum == AI_ADD_USER){
        dlgTitleText = "添加好友";
        dlgLableText = "好友ID";
        dlgButtonText = "添加";
        m_addItemGlg->setMsgFlag(ADD_FRIEND_MSG);
    }else if(itemNum == AI_ADD_GROUP){
        dlgTitleText = "添加群聊天";
        dlgLableText = "群ID";
        dlgButtonText = "添加";
        m_addItemGlg->setMsgFlag(ADD_GROUP_MSG);
    }else if(itemNum == AI_CREATE_GROUP){
        dlgTitleText = "创建群聊天";
        dlgLableText = "群聊名称";
        dlgButtonText = "创建";
        m_addItemGlg->setMsgFlag(CREATE_GROUP_MSG);
    }
    qDebug()<<"addOrCerateItem-2";
    //发送消息
    connect(m_addItemGlg,&AddItemDialog::clickAddItem,m_worker,&NetWorkThread::sendMsg);
    //接收消息
    //connect(m_addItemGlg,&AddItemDialog::clickAddItem,m_worker,&NetWorkThread::sendMsg);

    m_addItemGlg->setInfo(dlgTitleText,dlgLableText,dlgButtonText);
    m_addItemGlg->exec();
    delete m_addItemGlg;
    m_addItemGlg = nullptr;
}


void MainWindow::on_pb_addfriend_clicked()
{
    int itemNum = AI_ADD_USER;
    addOrCerateItem(itemNum);
}


void MainWindow::on_pb_addgroup_clicked()
{
    int itemNum = AI_ADD_GROUP;
    addOrCerateItem(itemNum);
}

void MainWindow::on_pb_creategroup_clicked()
{
    int itemNum = AI_CREATE_GROUP;
    addOrCerateItem(itemNum);
}


void MainWindow::on_lv_friend_itemClicked(QListWidgetItem *item)
{

    int clickedIndex = ui->lv_friend->indexFromItem(item).row();
    int friendid = m_index2friendid[clickedIndex];

    ui->label_11->setText(QString::fromStdString(m_currentUserFriendList[clickedIndex].getName()));

    m_nowFriendIdChat = friendid;//记录当前聊天框
    m_nowShowChat = -1;//私聊

    //显示聊天数据
    //去除标记
    QString str = item->text();
    if(str.right(1) == "*"){
        str =  str.left(str.length() - 1);
        item->setText(str);
    }
    updateFriendChatView(friendid);
    ui->tb_allmsg_2->clear();
}


void MainWindow::on_lv_group_itemClicked(QListWidgetItem *item)
{
    int clickedIndex = ui->lv_group->indexFromItem(item).row();
    int groupid = m_index2groupid[clickedIndex];

    //ui->label_11->setText(QString::fromStdString(m_currentUserFriendList[clickedIndex].getName()));

    m_nowGroupIdChat = groupid;//记录当前聊天框
    m_nowShowChat = 1;//群聊

    QString str = item->text();
    if(str.right(1) == "*"){
        str =  str.left(str.length() - 1);
        item->setText(str);
    }
    updateGroupChatView(groupid);

    //更新侧边栏
    ui->tb_allmsg_2->clear();
    Group nowGroup = m_currentUserGroupList[clickedIndex];
    ui->tb_allmsg_2->append(QString("<群id>"));
    ui->tb_allmsg_2->append(QString::number(nowGroup.getId()));
    ui->tb_allmsg_2->append(QString("<群描述>"));
    ui->tb_allmsg_2->append(QString::fromStdString(nowGroup.getDesc()));
    ui->tb_allmsg_2->append(QString("<群成员>"));
    for(auto gu : nowGroup.getUsers()){
        QString itemQstr1 = QString::fromStdString(gu.getName());
        QString itemQstr2 = QString::fromStdString(gu.getRole());
        QString itemQstr3 = QString("-------");
        ui->tb_allmsg_2->append(itemQstr1);
        ui->tb_allmsg_2->append(itemQstr2);
        ui->tb_allmsg_2->append(itemQstr3);
    }
}


void MainWindow::updateFriendChatView(int friendid){
    ui->tb_allmsg->clear();
    QPalette palette = ui->tb_allmsg->palette();
    palette.setColor(QPalette::Text, QColor("black"));
    ui->tb_allmsg->setPalette(palette);
    for(auto oneMsg:m_friendAllMsg[friendid]){
        string someStr = "  >>> @";
        if(CurrentUserInfo::GetInstance().getUserId() == oneMsg.getId()){
            someStr = "  <<< @";
        }
        string timeStr = oneMsg.getTime();
        string nameStr = oneMsg.getName();

        ui->tb_allmsg->append(QString::fromStdString(timeStr+someStr+nameStr));
        string sendMsgStr = oneMsg.getMsg();
        ui->tb_allmsg->append(QString::fromStdString(sendMsgStr));
        ui->tb_allmsg->append(QString::fromStdString(""));
    }
}

void MainWindow::updateGroupChatView(int groupid){
    ui->tb_allmsg->clear();
    QPalette palette = ui->tb_allmsg->palette();
    palette.setColor(QPalette::Text, QColor("black"));
    ui->tb_allmsg->setPalette(palette);
    for(auto oneMsg:m_groupAllMsg[groupid]){
        string someStr = "  >>> @";
        if(CurrentUserInfo::GetInstance().getUserId() == oneMsg.getId()){
            someStr = "  <<< @";
        }
        string timeStr = oneMsg.getTime();
        string nameStr = oneMsg.getName();

        ui->tb_allmsg->append(QString::fromStdString(timeStr+someStr+nameStr));
        string sendMsgStr = oneMsg.getMsg();
        ui->tb_allmsg->append(QString::fromStdString(sendMsgStr));
        ui->tb_allmsg->append(QString::fromStdString(""));
    }
}

//发送消息
void MainWindow::on_pb_sendmsg_clicked()
{
    QString willSendMsg = ui->te_sendmsg->toPlainText();
    qDebug()<<willSendMsg;
    ui->te_sendmsg->clear();

    if(m_nowShowChat ==0){
        return;
    }
    if(m_nowShowChat > 0){//群聊
        json willSendMsgJson;
        willSendMsgJson["msgid"] = GROUP_CHAT_MSG;
        willSendMsgJson["id"] = CurrentUserInfo::GetInstance().getUserId();
        willSendMsgJson["name"] = CurrentUserInfo::GetInstance().getUserName();
        willSendMsgJson["groupid"] = m_nowGroupIdChat;
        willSendMsgJson["msg"] = willSendMsg.toStdString();
        willSendMsgJson["time"] = getCurrentTime();

        MsgContent msgMe(CurrentUserInfo::GetInstance().getUserId(),CurrentUserInfo::GetInstance().getUserName(),willSendMsg.toStdString(),getCurrentTime());
        QString qsendMsgStr = QString::fromStdString(willSendMsgJson.dump());
        emit sendMsgToServer(qsendMsgStr);

        m_groupAllMsg[m_nowGroupIdChat].push_back(msgMe);
        updateGroupChatView(m_nowGroupIdChat);

    }else{//私聊
        json willSendMsgJson;
        willSendMsgJson["msgid"] = ONE_CHAT_MSG;
        willSendMsgJson["id"] = CurrentUserInfo::GetInstance().getUserId();
        willSendMsgJson["name"] = CurrentUserInfo::GetInstance().getUserName();
        willSendMsgJson["toid"] = m_nowFriendIdChat;
        willSendMsgJson["msg"] = willSendMsg.toStdString();
        willSendMsgJson["time"] = getCurrentTime();

        MsgContent msgMe(CurrentUserInfo::GetInstance().getUserId(),CurrentUserInfo::GetInstance().getUserName(),willSendMsg.toStdString(),getCurrentTime());
        QString qsendMsgStr = QString::fromStdString(willSendMsgJson.dump());
        emit sendMsgToServer(qsendMsgStr);

        m_friendAllMsg[m_nowFriendIdChat].push_back(msgMe);
        updateFriendChatView(m_nowFriendIdChat);
    }
}

//清空消息
void MainWindow::on_pb_clearmsg_clicked()
{
    ui->te_sendmsg->clear();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    int result = QMessageBox::information(this,tr("提示"),tr("是否关闭界面!"),tr("是"), tr("否"),0,1);
    if(result == 0)
    {
        //输入关闭的指令
        on_pb_logout_clicked();
        this->close();
    }
    else
    {
        //输入不关闭的指令
        event->ignore();
    }
}



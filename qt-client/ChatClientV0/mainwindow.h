#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include <vector>
#include <string>
#include <unordered_map>
#include <QListWidgetItem>
#include <QCloseEvent>

#include "networkthread.h"
#include "user.h"
#include "groupuser.h"
#include "json.h"
#include "group.h"
#include "msgcontent.h"
#include "additemdialog.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void connServer();
    void sendMsgToServer(QString msg);

    void loginIsOK();
    void loginIsNO(QString errMsg);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

    void initStartUi();
    void initSubNetThread();

    //ui状态
    void disableAll();
    void connServerDisplay();
    void loginDisplay();

    //统一消息接收
    void reciveMsgFromServer(QString msg);
    //消息处理
    void loginMsgHandler(const nlohmann::json &jsmsg);//登录
    void regMsgHandler(const nlohmann::json &jsmsg);//注册
    void logoutMsgHandler(const nlohmann::json &jsmsg);//注销
    void oneChatMsgHandler(const nlohmann::json &jsmsg);//单人聊天
    void groupChatMsgHandler(const nlohmann::json &jsmsg);//群组聊天
    void addFriendMsgHandler(const nlohmann::json &jsmsg);//添加好友回复消息
    void addOrCreateGroupMsgHandler(const nlohmann::json &jsmsg);//添加创建全回复消息


    enum ActionItem{
        AI_ADD_USER = 1,
        AI_CREATE_GROUP,
        AI_ADD_GROUP,
    };
    //统一添加创建好友
    void addOrCerateItem(int itemNum);

    //更新列表
    void updateFriendlist();
    void updateGrouplist();

    //更新聊天框
    void updateFriendChatView(int friendid);
    void updateGroupChatView(int groupid);


private slots:
    //连接相关
    void on_pb_login_clicked();
    void on_pb_connserver_clicked();
    void on_pb_logout_clicked();

    //添加关系
    void on_pb_addfriend_clicked();
    void on_pb_addgroup_clicked();
    void on_pb_creategroup_clicked();

    void on_lv_friend_itemClicked(QListWidgetItem *item);
    void on_pb_sendmsg_clicked();
    void on_pb_clearmsg_clicked();


    void on_lv_group_itemClicked(QListWidgetItem *item);

private:

    Ui::MainWindow *ui;

    AddItemDialog* m_addItemGlg;
    NetWorkThread* m_worker;

    int m_id;
    QString m_name;


    // 数据
    // 记录当前登录用户的好友列表信息
    vector<User> m_currentUserFriendList;
    // 记录当前登录用户的群组列表信息
    vector<Group> m_currentUserGroupList;

    //当前的聊天记录
    std::unordered_map<int,std::vector<MsgContent>> m_friendAllMsg;
    std::unordered_map<int,std::vector<MsgContent>> m_groupAllMsg;

    std::unordered_map<int,int> m_friendid2Itemindex;
    std::unordered_map<int,int> m_groupid2Itemindex;
    std::unordered_map<int,int> m_index2friendid;
    std::unordered_map<int,int> m_index2groupid;

    //标志
    int m_nowFriendIdChat = 0;
    int m_nowGroupIdChat = 0;
    int m_nowShowChat = 0;//是群聊还是私聊 -1私聊 1群聊


    QString m_willSendMsg;

};
#endif // MAINWINDOW_H

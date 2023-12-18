#ifndef MSGCONTENT_H
#define MSGCONTENT_H
#include <chrono>
#include <string>

class MsgContent{

public:
    MsgContent(int id = -1,std::string name = "",std::string msg="",std::string time = 0){
        m_id = id;
        m_name = name;
        m_msg = msg;
        m_time = time;
    }

    void setTime(std::string time){
        m_time = time;
    }
    void setName(std::string name){
        m_name = name;
    }
    void setMsg(std::string msg){
        m_msg = msg;
    }

    void setId(int id){
        m_id = id;
    }

    std::string getTime(){
        return m_time;
    }

    std::string getName(){
        return m_name;
    }

    std::string getMsg(){
        return m_msg;
    }

    int getId(){
        return m_id;
    }
private:
    //时间
    std::string m_time;
    //发送人name
    std::string m_name;
    //消息内容
    std::string m_msg;

    int m_id;
};


#endif // MSGCONTENT_H

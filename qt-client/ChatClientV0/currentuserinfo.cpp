#include "currentuserinfo.h"

int CurrentUserInfo::getUserId(){
    return g_currentUserId;
}

std::string CurrentUserInfo::getUserName(){
    return g_currentUserName;
}

std::string CurrentUserInfo::getUserPwd(){
    return g_currentUserPwd;
}

void CurrentUserInfo::setUserId(int id){
    g_currentUserId = id;
}

void CurrentUserInfo::setUserName(const std::string& name){
    g_currentUserName = name;
}

void CurrentUserInfo::setUserPwd(const std::string& pwd){
    g_currentUserPwd = pwd;
}

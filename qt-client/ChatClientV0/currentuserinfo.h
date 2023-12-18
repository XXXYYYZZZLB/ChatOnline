#ifndef CURRENTUSERINFO_H
#define CURRENTUSERINFO_H
#define DISALLOW_COPY_AND_ASSIGN(TypeName)\
    TypeName(const TypeName&) = delete;\
    TypeName& operator=(const TypeName&) = delete;

#include <string>

class CurrentUserInfo
{
public:
    static CurrentUserInfo& GetInstance() {
        static CurrentUserInfo instance;
        return instance;
    }

    int getUserId();
    std::string getUserName();
    std::string getUserPwd();
    void setUserId(int id);
    void setUserName(const std::string& name);
    void setUserPwd(const std::string& pwd);

private:
    //data
    int g_currentUserId;
    std::string g_currentUserName;
    std::string g_currentUserPwd;

    CurrentUserInfo() = default;
    ~CurrentUserInfo() = default;

    DISALLOW_COPY_AND_ASSIGN(CurrentUserInfo)
};

#endif // CURRENTUSERINFO_H



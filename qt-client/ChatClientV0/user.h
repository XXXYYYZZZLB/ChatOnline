#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

// User表的ORM类
class User {
public:
  User(int id = 0, string name = "", string password = "",
       string state = "offline") {
    this->id = id;
    this->name = name;
    this->password = password;
    this->state = state;
  }

  void setId(int id) { this->id = id; }
  void setName(string name) { this->name = name; }
  void setPwd(string password) { this->password = password; }
  void setState(string state) { this->state = state; }

  int getId() const { return this->id; }
  string getName()const { return this->name; }
  string getPwd() const{ return this->password; }
  string getState() const{ return this->state; }

  bool operator==(const User & user2) const
  {
        if(id == user2.getId() && name ==user2.getName()&&password == user2.getPwd() && state == user2.getState())
            return true;
        else
            return false;
  }

protected:
  int id;
  string name;
  string password;
  string state;
};

#endif // USER_H

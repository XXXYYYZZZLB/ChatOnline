#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H
#include <QString>
#include <QDialog>

namespace Ui {
class addItemDialog;
}

class AddItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddItemDialog(QWidget *parent = nullptr);
    ~AddItemDialog();

    void setInfo(const QString& titleText,const QString& labelText,const QString& pushbuttonText);
    void setMsgFlag(int msgFlag);


signals:
    void clickAddItem(QString msg);
    //点击添加发送数据给服务器
    //等待回传数据

    //槽
    //添加成功 or 失败 更新列表

private slots:
    void on_pb_lable_clicked();

private:
    Ui::addItemDialog *ui;

    int m_msgFlag;
    QString m_titleText;
    QString m_labelText;
    QString m_pushbuttonText;
};

#endif // ADDITEMDIALOG_H

#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QSqlDatabase>

// 🔥 ADD THIS BLOCK (VERY IMPORTANT)
QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_loginButton_clicked();

private:
    Ui::Login *ui;
};

#endif // LOGIN_H

#include "login.h"
#include "ui_login.h"
#include <QSqlQuery>
#include <QMessageBox>
#include "signup.h"

Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    connect(ui->signupBtn, &QPushButton::clicked, this, [this]() {
        signup s;
        if(s.exec() == QDialog::Accepted) {
            ui->usernameEdit->setText(s.getUsername()); // or last entered username (advanced)
        }
    });

    // connect button manually
    connect(ui->loginBtn, &QPushButton::clicked,
            this, &Login::on_loginButton_clicked);
}

Login::~Login()
{
    delete ui;
}
void Login::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if(username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter username and password");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username=? AND password=?");
    query.addBindValue(username);
    query.addBindValue(password);

    if(query.exec() && query.next()) {
        QMessageBox::information(this, "Success", "Login Successful");
        accept();   // ✅ VERY IMPORTANT
    }
    else {
        QMessageBox::warning(this, "Error", "Invalid username or password");
        ui->usernameEdit->clear();
        ui->passwordEdit->clear();
    }
}

#include "signup.h"
#include "ui_signup.h"
#include <QSqlQuery>
#include <QMessageBox>

signup::signup(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::signup)
{
    ui->setupUi(this);
}

signup::~signup()
{
    delete ui;
}

void signup::on_signupBtn_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if(username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter username and password");
        return;
    }

    // 🔥 Duplicate check
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT * FROM users WHERE username = ?");
    checkQuery.addBindValue(username);

    if(!checkQuery.exec()) {
        QMessageBox::warning(this, "Error", "Database error");
        return;
    }

    if(checkQuery.next()) {
        QMessageBox::warning(this, "Error", "Username already exists");
        ui->passwordEdit->clear();
        ui->usernameEdit->selectAll();
        ui->usernameEdit->setFocus();
        return;
    }

    // 🔥 Insert new user
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(password);

    if(query.exec()) {
        QMessageBox::information(this, "Success", "Account Created");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Signup Failed");
    }
}
QString signup::getUsername() const {
    return ui->usernameEdit->text();
}

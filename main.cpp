#include "mainwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include "login.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName(
        "DRIVER={MySQL ODBC 9.6 Unicode Driver};"
        "SERVER=localhost;"
        "DATABASE=student_management;"
        "USER=root;"
        "PASSWORD=abhimysql123@;"
        "PORT=3306;"
        );

    if (!db.open()) {
        QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
        return -1; // stop app
    }

    Login login;

    if(login.exec() == QDialog::Accepted) {
        MainWindow w;
        w.show();
        return a.exec();
    }

    return 0;
}

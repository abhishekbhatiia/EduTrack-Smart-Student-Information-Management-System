#ifndef SIGNUP_H
#define SIGNUP_H

#include <QDialog>

namespace Ui {
class signup;
}

class signup : public QDialog
{
    Q_OBJECT

public:
    explicit signup(QWidget *parent = nullptr);  // ✅ FIXED
    ~signup();                                   // ✅ ADDED
    QString getUsername() const;

private slots:
    void on_signupBtn_clicked();                // ✅ IMPORTANT

private:
    Ui::signup *ui;
};

#endif // SIGNUP_H

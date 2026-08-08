#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHeaderView>
#include <QMouseEvent>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QRegularExpression>
#include "login.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setShowGrid(false);   // cleaner look
    ui->tableWidget->setStyleSheet(
        "QTableWidget::item { padding: 6px; }"
        );
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(30);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    loadStudents();
    updateDashboard();
    ui->tableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setStyleSheet(R"(
    QWidget {
        background-color: #1e1e1e;
        color: #ffffff;
        font-size: 14px;
    }

    QLineEdit, QComboBox {
        background-color: #2b2b2b;
        border: 1px solid #444;
        border-radius: 6px;
        padding: 5px;
        color: white;
    }

    QLineEdit:focus, QComboBox:focus {
        border: 1px solid #00bcd4;
    }

    QPushButton {
        background-color: #2b2b2b;
        border: 1px solid #555;
        border-radius: 8px;
        padding: 6px;
    }

    QPushButton:hover {
        background-color: #3c3c3c;
        border: 1px solid #00bcd4;
    }

QTableWidget {
    background-color: #2b2b2b;
    alternate-background-color: #242424;
    gridline-color: #444;
    border: 1px solid #444;
    border-radius: 8px;
}
QTableWidget::item:hover {
    background-color: #3a3a3a;
    border-radius: 4px;
}

    QHeaderView::section {
        background-color: #333;
        padding: 5px;
        border: 1px solid #444;
    }
QPushButton#addBtn {
    background-color: #1f3b2c;
    border: 1px solid #00c853;
}

QPushButton#addBtn:hover {
    background-color: #00c853;
    color: black;
}

QPushButton#deleteBtn {
    background-color: #3b1f1f;
    border: 1px solid #ff5252;
}

QPushButton#deleteBtn:hover {
    background-color: #ff5252;
    color: black;
}

QPushButton#editBtn {
    background-color: #1f2a3b;
    border: 1px solid #2196f3;
}

QPushButton#editBtn:hover {
    background-color: #2196f3;
    color: black;
}

QPushButton#clearBtn, QPushButton#showAllBtn {
    background-color: #2b2b2b;
    border: 1px solid #777;
}

QPushButton#clearBtn:hover, QPushButton#showAllBtn:hover {
    background-color: #444;
}
QTableWidget::item:selected {
    background-color: #00bcd4;
    color: black;
}

QTableWidget::item:hover {
    background-color: #3a3a3a;
}

QTableWidget {
    selection-background-color: #00bcd4;
    selection-color: black;
}
QHeaderView::section {
    font-weight: bold;
    font-size: 14px;
}
QTableWidget {
    border: 1px solid #444;
    border-radius: 8px;
}
QTableWidget::item:selected {
    background-color: #00e5ff;
    color: black;
}

QTableWidget::item:selected:active {
    background-color: #00bcd4;
}
QLabel#titleLabel {
    font-size: 30px;
    font-weight: bold;
    color: #00e5ff;
    letter-spacing: 3px;
}
QGroupBox {
    border: 1px solid #444;
    border-radius: 10px;
    margin-top: 10px;
    padding: 10px;
}

QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 5px;
    color: #00e5ff;
}
QLineEdit:focus, QComboBox:focus {
    border: 2px solid #00e5ff;
}
QPushButton:pressed {
    background-color: #111;
}
QPushButton#exportBtn {
    background-color: #1f3b3b;
    border: 1px solid #00e5ff;
}
/* CLEAR (Neutral) */
QPushButton#clearBtn {
    background-color: #2b2b2b;
    border: 1px solid #777;
}
QPushButton#clearBtn:hover {
    background-color: #555;
}

QPushButton#showAllBtn {
    background-color: #1f3f3a;
    border: 1px solid #26a69a;
}

QPushButton#showAllBtn:hover {
    background-color: #26a69a;
    color: black;
}

/* LOGOUT (Orange - Warning) */
QPushButton#logoutBtn {
    background-color: #3b2a1f;
    border: 1px solid #ff9800;
}
QPushButton#logoutBtn:hover {
    background-color: #ff9800;
    color: black;
}

/* EXIT (Red - Danger) */
QPushButton#exitBtn {
    background-color: #3b1f1f;
    border: 1px solid #f44336;
}
QPushButton#exitBtn:hover {
    background-color: #f44336;
    color: black;
}

QPushButton#exportBtn:hover {
    background-color: #00e5ff;
    color: black;
}
QLabel {
    line-height: 150%;
}
)");

    // 🔥 COMBOBOX SETUP
    ui->courseCombo->addItem("Select Course");
    ui->courseCombo->addItems({"CSE", "IT", "ECE", "CE"});

    ui->yearCombo->addItem("Select Year");
    ui->yearCombo->addItems({"1ST", "2ND", "3RD", "4TH"});

    // TABLE SETUP
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels(
        {"Roll No","Name",  "Course", "Year",  "Phone","Email"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setSortingEnabled(true);

    // DATABASE CONNECTION

    // 🔥 SEARCH COMBOBOX SETUP

    ui->searchCourseCombo->addItem("All Courses");
    ui->searchCourseCombo->addItems({"CSE", "IT", "ECE", "CE"});

    ui->searchYearCombo->addItem("All Years");
    ui->searchYearCombo->addItems({"1ST", "2ND", "3RD", "4TH"});

    // TABLE SELECTION
    connect(ui->tableWidget, &QTableWidget::itemSelectionChanged, this, [=]() {

        int row = ui->tableWidget->currentRow();

        if(row < 0) {
            ui->nameEdit->clear();
            ui->rollEdit->clear();
            ui->emailEdit->clear();
            ui->phoneEdit->clear();
            ui->courseCombo->setCurrentIndex(0);
            ui->yearCombo->setCurrentIndex(0);
            return;
        }

        ui->rollEdit->setText(ui->tableWidget->item(row,0)->text());
        ui->nameEdit->setText(ui->tableWidget->item(row,1)->text());
        ui->courseCombo->setCurrentText(ui->tableWidget->item(row,2)->text());
        ui->yearCombo->setCurrentText(ui->tableWidget->item(row,3)->text());
        ui->phoneEdit->setText(ui->tableWidget->item(row,4)->text());
        ui->emailEdit->setText(ui->tableWidget->item(row,5)->text());
    });


    // ADD
    connect(ui->addBtn, &QPushButton::clicked, this, [=]() {

        QString name = ui->nameEdit->text().trimmed();
        QString roll = ui->rollEdit->text().trimmed();
        QString course = ui->courseCombo->currentText();
        QString year = ui->yearCombo->currentText();
        QString email = ui->emailEdit->text().trimmed();
        QString phone = ui->phoneEdit->text().trimmed();

        if(name.isEmpty() || roll.isEmpty() || email.isEmpty() || phone.isEmpty()) {
            QMessageBox::warning(this, "Error", "All fields are required!");
            return;
        }

        if(course == "Select Course" || year == "Select Year") {
            QMessageBox::warning(this, "Error", "Select valid course & year!");
            return;
        }

        QRegularExpression regex("^\\d{10}$");

        if(!regex.match(phone).hasMatch()) {
            QMessageBox::warning(this, "Error", "Enter valid 10-digit phone number!");
            return;
        }

        if(!email.contains("@")) {
            QMessageBox::warning(this, "Error", "Invalid Email!");
            return;
        }


        QSqlQuery check;
        check.prepare("SELECT COUNT(*) FROM students WHERE roll_number=? AND course=?");
        check.addBindValue(roll);
        check.addBindValue(course);

        if(check.exec() && check.next() && check.value(0).toInt() > 0) {
            QMessageBox::warning(this, "Error", "Roll number already exists for this course!");
            return;
        }

        QSqlQuery query;
        query.prepare("INSERT INTO students (name, roll_number, course, year, email, phone) VALUES (?, ?, ?, ?, ?, ?)");
        query.addBindValue(name);
        query.addBindValue(roll);
        query.addBindValue(course);
        query.addBindValue(year);
        query.addBindValue(email);
        query.addBindValue(phone);

        if(query.exec()) {
            loadStudents();
            updateDashboard();
        }else {
            if(query.lastError().text().contains("unique_roll_course")) {
                QMessageBox::warning(this, "Error",
                                     "Roll number already exists for this course!");
            } else {
                QMessageBox::warning(this, "Error",
                                     query.lastError().text());
            }
        }

        ui->clearBtn->click();
    });

    // Export
    connect(ui->exportBtn, &QPushButton::clicked, this, [=]() {

        QMessageBox msgBox;
        msgBox.setWindowTitle("Export Option");
        msgBox.setText("Choose export type:");

        QPushButton *allBtn = msgBox.addButton("Export All Data", QMessageBox::AcceptRole);
        QPushButton *filteredBtn = msgBox.addButton("Export Filtered Data", QMessageBox::AcceptRole);
        msgBox.addButton(QMessageBox::Cancel);

        msgBox.exec();

        if(msgBox.clickedButton() == nullptr ||
            msgBox.clickedButton()->text() == "Cancel")
            return;

        bool exportAll = (msgBox.clickedButton() == allBtn);

        QString fileName = QFileDialog::getSaveFileName(this,
                                                        "Save File",
                                                        "Student_Report.csv",
                                                        "CSV Files (*.csv)");

        if(fileName.isEmpty()) return;

        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Error", "Cannot save file");
            return;
        }

        QTextStream out(&file);

        // HEADER
        QStringList headers = { "Roll No", "Name", "Course", "Year", "Phone", "Email"};
        out << headers.join(",") << "\n";

        if(exportAll) {
            // 🔥 EXPORT ALL FROM DATABASE
            QSqlQuery query("SELECT roll_number, name, course, year, phone, email FROM students");

            while(query.next()) {
                for(int col = 0; col < 6; col++) {
                    if(col == 4) {
                        out << "=\"" << query.value(col).toString() << "\"";
                    }else {
                        out << query.value(col).toString();
                    }
                    if(col != 5) out << ",";
                }
                out << "\n";
            }
        }
        else {
            // 🔥 EXPORT FILTERED (TABLE)
            for(int row = 0; row < ui->tableWidget->rowCount(); row++) {
                for(int col = 0; col < ui->tableWidget->columnCount(); col++) {

                    QTableWidgetItem *item = ui->tableWidget->item(row, col);

                    if(col == 4) {
                        if(item) out << "=\"" << item->text() << "\"";
                    }else {
                        if(item) out << item->text();
                    }
                    if(col != ui->tableWidget->columnCount() - 1)
                        out << ",";
                }
                out << "\n";
            }
        }

        file.close();

        QMessageBox::information(this, "Success",
                                 "Data exported successfully!\n(Open in Excel for better view)");
    });    // EDIT BUTTON
    connect(ui->editBtn, &QPushButton::clicked, this, [=]() {

        // ✅ Check selection
        QModelIndexList selectedRows = ui->tableWidget->selectionModel()->selectedRows();

        if(selectedRows.isEmpty()) {
            QMessageBox::warning(this, "Error", "Please select a student first!");
            return;
        }

        int row = selectedRows.first().row();

        // ✅ OLD + NEW roll
        QString oldRoll = ui->tableWidget->item(row, 0)->text();   // original
        QString newRoll = ui->rollEdit->text().trimmed();          // edited
        QString oldCourse = ui->tableWidget->item(row, 2)->text();

        // ✅ Get all fields
        QString name   = ui->nameEdit->text().trimmed();
        QString course = ui->courseCombo->currentText();
        QString year   = ui->yearCombo->currentText();
        QString email  = ui->emailEdit->text().trimmed();
        QString phone  = ui->phoneEdit->text().trimmed();

        // ✅ Validation
        if(name.isEmpty() || newRoll.isEmpty() || email.isEmpty() || phone.isEmpty()) {
            QMessageBox::warning(this, "Error", "All fields are required!");
            return;
        }

        if(course == "Select Course" || year == "Select Year") {
            QMessageBox::warning(this, "Error", "Please select valid course and year!");
            return;
        }

        if(!email.contains("@")) {
            QMessageBox::warning(this, "Error", "Invalid Email!");
            return;
        }
        QRegularExpression regex("^\\d{10}$");

        if(!regex.match(phone).hasMatch()) {
            QMessageBox::warning(this, "Error", "Enter valid 10-digit phone number!");
            return;
        }

        // ✅ Duplicate roll check
        QSqlQuery check;
        check.prepare("SELECT COUNT(*) FROM students WHERE roll_number=? AND course=? AND NOT (roll_number=? AND course=?)");
        check.addBindValue(newRoll);   // edited roll
        check.addBindValue(course);    // edited course
        check.addBindValue(oldRoll);   // original roll
        check.addBindValue(oldCourse);

        if(check.exec() && check.next()) {
            int count = check.value(0).toInt();

            // allow same roll if not changed
            if(count > 0){
                QMessageBox::warning(this, "Error", "Roll number already exists for this course!");
                return;
            }
        }

        // ✅ UPDATE QUERY (IMPORTANT FIX)
        QSqlQuery query;
        query.prepare("UPDATE students SET name=?, roll_number=?, course=?, year=?, email=?, phone=? WHERE roll_number=? AND course=?");

        query.addBindValue(name);
        query.addBindValue(newRoll);   // new roll
        query.addBindValue(course);
        query.addBindValue(year);
        query.addBindValue(email);
        query.addBindValue(phone);
        query.addBindValue(oldRoll);   // where condition
         query.addBindValue(oldCourse);

        if(query.exec()) {
            QMessageBox::information(this, "Success", "Student updated successfully!");
            loadStudents();
            updateDashboard();
            ui->tableWidget->clearSelection();
        } else {
            if(query.lastError().text().contains("unique_roll_course")) {
                QMessageBox::warning(this, "Error",
                                     "Roll number already exists for this course!");
            } else {
                QMessageBox::warning(this, "Error",
                                     query.lastError().text());
            }
        }
    });


    // DELETE
    connect(ui->deleteBtn, &QPushButton::clicked, this, [=]() {

        QModelIndexList selectedRows = ui->tableWidget->selectionModel()->selectedRows();

        if(selectedRows.isEmpty()) {
            QMessageBox::warning(this, "Error", "Please select a student first!");
            return;
        }

        int row = selectedRows.first().row();

        QString roll = ui->tableWidget->item(row, 0)->text();
        QString name = ui->tableWidget->item(row, 1)->text();
        auto reply = QMessageBox::question(this,
                                           "Confirm Delete",
                                           "Delete student:\nName: " + name + "\nRoll: " + roll,
                                           QMessageBox::Yes | QMessageBox::No);

        if(reply == QMessageBox::No) {
            ui->clearBtn->click();   // optional (reuse your clear logic)
            return;
        }

        QSqlQuery query;
        query.prepare("DELETE FROM students WHERE roll_number=?");
        query.addBindValue(roll);

        if(query.exec()) {
            loadStudents();
            updateDashboard();   // 🔥 ADD HERE
        }
    });
    connect(ui->logoutBtn, &QPushButton::clicked,
            this, &MainWindow::on_logoutBtn_clicked);
    connect(ui->exitBtn, &QPushButton::clicked,
            this, &MainWindow::on_exitBtn_clicked);

    // 🔍 SEARCH (FINAL FIXED)
    connect(ui->searchBtn, &QPushButton::clicked, this, [=]() {

        QString name   = ui->nameSearchEdit->text().trimmed();
        QString roll   = ui->rollSearchEdit->text().trimmed();
        QString course = ui->searchCourseCombo->currentText();
        QString year   = ui->searchYearCombo->currentText();

        bool isName   = !name.isEmpty();
        bool isRoll   = !roll.isEmpty();
        bool isCourse = ui->searchCourseCombo->currentIndex() > 0;
        bool isYear   = ui->searchYearCombo->currentIndex() > 0;

        // 🔥 CASE 1: Only name → already handled by live search
        if(isName && !isRoll && !isCourse && !isYear) {
            ui->nameSearchEdit->blockSignals(true);
            ui->nameSearchEdit->clear();
            ui->nameSearchEdit->blockSignals(false);
            return;
        }

        // 🔥 CASE 2: Nothing entered → show popup
        if(!isName && !isRoll && !isCourse && !isYear) {
            QMessageBox::warning(this, "Search Error",
                                 "Please enter at least one field!");
            return;
        }

        // 🔥 QUERY BUILD
        QString queryStr = "SELECT roll_number, name, course, year, phone, email FROM students WHERE 1=1";

        if(isName)
            queryStr += " AND LOWER(name) LIKE LOWER(?)";

        if(isRoll)
            queryStr += " AND roll_number=?";

        if(isCourse)
            queryStr += " AND LOWER(course)=LOWER(?)";

        if(isYear)
            queryStr += " AND LOWER(year)=LOWER(?)";

        QSqlQuery query;
        query.prepare(queryStr);

        if(isName)
            query.addBindValue("%" + name + "%");

        if(isRoll)
            query.addBindValue(roll);

        if(isCourse)
            query.addBindValue(course);

        if(isYear)
            query.addBindValue(year);

        if(!query.exec()) {
            qDebug() << query.lastError().text();
            return;
        }

        // 🔥 LOAD TABLE
        ui->tableWidget->setSortingEnabled(false);
        ui->tableWidget->setRowCount(0);

        int row = 0;
        while(query.next()) {
            ui->tableWidget->insertRow(row);

            for(int col = 0; col < 6; col++) {
                QTableWidgetItem *item;

                if(col == 0) { // Roll No numeric sort
                    item = new QTableWidgetItem();
                    item->setData(Qt::EditRole, query.value(col).toInt());
                } else {
                    item = new QTableWidgetItem(query.value(col).toString());
                }

                ui->tableWidget->setItem(row, col, item);
            }

            row++;
        }

        ui->tableWidget->setSortingEnabled(true);

        if(row == 0)
            QMessageBox::information(this, "Search", "No result found");

        updateStudentCount();

        // 🔥 CLEAR AFTER SEARCH
        // 🔥 CLEAR WITHOUT TRIGGERING FILTER AGAIN

        ui->nameSearchEdit->blockSignals(true);
        ui->rollSearchEdit->blockSignals(true);
        ui->searchCourseCombo->blockSignals(true);
        ui->searchYearCombo->blockSignals(true);

        ui->nameSearchEdit->clear();
        ui->rollSearchEdit->clear();
        ui->searchCourseCombo->setCurrentIndex(0);
        ui->searchYearCombo->setCurrentIndex(0);

        ui->nameSearchEdit->blockSignals(false);
        ui->rollSearchEdit->blockSignals(false);
        ui->searchCourseCombo->blockSignals(false);
        ui->searchYearCombo->blockSignals(false);
    });
    connect(ui->nameSearchEdit, &QLineEdit::textChanged, this, [=]() {
        applyFilters();
    });

    // SHOW ALL
    connect(ui->showAllBtn, &QPushButton::clicked, this, [=]() {
        loadStudents();
        updateDashboard();
    });
    // 🔥 ENTER KEY SEARCH (UPDATED)
    connect(ui->nameSearchEdit, &QLineEdit::returnPressed, this, [=]() {
        ui->searchBtn->click();
    });

    connect(ui->rollSearchEdit, &QLineEdit::returnPressed, this, [=]() {
        ui->searchBtn->click();
    });
    connect(ui->searchYearCombo, &QComboBox::currentIndexChanged, this, [=]() {
        applyFilters();
    });    connect(ui->searchCourseCombo, &QComboBox::currentIndexChanged, this, [=]() {
        applyFilters();
    });

    // CLEAR
    connect(ui->clearBtn, &QPushButton::clicked, this, [=]() {

        // 🔹 Clear student input fields
        ui->nameEdit->clear();
        ui->rollEdit->clear();
        ui->emailEdit->clear();
        ui->phoneEdit->clear();
        ui->courseCombo->setCurrentIndex(0);
        ui->yearCombo->setCurrentIndex(0);

        // 🔹 Clear search fields
        ui->nameSearchEdit->clear();
        ui->rollSearchEdit->clear();
        ui->searchCourseCombo->setCurrentIndex(0);
        ui->searchYearCombo->setCurrentIndex(0);

        // 🔹 Reset table (VERY IMPORTANT)
        loadStudents();
        updateDashboard();

        // 🔹 Clear selection
        ui->tableWidget->clearSelection();
    });
}


// LOAD
void MainWindow::loadStudents() {

    ui->tableWidget->setSortingEnabled(false);

    QSqlQuery query("SELECT roll_number,name, course, year, phone,email FROM students");

    ui->tableWidget->setRowCount(0);

    int row = 0;
    while(query.next()) {

        ui->tableWidget->insertRow(row);

        for(int col = 0; col < 6; col++) {

            QTableWidgetItem *item;

            if(col == 0) { // Roll No → numeric sorting
                item = new QTableWidgetItem();
                item->setData(Qt::EditRole, query.value(col).toInt());
            } else {
                item = new QTableWidgetItem(query.value(col).toString());
            }
            if(col == 0 || col == 2 || col == 3) {
                item->setTextAlignment(Qt::AlignCenter);
            } else {
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            }

            ui->tableWidget->setItem(row, col, item);
        }

        row++;
    }

    ui->tableWidget->setSortingEnabled(true);
    updateStudentCount();
}void MainWindow::updateDashboard()
{
    QString courseStats = "";
    QString yearStats = "";

    // ✅ COURSE QUERY
    QSqlQuery courseQuery;
    if(!courseQuery.exec("SELECT course, COUNT(*) FROM students WHERE course !='EE' GROUP BY course")) {
        qDebug() << "Course Query Error:" << courseQuery.lastError().text();
    }

    while(courseQuery.next()) {
        QString course = courseQuery.value(0).toString();
        int count = courseQuery.value(1).toInt();

        qDebug() << "COURSE:" << course << count;   // 🔥 DEBUG

        courseStats += "• " + course + "  →  " + QString::number(count) + "\n";
    }

    // ✅ YEAR QUERY (SEPARATE OBJECT)
    QSqlQuery yearQuery;
    if(!yearQuery.exec(
            "SELECT year, COUNT(*) FROM students "
            "GROUP BY year "
            "ORDER BY CASE "
            "WHEN year = '1ST' THEN 1 "
            "WHEN year = '2ND' THEN 2 "
            "WHEN year = '3RD' THEN 3 "
            "WHEN year = '4TH' THEN 4 "
            "END"
            )) {
        qDebug() << "Year Query Error:" << yearQuery.lastError().text();
    }

    while(yearQuery.next()) {
        QString year = yearQuery.value(0).toString();
        int count = yearQuery.value(1).toInt();

        qDebug() << "YEAR:" << year << count;   // 🔥 DEBUG

        yearStats += "• " + year + "  →  " + QString::number(count) + "\n";
    }

    // ✅ SET UI
    ui->courseLabel->setText(courseStats);
    ui->yearLabel->setText(yearStats);
}

void MainWindow::updateStudentCount()
{
    ui->totalLabel->setText("Total Students: " +
                            QString::number(ui->tableWidget->rowCount()));
}
void MainWindow::applyFilters()
{
    QString name   = ui->nameSearchEdit->text().trimmed();
    QString roll   = ui->rollSearchEdit->text().trimmed();
    QString course = ui->searchCourseCombo->currentText();
    QString year   = ui->searchYearCombo->currentText();

    bool isName   = !name.isEmpty();
    bool isRoll   = !roll.isEmpty();
    bool isCourse = ui->searchCourseCombo->currentIndex() > 0;
    bool isYear   = ui->searchYearCombo->currentIndex() > 0;

    QString queryStr = "SELECT  roll_number, name, course, year, phone, email FROM students WHERE 1=1";

    if(isName)
        queryStr += " AND LOWER(name) LIKE LOWER(?)";

    if(isRoll)
        queryStr += " AND roll_number=?";

    if(isCourse)
        queryStr += " AND LOWER(course)=LOWER(?)";

    if(isYear)
        queryStr += " AND LOWER(year)=LOWER(?)";

    QSqlQuery query;
    query.prepare(queryStr);

    if(isName)
        query.addBindValue("%" + name + "%");

    if(isRoll)
        query.addBindValue(roll);

    if(isCourse)
        query.addBindValue(course);

    if(isYear)
        query.addBindValue(year);

    if(!query.exec()) {
        qDebug() << query.lastError().text();
        return;
    }

    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->setRowCount(0);

    int row = 0;
    while(query.next()) {
        ui->tableWidget->insertRow(row);

        for(int col = 0; col < 6; col++) {
            QTableWidgetItem *item;

            if(col == 0) { // Roll No numeric sort
                item = new QTableWidgetItem();
                item->setData(Qt::EditRole, query.value(col).toInt());
            } else {
                item = new QTableWidgetItem(query.value(col).toString());
            }

            ui->tableWidget->setItem(row, col, item);
        }

        row++;
    }
    ui->tableWidget->setSortingEnabled(true);

    updateStudentCount();
}


void MainWindow::on_logoutBtn_clicked()
{
    auto reply = QMessageBox::question(this,
                                       "Logout",
                                       "Are you sure you want to logout?",
                                       QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes)
    {
        this->hide();   // hide main window

        Login *login = new Login();
        login->show();  // open login screen
    }
    else
    {
        // ❌ user clicked NO → do nothing
        return;
    }
}
void MainWindow::on_exitBtn_clicked()
{
    auto reply = QMessageBox::question(this,
                                       "Exit",
                                       "Are you sure you want to exit?",
                                       QMessageBox::Yes | QMessageBox::No);

    if(reply == QMessageBox::Yes)
        QApplication::quit();
}
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!ui->tableWidget->underMouse()) {
        ui->tableWidget->clearSelection();
    }
    QMainWindow::mousePressEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QIcon>
#include <vector>
#include <memory>

class Contact {
public:
    virtual ~Contact() = default;
    virtual QString getName() const = 0;
    virtual QString getInfo() const = 0;
    virtual QString getType() const = 0;
    virtual QString toText() const = 0;
};

class PersonContact : public Contact {
public:
    PersonContact(QString n = "", QString p = "", QString e = "");
    QString getName() const override;
    QString getInfo() const override;
    QString getType() const override;
    QString toText() const override;
    void setData(QString n, QString p, QString e);
private:
    QString name, phone, email;
};

class CompanyContact : public Contact {
public:
    CompanyContact(QString n = "", QString p = "", QString a = "");
    QString getName() const override;
    QString getInfo() const override;
    QString getType() const override;
    QString toText() const override;
    void setData(QString n, QString p, QString a);
private:
    QString name, phone, address;
};

class GroupContact : public Contact {
public:
    GroupContact(QString n = "", QString d = "", int c = 0);
    QString getName() const override;
    QString getInfo() const override;
    QString getType() const override;
    QString toText() const override;
    void setData(QString n, QString d, int c);
private:
    QString name, desc;
    int count;
};

class EmergencyContact : public Contact {
public:
    EmergencyContact(QString n = "", QString p = "", QString r = "");
    QString getName() const override;
    QString getInfo() const override;
    QString getType() const override;
    QString toText() const override;
    void setData(QString n, QString p, QString r);
private:
    QString name, phone, relation;
};

// Фабрика
class ContactFactory {
public:
    static std::unique_ptr<Contact> create(QString type);
    static std::unique_ptr<Contact> fromText(QString text);
};

QT_BEGIN_NAMESPACE
class QListWidget;
class QLineEdit;
class QComboBox;
class QTextEdit;
class QPushButton;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    void addContact();
    void deleteContact();
    void saveFile();
    void loadFile();
    void showContact(int row);
    void filter();

private:
    void updateList();

    QListWidget* list;
    QLineEdit* searchBox;
    QComboBox* typeBox;
    QTextEdit* details;
    QPushButton* addBtn;
    QPushButton* delBtn;
    QPushButton* saveBtn;
    QPushButton* loadBtn;

    std::vector<std::unique_ptr<Contact>> contacts;
    std::vector<Contact*> filtered;
};

#endif

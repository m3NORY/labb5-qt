#include "mainwindow.h"
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QInputDialog>


PersonContact::PersonContact(QString n, QString p, QString e)
    : name(n), phone(p), email(e) {}

QString PersonContact::getName() const { return name; }
QString PersonContact::getInfo() const { return phone + "\n" + email; }
QString PersonContact::getType() const { return "Person"; }
QString PersonContact::toText() const { return "P:" + name + ";" + phone + ";" + email; }
void PersonContact::setData(QString n, QString p, QString e) { name = n; phone = p; email = e; }

// Реализация методов CompanyContact
CompanyContact::CompanyContact(QString n, QString p, QString a)
    : name(n), phone(p), address(a) {}

QString CompanyContact::getName() const { return name; }
QString CompanyContact::getInfo() const { return phone + "\n" + address; }
QString CompanyContact::getType() const { return "Company"; }
QString CompanyContact::toText() const { return "C:" + name + ";" + phone + ";" + address; }
void CompanyContact::setData(QString n, QString p, QString a) { name = n; phone = p; address = a; }

// Реализация методов GroupContact
GroupContact::GroupContact(QString n, QString d, int c)
    : name(n), desc(d), count(c) {}

QString GroupContact::getName() const { return name; }
QString GroupContact::getInfo() const { return desc + "\n" + QString::number(count) + " участников"; }
QString GroupContact::getType() const { return "Group"; }
QString GroupContact::toText() const { return "G:" + name + ";" + desc + ";" + QString::number(count); }
void GroupContact::setData(QString n, QString d, int c) { name = n; desc = d; count = c; }

// Реализация методов EmergencyContact
EmergencyContact::EmergencyContact(QString n, QString p, QString r)
    : name(n), phone(p), relation(r) {}

QString EmergencyContact::getName() const { return name; }
QString EmergencyContact::getInfo() const { return phone + "\n" + relation; }
QString EmergencyContact::getType() const { return "Emergency"; }
QString EmergencyContact::toText() const { return "E:" + name + ";" + phone + ";" + relation; }
void EmergencyContact::setData(QString n, QString p, QString r) { name = n; phone = p; relation = r; }

// Реализация фабрики
std::unique_ptr<Contact> ContactFactory::create(QString type) {
    if (type == "Person") return std::make_unique<PersonContact>();
    if (type == "Company") return std::make_unique<CompanyContact>();
    if (type == "Group") return std::make_unique<GroupContact>();
    if (type == "Emergency") return std::make_unique<EmergencyContact>();
    return nullptr;
}

std::unique_ptr<Contact> ContactFactory::fromText(QString text) {
    if (text.startsWith("P:")) {
        auto parts = text.mid(2).split(";");
        if (parts.size() >= 3)
            return std::make_unique<PersonContact>(parts[0], parts[1], parts[2]);
    }
    if (text.startsWith("C:")) {
        auto parts = text.mid(2).split(";");
        if (parts.size() >= 3)
            return std::make_unique<CompanyContact>(parts[0], parts[1], parts[2]);
    }
    if (text.startsWith("G:")) {
        auto parts = text.mid(2).split(";");
        if (parts.size() >= 3)
            return std::make_unique<GroupContact>(parts[0], parts[1], parts[2].toInt());
    }
    if (text.startsWith("E:")) {
        auto parts = text.mid(2).split(";");
        if (parts.size() >= 3)
            return std::make_unique<EmergencyContact>(parts[0], parts[1], parts[2]);
    }
    return nullptr;
}

// Реализация MainWindow
MainWindow::MainWindow() {
    // Создаем главный виджет
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    QWidget* left = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(left);
    left->setMaximumWidth(300);

    // Поиск и фильтр
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("Поиск...");
    typeBox = new QComboBox();
    typeBox->addItems({"Все", "Person", "Company", "Group", "Emergency"});
    searchLayout->addWidget(searchBox);
    searchLayout->addWidget(typeBox);
    leftLayout->addLayout(searchLayout);

    list = new QListWidget();
    leftLayout->addWidget(list);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    addBtn = new QPushButton("+ Добавить");
    delBtn = new QPushButton("- Удалить");
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);
    leftLayout->addLayout(btnLayout);

    QHBoxLayout* fileLayout = new QHBoxLayout();
    saveBtn = new QPushButton("Сохранить");
    loadBtn = new QPushButton("Загрузить");
    fileLayout->addWidget(saveBtn);
    fileLayout->addWidget(loadBtn);
    leftLayout->addLayout(fileLayout);

    QWidget* right = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(right);

    QLabel* titleLabel = new QLabel("Детали контакта:");
    rightLayout->addWidget(titleLabel);

    details = new QTextEdit();
    details->setReadOnly(true);
    rightLayout->addWidget(details);

    // Собираем все вместе
    mainLayout->addWidget(left);
    mainLayout->addWidget(right);

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addContact);
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::deleteContact);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveFile);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadFile);
    connect(list, &QListWidget::currentRowChanged, this, &MainWindow::showContact);
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::filter);
    connect(typeBox, &QComboBox::currentTextChanged, this, &MainWindow::filter);

    // Добавляем тестовые данные
    contacts.push_back(std::make_unique<PersonContact>("Иван Петров", "123-45-67", "ivan@mail.ru"));
    contacts.push_back(std::make_unique<CompanyContact>("ООО что-то", "495-111-22-33", "Москва"));
    contacts.push_back(std::make_unique<GroupContact>("Друзья", "Школьные друзья", 5));
    contacts.push_back(std::make_unique<EmergencyContact>("Скорая помощь", "103", "Медицина"));

    updateList();

    setWindowTitle("Редактор контактов");
    resize(700, 500);
}

void MainWindow::addContact() {
    QString type = typeBox->currentText();
    if (type == "Все") type = "Person";

    auto contact = ContactFactory::create(type);
    if (!contact) return;

    QStringList labels;
    QStringList values;

    if (type == "Person") {
        labels = {"Имя:", "Телефон:", "Email:"};
    } else if (type == "Company") {
        labels = {"Название:", "Телефон:", "Адрес:"};
    } else if (type == "Group") {
        labels = {"Название:", "Описание:", "Кол-во участников:"};
    } else {
        labels = {"Имя:", "Телефон:", "Отношение:"};
    }

    for (const QString& label : labels) {
        bool ok;
        QString value = QInputDialog::getText(this, "Новый контакт", label, QLineEdit::Normal, "", &ok);
        if (!ok) return;
        values << value;
    }

    if (type == "Person") {
        dynamic_cast<PersonContact*>(contact.get())->setData(values[0], values[1], values[2]);
    } else if (type == "Company") {
        dynamic_cast<CompanyContact*>(contact.get())->setData(values[0], values[1], values[2]);
    } else if (type == "Group") {
        dynamic_cast<GroupContact*>(contact.get())->setData(values[0], values[1], values[2].toInt());
    } else {
        dynamic_cast<EmergencyContact*>(contact.get())->setData(values[0], values[1], values[2]);
    }

    contacts.push_back(std::move(contact));
    updateList();
}

void MainWindow::deleteContact() {
    int row = list->currentRow();
    if (row < 0) return;

    if (!filtered.empty()) {
        Contact* c = filtered[row];
        auto it = std::find_if(contacts.begin(), contacts.end(),
                               [c](auto& ptr) { return ptr.get() == c; });
        if (it != contacts.end()) contacts.erase(it);
        filtered.clear();
        filter();
    } else {
        contacts.erase(contacts.begin() + row);
    }

    updateList();
    details->clear();
}

void MainWindow::saveFile() {
    QString file = QFileDialog::getSaveFileName(this, "Сохранить контакты", "", "Text files (*.txt)");
    if (file.isEmpty()) return;

    QFile f(file);
    if (!f.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
        return;
    }

    QTextStream out(&f);
    for (auto& c : contacts) {
        out << c->toText() << "\n";
    }
    f.close();

    QMessageBox::information(this, "Готово", "Контакты сохранены!");
}

void MainWindow::loadFile() {
    QString file = QFileDialog::getOpenFileName(this, "Загрузить контакты", "", "Text files (*.txt)");
    if (file.isEmpty()) return;

    QFile f(file);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить файл");
        return;
    }

    contacts.clear();

    QTextStream in(&f);
    while (!in.atEnd()) {
        auto c = ContactFactory::fromText(in.readLine());
        if (c) contacts.push_back(std::move(c));
    }
    f.close();

    filtered.clear();
    updateList();
    details->clear();
    QMessageBox::information(this, "Готово", "Контакты загружены!");
}

void MainWindow::showContact(int row) {
    if (row < 0) {
        details->clear();
        return;
    }

    Contact* c = filtered.empty() ? contacts[row].get() : filtered[row];

    QString text = "=== " + c->getName() + " ===\n\n";
    text += "Тип: " + c->getType() + "\n\n";
    text += c->getInfo();

    details->setText(text);
}

void MainWindow::filter() {
    QString search = searchBox->text().toLower();
    QString type = typeBox->currentText();

    filtered.clear();

    for (auto& c : contacts) {
        bool matchSearch = search.isEmpty() || c->getName().toLower().contains(search);
        bool matchType = type == "Все" || c->getType() == type;

        if (matchSearch && matchType) {
            filtered.push_back(c.get());
        }
    }

    updateList();
}

void MainWindow::updateList() {
    list->clear();

    if (filtered.empty()) {
        for (auto& c : contacts) {
            list->addItem(c->getName());
        }
    } else {
        for (Contact* c : filtered) {
            list->addItem(c->getName());
        }
    }
}

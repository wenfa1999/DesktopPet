#include "appconfig.h"

#if _MSC_VER >= 1600 // VS2010版本号是1600
#pragma execution_character_set("utf-8")
#endif

AppConfig::AppConfig(QWidget *parent) : QWidget{parent} {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    InterfaceInit();
}

void AppConfig::InterfaceInit() {
    QString qss;
    QFile qssFile(":/qss/setting");
    qssFile.open(QFile::ReadOnly);
    if (qssFile.isOpen()) {
        qss = QLatin1String(qssFile.readAll());
        this->setStyleSheet(qss);
    }

    // 窗口大小
    this->setMinimumSize(400, 225);
    this->resize(800, 450);
    this->setProperty("class", "settingWidget");

    listWidget = new QListWidget(this);
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setMaximumWidth(360);

    QListWidgetItem *item1 = new QListWidgetItem(listWidget);
    QListWidgetItem *item2 = new QListWidgetItem(listWidget);
    QListWidgetItem *item3 = new QListWidgetItem(listWidget);

    item1->setText("通用");
    item2->setText("外观");
    item3->setText("关于");

    mainLayout = new QHBoxLayout();
    mainLayout->setDirection(QHBoxLayout::LeftToRight);
    mainLayout->addStretch();
    mainLayout->addWidget(listWidget, 1);

    QWidget *showWidget = new QWidget(this);
    showWidget->setProperty("class", "showWIdget");
    showWidget->setMaximumWidth(1500);

    QLabel *label1 = new QLabel(showWidget);
    label1->setText("sgggf");
    mainLayout->addWidget(showWidget, 3);

    mainLayout->addStretch();
    mainLayout->addStretch();

    this->setLayout(mainLayout);
}

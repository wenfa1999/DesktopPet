#include "appconfig.h"
#include <QDebug>

#if _MSC_VER >= 1600 // VS2010版本号是1600
#pragma execution_character_set("utf-8")
#endif

AppConfig::AppConfig(QWidget *parent)
    : QWidget{parent}, generalSetWidget(nullptr) {

    this->setWindowTitle("设置");
    this->setWindowIcon(QIcon(":/chicken"));
    this->setWindowIconText("余人人");
    this->setObjectName("AppConfig");

    InterfaceInit();

    connect(listWidget, &QListWidget::currentRowChanged, [=](int currentRow) {
        qDebug() << "Select: " << currentRow;
        switch (currentRow) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        default:
            qDebug() << "Something wrong!";
            break;
        }
    });
}

AppConfig::~AppConfig() {
    //    delete settingsWidget;
    //    clearLayout(generalSetLayout);
}

/**
 * @brief 界面初始化
 */
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

    // 左侧菜单
    listWidget = new QListWidget(this);
    listWidget->setProperty("class", "listWidget");
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setMaximumWidth(360);

    QListWidgetItem *item1 = new QListWidgetItem(listWidget);
    QListWidgetItem *item2 = new QListWidgetItem(listWidget);
    QListWidgetItem *item3 = new QListWidgetItem(listWidget);

    item1->setText("通用");
    item2->setText("外观");
    item3->setText("关于");
    listWidget->setCurrentRow(0);

    // 右侧详细设置

    // 主界面布局
    mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(60);
    mainLayout->addStretch();
    mainLayout->addStretch();
    mainLayout->addStretch();
    mainLayout->addWidget(listWidget, 1);
    mainLayout->addStretch();
    mainLayout->addStretch();
    mainLayout->addStretch();
    mainLayout->addStretch();
    this->setLayout(mainLayout);

    ShowGeneralSettings();

}

/**
 * @brief 通用设置界面
 */
void AppConfig::ShowGeneralSettings() {
    if (generalSetWidget == nullptr) {
        generalSetWidget = new QWidget(this);
        generalSetWidget->setProperty("class", "generalSetWidget");
        generalSetWidget->setMaximumWidth(1250);
    }

    mainLayout->insertWidget(5, generalSetWidget, 3);

    /// 通用设置界面
    // 语言
    QWidget *lanWidget = new QWidget(generalSetWidget);
    lanWidget->setProperty("class", "lanWidget");

    QLabel *lanLabel = new QLabel("语言", lanWidget);
    QComboBox *lanBox = new QComboBox(lanWidget);
    lanBox->addItem("简体中文");
    lanBox->addItem("繁體中文");
    lanBox->addItem("English");

    lanLayout = new QHBoxLayout(lanWidget);
    lanLayout->addWidget(lanLabel);
    lanLayout->addStretch();
    lanLayout->addWidget(lanBox);

    // 语言结束
    // 开机启动
    QWidget *bootWidget = new QWidget(generalSetWidget);
    bootWidget->setProperty("class", "lanWidget");

    QLabel *bootLabel = new QLabel("开机启动", bootWidget);
    QRadioButton *btnTrue = new QRadioButton("是", bootWidget);
    QRadioButton *btnFalse = new QRadioButton("否", bootWidget);

    QHBoxLayout *bootLayout = new QHBoxLayout(bootWidget);
    bootLayout->addWidget(bootLabel);
    bootLayout->addStretch();
    bootLayout->addWidget(btnTrue);
    bootLayout->addWidget(btnFalse);

    generalSetLayout = new QVBoxLayout(generalSetWidget);
//    generalSetLayout->setMargin(30);
    generalSetLayout->addWidget(lanWidget);
    generalSetLayout->addWidget(bootWidget);
    generalSetLayout->addStretch();
}

/*static void clearLayout(QLayout *layout) {
    if (nullptr == layout) {
        return;
    }

    QLayoutItem *child = layout->takeAt(0);
    while (child) {
        QLayout *subLayout = child->layout();
        if (nullptr == subLayout) {
            QWidget *widget = child->widget();
            if (nullptr != widget) {
                widget->setParent(nullptr);
                delete widget;
            }
        } else {
            clearLayout(subLayout);
        }
        delete child;
        child = layout->takeAt(0);
    }
}*/

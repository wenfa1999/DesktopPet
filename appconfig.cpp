#include "appconfig.h"
#include <QDebug>
#include <QHeaderView>

#if _MSC_VER >= 1600 // VS2010版本号是1600
#pragma execution_character_set("utf-8")
#endif

AppConfig::AppConfig(QWidget *parent)
    : QWidget{parent}
    , generalSetWidget(nullptr)
    , shortcutSetWidget(nullptr)
{
    this->setWindowFlags(Qt::FramelessWindowHint);  // 无标题栏
    this->setWindowTitle("设置");
    this->setWindowIcon(QIcon(":/chicken"));
    this->setWindowIconText("余人人");
    this->setObjectName("AppConfig");

    InterfaceInit();

    connect(listWidget, &QListWidget::currentRowChanged, this, [&](int currentRow) {
        qDebug() << "Select: " << currentRow;
        switch (currentRow) {
        case 0:
            generalSetWidget->show();
            shortcutSetWidget->hide();
            break;
        case 1:
            generalSetWidget->hide();
            shortcutSetWidget->show();
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
    this->resize(800, 450 + 30);
    this->setProperty("class", "settingWidget");

    // 标题栏
    QWidget *titleWidget = new QWidget(this);
    titleWidget->setFixedHeight(30);
    titleWidget->setProperty("class", "titleWidget");
    titleWidget->setLayout(new QHBoxLayout());
    QPushButton *btnMinimize = new QPushButton(titleWidget);
    btnMinimize->setObjectName("btnMinimize");
    QPushButton *btnMaximize = new QPushButton(titleWidget);
    btnMaximize->setObjectName("btnMaximize");
    QPushButton *btnClose = new QPushButton(titleWidget);
    btnClose->setObjectName("btnClose");
    dynamic_cast<QHBoxLayout *>(titleWidget->layout())->addStretch();
    titleWidget->layout()->addWidget(btnMinimize);
    titleWidget->layout()->addWidget(btnMaximize);
    titleWidget->layout()->addWidget(btnClose);
    titleWidget->layout()->setContentsMargins(0,0,0,0);

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
    item2->setText("快捷键设置");
    item3->setText("关于");
    listWidget->setCurrentRow(0);

    // 主界面布局
    mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(60, 30, 60, 60);
    mainLayout->addStretch();
    mainLayout->addStretch();
    mainLayout->addStretch();
    mainLayout->addWidget(listWidget, 1);
    mainLayout->addStretch();
    mainLayout->addStretch();
    mainLayout->addStretch();
    mainLayout->addStretch();

    QVBoxLayout *widgetLayout = new QVBoxLayout(this);
    widgetLayout->addWidget(titleWidget);
    widgetLayout->addLayout(mainLayout);
    widgetLayout->setMargin(10);
    widgetLayout->setSpacing(0);

    this->setLayout(widgetLayout);
//    this->setLayout(mainLayout);

    // 通用设置Widget初始化
    GeneralSettingsInit();
    // 快捷键设置Widget初始化
    ShortcutSettingsInit();
}

/**
 * @brief 通用设置界面
 */
void AppConfig::GeneralSettingsInit() {
    if (nullptr == generalSetWidget) {
        generalSetWidget = new QWidget(this);
        generalSetWidget->setProperty("class", "generalSetWidget");
        generalSetWidget->setMaximumWidth(1250);
        generalSetWidget->hide();
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

/**
 * @brief 快捷键设置
 */
void AppConfig::ShortcutSettingsInit()
{
    if (nullptr == shortcutSetWidget) {
        shortcutSetWidget = new QWidget(this);
        shortcutSetWidget->setProperty("class", "shortcutSetWidget");
        shortcutSetWidget->setMaximumWidth(1250);
        shortcutSetWidget->hide();
    }
    mainLayout->insertWidget(5, shortcutSetWidget, 3);

    // 滚动区域
    QScrollArea *shortcutScrollArea = new QScrollArea(shortcutSetWidget);
//    shortcutScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
//    shortcutScrollArea->setWidgetResizable(true);  // 设置滚动区域自适应

    // 滚动区域的Frame container
    QFrame *container = new QFrame();
    container->setLayout(new QVBoxLayout());
//    container->setMinimumWidth(1200);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /// 功能区
    for (size_t j = 0; j < 12; ++j)
    {
        QGroupBox *FnBox = new QGroupBox(shortcutScrollArea);
        container->layout()->addWidget(FnBox);
        FnBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        FnBox->setTitle("F" + QString::number(j+1));
        QGridLayout *FnLayout = new QGridLayout(FnBox);
        // 设置FnBox的最小大小为其父控件的大小
        FnBox->setMinimumSize(shortcutScrollArea->viewport()->size());
        // 设置FnBox布局的列数为7
        FnLayout->setColumnStretch(7, 1);
        for(auto i = 'A'; i <= 'Z'; ++i)
        {
            QPushButton *btn = new QPushButton(FnBox);
            connect(btn, &QPushButton::clicked, this, [=]()
                    {
                        QString filePath = QFileDialog::getOpenFileName(btn, "选择文件", "", "All Files (*.*)");
                        if (!filePath.isEmpty()) {
                            qDebug() << filePath;
                            MyDatabase *database = MyDatabase::instance();
                            database->updateValue(QString("F" + QString::number(j+1)).toStdString().c_str(), QString(i), filePath);
                        }
                    });
            btn->setText(QString(i));
//            btn->setFixedSize(100, 50); // 设置按钮的固定大小
            btn->setFont(QFont("Arial", 12)); // 设置按钮的字体大小
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            FnLayout->addWidget(btn, (i-2) / 7, (i-2) % 7);
            FnLayout->setHorizontalSpacing(10); // 设置按钮之间的水平间距
            FnLayout->setVerticalSpacing(10); // 设置按钮之间的垂直间距
            FnLayout->setColumnStretch(0, 1);
            FnLayout->setColumnStretch(1, 1);
            FnLayout->setColumnStretch(2, 1);
            FnLayout->setColumnStretch(3, 1);
            FnLayout->setColumnStretch(4, 1);
            FnLayout->setColumnStretch(5, 1);
            FnLayout->setColumnStretch(6, 1);
        }
    }

    // Test
    QPushButton *btn = new QPushButton(shortcutScrollArea);
//    container->layout()->addWidget(btn);

    shortcutScrollArea->setWidget(container);


    // 快捷键配置Layout
    shortcutLayout = new QVBoxLayout(shortcutSetWidget);
    shortcutLayout->addWidget(shortcutScrollArea);
    shortcutLayout->addWidget(btn);

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

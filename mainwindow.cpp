#include "mainwindow.h"
#include <QDebug>
#include <mmsystem.h>
#include <QDialog>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScreen>
#include <QGuiApplication>
#include <QContextMenuEvent>
#include <QMessageBox>
#pragma comment(lib, "winmm.lib")


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    ,m_windowOpacity(200)
    ,settingInterface(nullptr)
    ,m_hourlyTimer(nullptr)
    ,m_hourlyChimeEnabled(false) // 默认不透明度
    ,m_mousePassThrough(false) // 默认不启用鼠标穿透
{
    // 初始化设置对象
    m_settings = new QSettings("DesktopPet", "Settings", this);
    
    // 加载设置
    loadSettings();
    
    QString qss;
    QFile qssFile(":/qss/qss");
    qssFile.open(QFile::ReadOnly);
    if (qssFile.isOpen()) {
        qss = QLatin1String(qssFile.readAll());
        this->setStyleSheet(qss);
    }

    // 设置窗口标志 - 重要：先创建窗口再设置透明属性
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::NoDropShadowWindowHint);
    
    // 由于透明度设置问题，改用Qt自带的透明度支持
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowOpacity(m_windowOpacity / 255.0); // 使用Qt的窗口透明度设置
    
    this->setAttribute(Qt::WA_QuitOnClose);

    // 初始化数据库
//    MyDatabase *dataBase = MyDatabase::instance();

    actionInit();
    trayIconInit();

    m_systemTrayIcon->setIcon(QIcon("://catIcon"));
    m_systemTrayIcon->show();

    // 调试显示窗口位置和大小
    qDebug() << "窗口初始化 - 位置:" << this->pos() << "大小:" << this->size();
    
    // 创建前景标签用于显示图像
    QLabel *label = new QLabel(this);
    label->setObjectName("imageLabel"); // 添加对象名以便查找
    label->resize(QSize(200, 200));
    label->move(20, 20); // 确保在窗口内可见
    
    // 加载默认GIF
    m_movie = new QMovie(this);
    m_movie->setFileName(":/imgs/basketball.gif");
    label->setMovie(m_movie);
    
    if (m_movie->isValid()) {
        qDebug() << "GIF有效: " << m_movie->fileName();
        m_movie->start();
    } else {
        qDebug() << "GIF无效: " << m_movie->fileName() << "错误: " << m_movie->lastErrorString();
        // 尝试加载备用图像
        label->setText("图像加载失败");
        label->setStyleSheet("color: red;");
    }
    
    this->setFixedSize(QSize(240, 324));

    // 注册热键
    registerWinHotKey();
    
    // 设置整点报时
    setupHourlyChime();

    // 加载上次使用的图像
    if (!m_lastImagePath.isEmpty() && QFile::exists(m_lastImagePath)) {
        loadCustomImage(m_lastImagePath);
    }
    
    // 确保窗口居中显示
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);
    qDebug() << "窗口已居中 - 位置:" << this->pos();

    connect(m_systemTrayIcon, &QSystemTrayIcon::activated, this,
            [=](QSystemTrayIcon::ActivationReason reson) {
        switch (reson) {
        case QSystemTrayIcon::DoubleClick:
            if (this->isVisible()) {
                this->setVisible(false);
                m_showAction->setEnabled(true);
                m_hideAction->setEnabled(false);
            } else {
                this->setVisible(true);
                m_showAction->setEnabled(false);
                m_hideAction->setEnabled(true);
            }
            break;
        default:
            break;
        }
    });
    
    // 添加1秒后检查窗口状态的调试信息
    QTimer::singleShot(1000, this, [this](){
        qDebug() << "窗口状态检查 - 可见:" << this->isVisible() 
                 << "位置:" << this->pos() 
                 << "透明度:" << m_windowOpacity
                 << "Qt透明度:" << this->windowOpacity()
                 << "活跃:" << this->isActiveWindow();
    });

    // 应用鼠标穿透设置
    if (m_mousePassThrough) {
        setMousePassThrough(true);
    }
}

MainWindow::~MainWindow() {
    // 保存设置
    saveSettings();
    
    // 释放影片资源
    if (m_movie) {
        m_movie->stop();
        delete m_movie;
        m_movie = nullptr;
    }
    
    // 释放设置界面资源
    if (settingInterface) {
        delete settingInterface;
        settingInterface = nullptr;
    }
    
    // 释放整点报时定时器
    if (m_hourlyTimer) {
        m_hourlyTimer->stop();
        delete m_hourlyTimer;
        m_hourlyTimer = nullptr;
    }
    
    // 系统托盘图标在父对象析构时会自动释放
}

/**
 * @brief 各Action初始化
 */
void MainWindow::actionInit() {
    m_showAction = new QAction("显示", this);
    m_showAction->setEnabled(false);
    connect(m_showAction, &QAction::triggered, this, [=] {
        if (!this->isVisible()) {
            this->show();
            // 确保显示后设置透明度
            QTimer::singleShot(50, this, [=]() {
                setWindowsTransparency(m_windowOpacity);
            });
        }
        m_showAction->setEnabled(false);
        m_hideAction->setEnabled(true);
    });
    m_hideAction = new QAction("隐藏", this);
    connect(m_hideAction, &QAction::triggered, this, [=] {
        if (this->isVisible()) {
            this->hide();
        }
        m_showAction->setEnabled(true);
        m_hideAction->setEnabled(false);
    });

    m_setAction = new QAction("设置", this);
    connect(m_setAction, &QAction::triggered, this, [=] {
        if (nullptr == settingInterface)
        {
            settingInterface = new AppConfig();
        }
        settingInterface->show();   // 展示
        settingInterface->raise();  // 置顶
    });
    
    // 添加选择图像动作
    m_imageAction = new QAction("选择图像", this);
    connect(m_imageAction, &QAction::triggered, this, [=] {
        QString fileName = QFileDialog::getOpenFileName(this, "选择图像", 
            "", "图像文件 (*.png *.jpg *.bmp *.gif)");
        if (!fileName.isEmpty()) {
            loadCustomImage(fileName);
        }
    });

    // 添加整点报时菜单项
    m_hourlyChimeAction = new QAction("整点报时", this);
    m_hourlyChimeAction->setCheckable(true);
    m_hourlyChimeAction->setChecked(m_hourlyChimeEnabled);
    connect(m_hourlyChimeAction, &QAction::toggled, this, [=](bool checked) {
        m_hourlyChimeEnabled = checked;
        if (checked) {
            if (!m_hourlyTimer->isActive()) {
                m_hourlyTimer->start(1000); // 每秒检查一次
            }
            QMessageBox::information(this, "整点报时", "整点报时功能已启用");
        } else {
            if (m_hourlyTimer->isActive()) {
                m_hourlyTimer->stop();
            }
        }
    });

    m_quitAction = new QAction("退出", this);
    connect(m_quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    // 添加鼠标穿透菜单项
    m_mousePassThroughAction = new QAction("鼠标穿透", this);
    m_mousePassThroughAction->setCheckable(true);
    m_mousePassThroughAction->setChecked(m_mousePassThrough);
    connect(m_mousePassThroughAction, &QAction::toggled, this, [=](bool checked) {
        setMousePassThrough(checked);
        // QMessageBox::information(this, "鼠标穿透", 
        //     checked ? "鼠标穿透已启用，鼠标点击将透过桌宠" : "鼠标穿透已禁用");
    });
}

/**
 * @brief 创建上下文菜单(用于右键点击和托盘图标)
 * @return 返回菜单指针
 */
QMenu* MainWindow::createContextMenu()
{
    QMenu *menu = new QMenu(this);
    menu->setProperty("class", "iconMenu");
    
    menu->addAction(m_showAction);
    menu->addAction(m_hideAction);
    menu->addSeparator();
    menu->addAction(m_imageAction);
    menu->addAction(m_hourlyChimeAction);
    
    // 添加透明度设置菜单项
    QAction *opacityAction = new QAction("设置透明度", this);
    connect(opacityAction, &QAction::triggered, this, [=]() {
        // 创建临时对话框
        QDialog dialog(this);
        dialog.setWindowTitle("设置透明度");
        
        // 创建滑块
        QSlider *slider = new QSlider(Qt::Horizontal, &dialog);
        slider->setRange(50, 255); // 50-255范围的不透明度
        slider->setValue(m_windowOpacity);
        slider->setTickInterval(20);
        slider->setTickPosition(QSlider::TicksBelow);
        
        // 创建标签
        QLabel *valueLabel = new QLabel(QString::number(m_windowOpacity), &dialog);
        
        // 滑块值变化时直接更新标签和透明度
        connect(slider, &QSlider::valueChanged, [=](int value) {
            valueLabel->setText(QString::number(value));
            setWindowsTransparency(value);
        });
        
        // 创建按钮
        QPushButton *okButton = new QPushButton("确定", &dialog);
        connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
        
        // 布局
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        QHBoxLayout *sliderLayout = new QHBoxLayout();
        sliderLayout->addWidget(slider);
        sliderLayout->addWidget(valueLabel);
        layout->addLayout(sliderLayout);
        layout->addWidget(okButton);
        
        // 显示对话框
        dialog.exec();
    });
    menu->addAction(opacityAction);
    
    // 添加鼠标穿透菜单项
    menu->addAction(m_mousePassThroughAction);
    
    menu->addAction(m_setAction);
    menu->addSeparator();
    menu->addAction(m_quitAction);
    
    return menu;
}

/**
 * @brief 系统托盘初始化
 */
void MainWindow::trayIconInit() {
    // 创建并设置托盘图标菜单
    m_trayIconMenu = createContextMenu();
    
    m_systemTrayIcon = new QSystemTrayIcon(this);
    m_systemTrayIcon->setContextMenu(m_trayIconMenu);
    m_systemTrayIcon->setToolTip("桌宠");
}

/**
 * @brief 重写mousePressEvent函数，实现左键拖动功能
 * @param event
 */
void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isMouseMoving = true;
        m_lastPos = event->globalPosition().toPoint();
    }
    QMainWindow::mousePressEvent(event);
}

/**
 * @brief 重写mouseReleaseEvent函数，实现左键拖动功能
 * @param event
 */
void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if ((Qt::LeftButton == event->button()) && (true == isMouseMoving)) {
        isMouseMoving = false;
    }
    QMainWindow::mouseReleaseEvent(event);
}

/**
 * @brief M重写mouseMoveEvent函数，实现左键拖动功能
 * @param event
 */
void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (true == isMouseMoving) {
            this->move(this->pos() + (event->globalPosition().toPoint() - m_lastPos));
            m_lastPos = event->globalPosition().toPoint();
        }
        QMainWindow::mouseMoveEvent(event);
}

/**
 * @brief 重写mouseDoubleClickEvent函数，实现双击隐藏
 * @param event
 */
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    if (Qt::LeftButton == event->button()) {
        this->hide();
        m_showAction->setEnabled(true);
        m_hideAction->setEnabled(false);
    }
    QMainWindow::mouseDoubleClickEvent(event);
}

/**
 * @brief 加载自定义图像
 * @param path 图像文件路径
 */
void MainWindow::loadCustomImage(const QString &path)
{
    QLabel *label = findChild<QLabel*>("imageLabel");
    if (!label) {
        qDebug() << "找不到标签控件";
        return;
    }
    
    // 停止并清理之前的动画
    if (m_movie) {
        m_movie->stop();
        delete m_movie;
        m_movie = nullptr;
    }
    
    // 根据扩展名判断图像类型
    QFileInfo fileInfo(path);
    QString suffix = fileInfo.suffix().toLower();
    
    if (suffix == "gif") {
        m_movie = new QMovie(this);
        m_movie->setFileName(path);
        label->setMovie(m_movie);
        
        if (m_movie->isValid()) {
            qDebug() << "自定义GIF有效: " << path;
            m_movie->start();
        } else {
            qDebug() << "自定义GIF无效: " << path << "错误: " << m_movie->lastErrorString();
            label->setText("图像加载失败");
            label->setStyleSheet("color: red;");
            return;
        }
    } else {
        QPixmap pixmap(path);
        if (!pixmap.isNull()) {
            qDebug() << "自定义图像有效: " << path << "尺寸: " << pixmap.size();
            label->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            qDebug() << "自定义图像无效: " << path;
            label->setText("图像加载失败");
            label->setStyleSheet("color: red;");
            return;
        }
    }
    
    m_currentImagePath = path;
    m_lastImagePath = path; // 保存路径以便下次启动时加载
    qDebug() << "成功加载图像: " << path;
}

/**
 * @brief 设置窗口透明度
 * @param opacity 不透明度 (0-255)
 */
void MainWindow::setWindowsTransparency(int opacity)
{
    // 参数有效性检查
    if (opacity < 50) opacity = 50;  // 防止完全透明
    if (opacity > 255) opacity = 255;
    
    m_windowOpacity = opacity; // 保存当前不透明度设置
    
    // 使用Qt的方式设置窗口透明度 (转换为0.0-1.0范围)
    double qtOpacity = m_windowOpacity / 255.0;
    this->setWindowOpacity(qtOpacity);
    
    qDebug() << "设置窗口透明度:" << m_windowOpacity << "Qt透明度:" << qtOpacity;
}

/**
 * @brief 窗口显示事件
 */
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    
    // 窗口显示时重新应用透明度
    QTimer::singleShot(50, this, [this]() {
        this->setWindowOpacity(m_windowOpacity / 255.0);
    });
}

/**
 * @brief 窗口状态改变事件
 */
void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    
    // 窗口状态改变时重新应用透明度
    if (event->type() == QEvent::WindowStateChange && isVisible()) {
        QTimer::singleShot(50, this, [this]() {
            setWindowsTransparency(m_windowOpacity);
        });
    }
}

/**
 * @brief 注册Windows全局热键
 */
void MainWindow::registerWinHotKey()
{
    // 注册Ctrl+Alt+P为显示/隐藏热键
    bool success = RegisterHotKey((HWND)this->winId(), 1, MOD_CONTROL | MOD_ALT, 'P');
    if (success) {
        qDebug() << "全局热键注册成功: Ctrl+Alt+P";
    } else {
        qDebug() << "全局热键注册失败: " << GetLastError();
    }
}

/**
 * @brief 处理Windows原生消息
 */
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY && msg->wParam == 1) {
        // 热键被按下，切换显示/隐藏状态
        if (this->isVisible()) {
            this->hide();
            m_showAction->setEnabled(true);
            m_hideAction->setEnabled(false);
        } else {
            this->show();
            m_showAction->setEnabled(false);
            m_hideAction->setEnabled(true);
        }
        return true;
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

/**
 * @brief 设置整点报时功能
 */
void MainWindow::setupHourlyChime()
{
    // 创建并设置定时器
    m_hourlyTimer = new QTimer(this);
    connect(m_hourlyTimer, &QTimer::timeout, this, &MainWindow::checkHourlyChime);
    
    // 根据需要可以在这里加载用户设置，确定是否启用整点报时
    // 例如：m_hourlyChimeEnabled = loadUserSettings("hourlyChime", false);
    
    // 如果启用，则启动定时器
    if (m_hourlyChimeEnabled && !m_hourlyTimer->isActive()) {
        m_hourlyTimer->start(1000); // 每秒检查一次
    }
}

/**
 * @brief 检查是否需要整点报时
 */
void MainWindow::checkHourlyChime()
{
    // 获取当前时间
    QTime currentTime = QTime::currentTime();
    
    // 检查是否整点
    if (currentTime.second() == 0 && currentTime.minute() == 0) {
        // 整点报时
        int hour = currentTime.hour();
        QString hourText = QString::number(hour) + "点整";
        
        // 显示系统通知
        m_systemTrayIcon->showMessage(
            "整点报时", 
            "现在是" + hourText, 
            QSystemTrayIcon::Information, 
            3000
        );
        
        // 可以在这里添加播放声音的代码
        // 例如使用QSound::play(":/sounds/hourly.wav");
        // 或者使用Windows API播放声音
        PlaySound(TEXT("SystemAsterisk"), NULL, SND_ASYNC);
        
        qDebug() << "整点报时: " << hourText;
    }
}

/**
 * @brief 设置鼠标穿透状态
 * @param enable 是否启用鼠标穿透
 */
void MainWindow::setMousePassThrough(bool enable)
{
    m_mousePassThrough = enable;
    
    // 使用Qt的方式设置鼠标穿透
    setAttribute(Qt::WA_TransparentForMouseEvents, enable);
    
    // 同时使用Windows API设置鼠标穿透(更彻底)
    HWND hwnd = (HWND)this->winId();
    if (hwnd) {
        LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        if (enable) {
            // 添加WS_EX_TRANSPARENT样式
            SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
        } else {
            // 移除WS_EX_TRANSPARENT样式
            SetWindowLong(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
        }
    }
    
    qDebug() << "鼠标穿透状态设置为:" << enable;
}

/**
 * @brief 保存应用设置
 */
void MainWindow::saveSettings()
{
    m_settings->setValue("hourlyChime", m_hourlyChimeEnabled);
    m_settings->setValue("lastImagePath", m_lastImagePath);
    m_settings->setValue("windowOpacity", m_windowOpacity);
    m_settings->setValue("windowGeometry", saveGeometry());
    m_settings->setValue("mousePassThrough", m_mousePassThrough); // 保存鼠标穿透状态
    m_settings->sync();
    qDebug() << "设置已保存";
}

/**
 * @brief 加载应用设置
 */
void MainWindow::loadSettings()
{
    m_hourlyChimeEnabled = m_settings->value("hourlyChime", false).toBool();
    m_lastImagePath = m_settings->value("lastImagePath", "").toString();
    m_windowOpacity = m_settings->value("windowOpacity", 200).toInt();
    m_mousePassThrough = m_settings->value("mousePassThrough", false).toBool(); // 加载鼠标穿透状态
    
    // 恢复窗口位置
    if (m_settings->contains("windowGeometry")) {
        restoreGeometry(m_settings->value("windowGeometry").toByteArray());
    }
    
    qDebug() << "设置已加载: 整点报时=" << m_hourlyChimeEnabled 
             << " 上次图像=" << m_lastImagePath 
             << " 不透明度=" << m_windowOpacity
             << " 鼠标穿透=" << m_mousePassThrough;
}

/**
 * @brief 处理右键菜单事件
 */
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    // 创建临时菜单(与托盘图标菜单内容相同)
    QMenu *contextMenu = createContextMenu();
    
    // 在鼠标位置显示菜单
    contextMenu->exec(event->globalPos());
    
    // 清理菜单资源
    delete contextMenu;
    
    // 事件已处理
    event->accept();
}

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>

#if _MSC_VER >= 1600 // VS2010版本号是1600
#pragma execution_character_set("utf-8")
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QString qss;
    QFile qssFile(":/qss/qss");
    qssFile.open(QFile::ReadOnly);
    if (qssFile.isOpen()) {
        qss = QLatin1String(qssFile.readAll());
        this->setStyleSheet(qss);
    }

    this->setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint |
                         Qt::WindowStaysOnTopHint | Qt::NoDropShadowWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    //    this->resize(960, 540);

    actionInit();
    trayIconInit();

    m_systemTrayIcon->setIcon(QIcon("://catIcon"));
    m_systemTrayIcon->show();

    QLabel *label = new QLabel(this);
    //    QPixmap pix(":/keqing");
    //    pix.scaled(200, 200, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //    label->setPixmap(
    //                pix.scaled(200, 200, Qt::IgnoreAspectRatio,
    //                Qt::SmoothTransformation));
    label->resize(QSize(200, 200));

    m_movie = new QMovie(this);
    m_movie->setFileName(":/img/basketball");
    label->setMovie(m_movie);
    m_movie->start();
    this->setFixedSize(QSize(240, 324));

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
}

MainWindow::~MainWindow() { delete ui; }

/**
 * @brief 各Action初始化
 */
void MainWindow::actionInit() {
    m_showAction = new QAction("显示", this);
    m_showAction->setEnabled(false);
    connect(m_showAction, &QAction::triggered, this, [=] {
        if (!this->isVisible()) {
            this->show();
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
    m_quitAction = new QAction("退出", this);
    connect(m_quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

/**
 * @brief 系统托盘初始化
 */
void MainWindow::trayIconInit() {
    m_trayIconMenu = new QMenu(this);
    m_trayIconMenu->setProperty("class", "iconMenu");

    m_trayIconMenu->addAction(m_showAction);
    m_trayIconMenu->addAction(m_hideAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

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
        m_lastPos = event->globalPos();
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
        this->move(this->pos() + (event->globalPos() - m_lastPos));
        m_lastPos = event->globalPos();
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

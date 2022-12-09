#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>

#if _MSC_VER >= 1600 // VS2010版本号是1600
#pragma execution_character_set("utf-8")
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    this->setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint |
                         Qt::WindowStaysOnTopHint | Qt::NoDropShadowWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->resize(400, 300);

    actionInit();
    trayIconInit();

    m_systemTrayIcon->setIcon(QIcon("://catIcon"));
    m_systemTrayIcon->show();

    QLabel *label = new QLabel(this);
    QPixmap pix("://chicken");
    label->setPixmap(pix);
    qDebug() << pix.size().width() << pix.size().width();
    label->resize(pix.size());
    this->setFixedSize(pix.size());

}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::actionInit() {
    m_quitAction = new QAction("退出", this);
    connect(m_quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWindow::trayIconInit() {
    m_trayIconMenu = new QMenu(this);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

    m_systemTrayIcon = new QSystemTrayIcon(this);
    m_systemTrayIcon->setContextMenu(m_trayIconMenu);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isMouseMoving = true;
        m_lastPos = event->globalPos();
        qDebug() << event->globalPos();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if ((Qt::LeftButton == event->button()) && (true == isMouseMoving))
    {
        isMouseMoving = false;
        qDebug() << "Release..";
    }
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (true == isMouseMoving)
    {
        this->move(this->pos() + (event->globalPos() - m_lastPos));
        m_lastPos = event->globalPos();
        qDebug() << "Moving...";
    }
    QMainWindow::mouseMoveEvent(event);
}

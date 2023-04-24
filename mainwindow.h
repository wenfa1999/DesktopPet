#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QPoint>
#include <QMovie>
#include <QThread>

#include "appconfig.h"
#include <sqlite3.h>
#include "mydatabase.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

private:
    void actionInit(void);
    void trayIconInit(void);

private:

    QSystemTrayIcon *m_systemTrayIcon;
    QAction *m_showAction;
    QAction *m_hideAction;
    QAction *m_quitAction;
    QAction *m_setAction;
    QMenu *m_trayIconMenu;

    QMovie *m_movie;

    AppConfig *settingInterface;

    QPoint m_lastPos;
    bool isMouseMoving;

    sqlite3 *db;
    char * errMsg = nullptr;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};
#endif // MAINWINDOW_H

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
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <windows.h>
#include <Winuser.h>
#include <QTimer>
#include <QTime>
#include <QDateTime>
#include <QSettings>

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
    void checkHourlyChime();

private:
    void actionInit(void);
    void trayIconInit(void);
    void loadCustomImage(const QString &path);
    void setWindowsTransparency(int opacity = 230);
    void registerWinHotKey();
    void setupHourlyChime();
    void saveSettings();
    void loadSettings();

private:
    QSystemTrayIcon *m_systemTrayIcon;
    QAction *m_showAction;
    QAction *m_hideAction;
    QAction *m_quitAction;
    QAction *m_setAction;
    QAction *m_imageAction;
    QMenu *m_trayIconMenu;

    QMovie *m_movie;
    QString m_currentImagePath;
    QString m_lastImagePath;
    int m_windowOpacity;
    QSettings *m_settings;

    AppConfig *settingInterface;

    QPoint m_lastPos;
    bool isMouseMoving;

    sqlite3 *db;
    char * errMsg = nullptr;

    QTimer *m_hourlyTimer;
    bool m_hourlyChimeEnabled;
    QAction *m_hourlyChimeAction;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;
};
#endif // MAINWINDOW_H

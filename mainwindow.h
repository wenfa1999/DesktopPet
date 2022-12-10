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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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
    Ui::MainWindow *ui;

    QSystemTrayIcon *m_systemTrayIcon;
    QAction *m_showAction;
    QAction *m_hideAction;
    QAction *m_quitAction;
    QMenu *m_trayIconMenu;


    QPoint m_lastPos;
    bool isMouseMoving;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};
#endif // MAINWINDOW_H

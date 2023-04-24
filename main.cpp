#include "mainwindow.h"

#include <QApplication>
#include <Windows.h>
#include <QDesktopServices>
#include <QUrl>
#include "mydatabase.h"

QMap<QString, QString> hotkeys = {
    {"ctrl+alt+t", "C:\\Program Files\\qBittorrent\\qbittorrent.exe"},  // 常规可执行文件路径
    {"ctrl+alt+s", "C:\\Users\\GWF\\OneDrive - MSFT\\图片\\user.png"},    // 图片路径
    {"ctrl+alt+n", "calc.exe"},  // 环境变量
};

// 定义回调函数
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false); // 设置不退出事件循环

    // 钩子
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if(hHook == nullptr)
    {
        qDebug() << "钩子安装失败！";
    }
    else
    {
        qDebug() << "钩子安装成功！";
    }

    MainWindow w;
    w.show();
    return a.exec();
}


LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
        if (wParam == WM_KEYDOWN)
        {
            // 获取按下的快捷键
            QString primaryKey, foreignKey;
            if (GetAsyncKeyState(VK_F1) & 0x8000) primaryKey  = "F1";
            if (GetAsyncKeyState(VK_F2) & 0x8000) primaryKey  = "F2";
            if (GetAsyncKeyState(VK_F3) & 0x8000) primaryKey  = "F3";
            if (GetAsyncKeyState(VK_F4) & 0x8000) primaryKey  = "F4";
            if (GetAsyncKeyState(VK_F5) & 0x8000) primaryKey  = "F5";
            if (GetAsyncKeyState(VK_F6) & 0x8000) primaryKey  = "F6";
            if (GetAsyncKeyState(VK_F7) & 0x8000) primaryKey  = "F7";
            if (GetAsyncKeyState(VK_F8) & 0x8000) primaryKey  = "F8";
            if (GetAsyncKeyState(VK_F9) & 0x8000) primaryKey  = "F9";
            if (GetAsyncKeyState(VK_F10) & 0x8000) primaryKey  = "F10";
            if (GetAsyncKeyState(VK_F11) & 0x8000) primaryKey  = "F11";
            if (GetAsyncKeyState(VK_F12) & 0x8000) primaryKey  = "F12+";
            if (primaryKey.isEmpty())
            {
                return CallNextHookEx(NULL, nCode, wParam, lParam);
            }
            if ((p->vkCode >= 'A' && p->vkCode <= 'Z') || (p->vkCode >= '0' && p->vkCode <= '9'))
            {
                foreignKey  = toupper(p->vkCode);
            }
            qDebug() << "press:" << primaryKey << "+" << foreignKey ;

            QString data;
            MyDatabase::instance()->findValue(primaryKey, foreignKey, data);
            qDebug() << "data: " << data;

            // 如果快捷键存在，则打开对应的软件
            if (!data.isEmpty())
            {
                QString cmd = "\"" + data + "\"";
                qDebug() << cmd;
                // system(cmd.toUtf8().constData());   // 有黑框
                // WinExec(cmd.toUtf8().constData(), SW_SHOW);   // 可用，注意路径不要转义引号以使用空格路径，不支持非可执行文件
                //                QDesktopServices::openUrl(QUrl::fromLocalFile(data));    // 正常使用，包括非可执行文件
                //                ShellExecute(nullptr, "open", data.toStdString().c_str(), NULL, NULL, SW_SHOWNORMAL);  // 正常使用，包括非可执行文件
                QDesktopServices::openUrl(QUrl::fromLocalFile(data));
                    // QProcess::startDetached(cmd, QStringList());    // 可用，注意路径不要转义引号以使用空格路径，不支持非可执行文件
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

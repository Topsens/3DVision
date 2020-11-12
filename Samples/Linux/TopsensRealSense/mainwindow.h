#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "depthview.h"
#include "panel.h"
#include <TopsensPeople.hpp>
#include <mutex>
#include <thread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void showEvent(QShowEvent *);

signals:
    void UpdateStatus();

private slots:
    void OnStart();
    void OnStop();
    void OnStatus();

private:
    void Resize(uint32_t, uint32_t);
    void Status(const QString&);

    static QString GetError(Topsens::Error);

private:
    Ui::MainWindow *ui;

    QString status;
    std::mutex lock;

    DepthView* dview;
    Panel* panel;

    bool stop;
    std::thread thread;
};

#endif // MAINWINDOW_H

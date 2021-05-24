#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <TopsensSensor.hpp>
#include <cmath>
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

signals:
    void UpdateColor();
    void UpdateDepth();

protected:
    void showEvent(QShowEvent*) override;
    void closeEvent(QCloseEvent*) override;

private slots:
    void OnUpdateColor();
    void OnUpdateDepth();
    void OnRefresh();
    void OnStart();
    void OnStop();
    void OnUsersNo(bool checked);
    void OnGroundYes(bool checked);

private:
    void PaintColor(const Topsens::ColorFrame& frame);
    void PaintDepth(const Topsens::DepthFrame& frame);
    void PaintUsers(const Topsens::UsersFrame& frame);
    void PaintGround(const Topsens::DepthFrame& frame, const Topsens::Vector4& groundPlain);
    void Arrange(Topsens::Orientation orientation);

    static inline bool IsGround(const Topsens::Vector3& point, const Topsens::Vector4& groundPlane, float thresh)
    {
        return std::fabs(point.X * groundPlane.X + point.Y * groundPlane.Y + point.Z * groundPlane.Z + groundPlane.W) < thresh;
    }
    static QString GetError(Topsens::Error error);

private:
    Ui::MainWindow *ui;

    Topsens::Sensor sensor;
    Topsens::Orientation orientation;

    bool stop;
    std::thread thread;
    std::mutex  cmutex;
    std::mutex  dmutex;

    QPixmap cpixmap;
    QPixmap dpixmap;

    std::vector<int> cpixels;
    std::vector<int> dpixels;

    static std::vector<int> palette;
};

#endif // MAINWINDOW_H

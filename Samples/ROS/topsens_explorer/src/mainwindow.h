#pragma once

#include "userpainter.h"
#include <QMainWindow>
#include <QTimer>
#include <mutex>
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <topsens/UsersFrame.h>

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
    void OnTimer();

private:
    void OnDInfo(const sensor_msgs::CameraInfo::ConstPtr&);
    void OnColor(const sensor_msgs::Image::ConstPtr&);
    void OnDepth(const sensor_msgs::Image::ConstPtr&);
    void OnUsers(const topsens::UsersFrame::ConstPtr&);

    void Arrange();
    void ToPointCloud(const sensor_msgs::Image::ConstPtr&);

private:
    static inline bool IsGround(const topsens::Vector3& point, const topsens::Vector4& ground)
    {
        return std::fabs(point.X * ground.X + point.Y * ground.Y + point.Z * ground.Z + ground.W) < 0.035f;
    }
private:
    Ui::MainWindow *ui;

    QTimer timer;

    UserPainter upainter;

    int32_t orient;

    uint64_t dtime;
    uint64_t utime;

    topsens::Vector4 ground;

    std::mutex clock;
    std::mutex dlock;

    QPixmap cpixmap;
    QPixmap dpixmap;

    std::vector<uint32_t> cpixels;
    std::vector<uint32_t> dpixels;
    std::vector<uint32_t> palette;

    float fx, fy, cx, cy;
    std::vector<topsens::Vector3> cloud;
    
    ros::NodeHandle node;
    ros::Subscriber color;
    ros::Subscriber depth;
    ros::Subscriber users;
    ros::Subscriber dinfo;

    ros::AsyncSpinner spinner;
};
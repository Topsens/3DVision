#pragma once

#include <QMainWindow>
#include <QTimer>
#include <mutex>
#include <ros/ros.h>
#include <topsens/UsersFrame.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>

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
    void showEvent(QShowEvent*) override;
    void closeEvent(QCloseEvent*) override;

signals:
    void Arrange(int width, int height, int orientation);
    void UpdateDepth();
    void UpdateUsers();

private slots:
    void OnTimer();
    void OnArrange(int width, int height, int orientation);
    void OnUpdateDepth();

private:
    void OnUsers(const topsens::UsersFrame::ConstPtr&);
    void OnDepth(const sensor_msgs::Image::ConstPtr&);

private:
    Ui::MainWindow *ui;

    QTimer timer;

    int32_t  orient;

    QPixmap pixmap;
    std::mutex dlock;
    std::vector<uint32_t> dpixels;
    std::vector<uint32_t> palette;

    ros::NodeHandle node;
    ros::Subscriber depth;
    ros::Subscriber users;
    ros::Subscriber camera;

    ros::AsyncSpinner spinner;
};
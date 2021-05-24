#pragma once

#include <QLabel>
#include <mutex>
#include <vector>
#include <topsens/UsersFrame.h>

class ULabel : public QLabel
{
    Q_OBJECT

public:
    explicit ULabel(QWidget*);

    void Draw(const topsens::UsersFrame::ConstPtr&, int32_t orientation);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    void PaintUser(const std::vector<QPointF>&, QPainter&);
    static void PaintBone(const QPointF&, const QPointF&, QPainter&);
    static void PaintJoint(const QPointF&, QPainter&);
    static QPointF Map(const topsens::Vector3& joint, const topsens::Vector4& projection);

private:
    std::mutex lock;
    std::vector<std::vector<QPointF>> users;
};
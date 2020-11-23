#ifndef DEPTHVIEW_H
#define DEPTHVIEW_H

#include <QWidget>
#include <Topsens.hpp>
#include <mutex>
#include <vector>

namespace Ui {
class DepthView;
}

class DepthView : public QWidget
{
    Q_OBJECT

public:
    explicit DepthView(QWidget *parent = 0);
    ~DepthView();

    void Draw(const uint16_t* depth, uint32_t width, uint32_t height, Topsens::Orientation orientation);
    void Draw(const Topsens::UsersFrame&);

signals:
    void Update();

protected:
    void resizeEvent(QResizeEvent*) override;

private slots:
    void OnUpdate();

private:
    Ui::DepthView *ui;

    std::mutex lock;
    std::vector<uint32_t> pixels;

    QPixmap pixmap;
    Topsens::Orientation orientation;

    static std::vector<uint32_t> palette;
};

#endif // DEPTHVIEW_H

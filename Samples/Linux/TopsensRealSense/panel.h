#ifndef PANEL_H
#define PANEL_H

#include <QWidget>
#include <mutex>
#include <Topsens.hpp>
#include <librealsense2/rs.hpp>

namespace Ui {
class Panel;
}

class Panel : public QWidget
{
    Q_OBJECT

public:
    explicit Panel(QWidget *parent = 0);
    ~Panel();

    uint32_t Width() const;
    uint32_t Height() const;
    bool Flip() const;
    std::string Serial() const;
    Topsens::Orientation Orientation() const;

    void Enable();
    void Disable();

signals:
    void Refresh();

private slots:
    void OnRefresh();
    void OnDevice();

protected:
    void resizeEvent(QResizeEvent*) override;

private:
    static int GetWidth(const QString&);
    static int GetHeight(const QString&);

public:
    Ui::Panel *ui;

    std::mutex lock;
    std::string serial;

    rs2::context context;
};

#endif // PANEL_H

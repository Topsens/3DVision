#ifndef PANEL_H
#define PANEL_H

#include <QWidget>
#include <TopsensSensor.hpp>

namespace Ui {
class Panel;
}

class Panel : public QWidget
{
    Q_OBJECT

public:
    explicit Panel(QWidget *parent = 0);
    ~Panel();

    void Count(uint32_t count);

    bool Align() const;
    bool Flip() const;
    bool Record() const;
    bool GenUsers() const;
    bool PaintGround() const;
    Topsens::Resolution ColorRes() const;
    Topsens::Resolution DepthRes() const;
    Topsens::Orientation Orientation() const;

    void Enable();
    void Disable();

protected:
    void resizeEvent(QResizeEvent*) override;

public:
    Ui::Panel *ui;
};

#endif // PANEL_H

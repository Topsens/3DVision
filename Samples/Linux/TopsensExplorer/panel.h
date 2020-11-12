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

    Topsens::Error Refresh(uint32_t& count);

    Topsens::Orientation Orientation();

    void EnableOptions();
    void DisableOptions();
    void EnableControls();
    void DisableControls();

protected:
    void resizeEvent(QResizeEvent*) override;

public:
    Ui::Panel *ui;
};

#endif // PANEL_H

#include "panel.h"
#include "ui_panel.h"

using namespace Topsens;

Panel::Panel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Panel)
{
    ui->setupUi(this);
    ui->bnStop->setEnabled(false);
    this->ui->cbCres->setCurrentIndex(2);
    this->ui->cbDres->setCurrentIndex(2);
    this->ui->rbUsersYes->setChecked(true);
    this->ui->rbAlignNo->setChecked(true);
    this->ui->rbFlipYes->setChecked(true);
    this->ui->rbLand->setChecked(true);
    this->ui->rbRecordNo->setChecked(true);
    this->ui->rbGroundYes->setChecked(true);
}

Panel::~Panel()
{
    delete ui;
}

void Panel::Count(uint32_t count)
{
    this->ui->cbDevice->clear();

    for (uint32_t i = 0; i < count; i++)
    {
        this->ui->cbDevice->addItem(QString::fromStdString(std::to_string(i)));
    }

    if (count)
    {
        this->ui->cbDevice->setCurrentIndex(0);
        this->Enable();
    }
    else
    {
        this->Disable();
        this->ui->bnRefresh->setEnabled(true);
        this->ui->bnStop->setEnabled(false);
    }
}

bool Panel::Align() const
{
    return this->ui->rbAlignYes->isChecked();
}

bool Panel::Flip() const
{
    return this->ui->rbFlipYes->isChecked();
}

bool Panel::Record() const
{
    return this->ui->rbRecordYes->isChecked();
}

bool Panel::GenUsers() const
{
    return this->ui->rbUsersYes->isChecked();
}

bool Panel::PaintGround() const
{
    return this->ui->rbGroundYes->isChecked();
}

Resolution Panel::ColorRes() const
{
    return (Resolution)this->ui->cbCres->currentIndex();
}

Resolution Panel::DepthRes() const
{
    return (Resolution)this->ui->cbDres->currentIndex();
}

Orientation Panel::Orientation() const
{
    if (this->ui->rbLand->isChecked())
    {
        return Topsens::Orientation::Landscape;
    }

    if (this->ui->rbClock->isChecked())
    {
        return Topsens::Orientation::PortraitClockwise;
    }

    if (this->ui->rbAntic->isChecked())
    {
        return Topsens::Orientation::PortraitAntiClockwise;
    }

    return Topsens::Orientation::Aerial;
}

void Panel::Enable()
{
    this->ui->cbDevice->setEnabled(true);
    this->ui->cbCres->setEnabled(true);
    this->ui->cbDres->setEnabled(true);
    this->ui->gbUsers->setEnabled(true);
    this->ui->gbAlign->setEnabled(true);
    this->ui->gbFlip->setEnabled(true);
    this->ui->gbOrient->setEnabled(true);
    this->ui->gbRecord->setEnabled(true);
    this->ui->gbGround->setEnabled(true);
    this->ui->bnStart->setEnabled(true);
    this->ui->bnRefresh->setEnabled(true);
    this->ui->bnStop->setEnabled(false);
}

void Panel::Disable()
{
    this->ui->cbDevice->setEnabled(false);
    this->ui->cbCres->setEnabled(false);
    this->ui->cbDres->setEnabled(false);
    this->ui->gbUsers->setEnabled(false);
    this->ui->gbAlign->setEnabled(false);
    this->ui->gbFlip->setEnabled(false);
    this->ui->gbOrient->setEnabled(false);
    this->ui->gbRecord->setEnabled(false);
    this->ui->gbGround->setEnabled(false);
    this->ui->bnStart->setEnabled(false);
    this->ui->bnRefresh->setEnabled(false);
    this->ui->bnStop->setEnabled(true);
}

void Panel::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    this->ui->hline->resize(this->width(), this->ui->hline->height());
}

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

Error Panel::Refresh(uint32_t& count)
{
    this->DisableOptions();

    this->ui->cbDevice->clear();

    auto err = Sensor::Count(count);
    if (Error::Ok != err)
    {
        return err;
    }

    if (count > 0)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            this->ui->cbDevice->addItem(QString::fromStdString(std::to_string(i)));
        }

        this->ui->cbDevice->setCurrentIndex(0);
        this->EnableOptions();
    }

    return Error::Ok;
}

Orientation Panel::Orientation()
{
    if (this->ui->rbLand->isChecked())
    {
        return Topsens::Orientation::Landscape;
    }

    if (this->ui->rbClock->isChecked())
    {
        return Topsens::Orientation::PortraitClockwise;
    }

    return Topsens::Orientation::PortraitAntiClockwise;
}

void Panel::EnableOptions()
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
}

void Panel::DisableOptions()
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
}

void Panel::EnableControls()
{
    this->ui->bnRefresh->setEnabled(false);
    this->ui->bnStop->setEnabled(true);
}

void Panel::DisableControls()
{
    this->ui->bnRefresh->setEnabled(true);
    this->ui->bnStop->setEnabled(false);
}

void Panel::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    this->ui->hline->resize(this->width(), this->ui->hline->height());
}

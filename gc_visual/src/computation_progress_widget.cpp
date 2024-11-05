#include "gc_visual/computation_progress_widget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>

ComputationProgressWidget::ComputationProgressWidget(QWidget* parent) :
    QWidget{ parent }
{
    auto layout = new QHBoxLayout{};
    setLayout(layout);

    label_ = new QLabel{};
    layout->addWidget(label_);

    progress_bar_ = new QProgressBar{};
    progress_bar_->setMaximumHeight(20);
    progress_bar_->setMaximumWidth(200);
    layout->addWidget(progress_bar_);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

auto ComputationProgressWidget::set_progress(uint32_t inode,
                                             double node_progress)
    -> void
{
    label_->setText(tr("Node ") + QString::number(inode));
    auto progress_percent = static_cast<int>(node_progress * 100);
    progress_bar_->setValue(progress_percent);
}

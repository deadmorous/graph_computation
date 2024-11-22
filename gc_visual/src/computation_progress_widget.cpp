#include "gc_visual/computation_progress_widget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

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

    auto stop_button = new QPushButton(tr("&Stop"));
    layout->addWidget(stop_button);

    connect(stop_button, &QPushButton::clicked,
            this, &ComputationProgressWidget::stop);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

auto ComputationProgressWidget::set_progress(gc::NodeIndex inode,
                                             double node_progress)
    -> void
{
    label_->setText(tr("Node ") + QString::number(inode.v));
    auto progress_percent = static_cast<int>(node_progress * 100);
    progress_bar_->setValue(progress_percent);
}

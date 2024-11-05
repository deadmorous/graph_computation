#pragma once

#include <QWidget>

class QLabel;
class QProgressBar;

class ComputationProgressWidget :
    public QWidget
{
public:
    explicit ComputationProgressWidget(QWidget* parent = nullptr);

public slots:
    auto set_progress(uint32_t inode, double node_progress)
        -> void;

private:
    QLabel* label_;
    QProgressBar* progress_bar_;
};

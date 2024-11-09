#pragma once

#include <QWidget>

class QLabel;
class QProgressBar;

class ComputationProgressWidget :
    public QWidget
{
    Q_OBJECT

public:
    explicit ComputationProgressWidget(QWidget* parent = nullptr);

signals:
    auto stop()
        -> void;

public slots:
    auto set_progress(uint32_t inode, double node_progress)
        -> void;

private:
    QLabel* label_;
    QProgressBar* progress_bar_;
};

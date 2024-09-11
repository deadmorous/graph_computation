#pragma once

#if 0 // TODO
#include "sequence_generator.hpp"
#include "presentation.hpp"

#include <QImage>
#include <QWidget>

class BitmapView
    : public QWidget
{
Q_OBJECT
public:
    explicit BitmapView(SequenceGeneratorInterface& gen,
                        PresentationInterface& presentation,
                        QWidget* parent = nullptr);

    auto image()
        -> QImage&;

protected:
    auto paintEvent(QPaintEvent*)
        -> void override;

private:
    SequenceGeneratorInterface& gen_;
    PresentationInterface& presentation_;
    QImage img_;
};

#endif // 0


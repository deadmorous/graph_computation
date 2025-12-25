/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include <QFont>
#include <QWidget>

#include <cstdint>
#include <variant>


class Cell2dRuleMapView final :
    public QWidget
{
    Q_OBJECT
public:
    Cell2dRuleMapView(uint8_t max_neighborhood_size, QWidget* parent = nullptr);

    auto map() const noexcept
        -> std::vector<int8_t>;

signals:
    auto map_changed(const std::vector<int8_t>& map) -> void;

public slots:
    auto set_map(const std::vector<int8_t>& map) -> void;
    auto set_state_count(uint8_t state_count) -> void;
    auto set_min_state(int8_t min_state) -> void;

protected:
    auto paintEvent(QPaintEvent*)
        -> void override;

    auto mouseReleaseEvent(QMouseEvent *event)
        -> void override;

private:
    struct RowCol final
    {
        int row;
        int col;
    };

    struct MenuBtn final {};

    struct NoItem final {};

    using Item = std::variant<RowCol, MenuBtn, NoItem>;

    struct Layout final
    {
        QSize lh_item_size;
        QSize th_item_size;
        int row_count{};
        int column_count{};
        int th_skip_factor{1};
        int th_text_width{};
        QRect menu_btn_rect;

        auto size() const noexcept -> QSize ;
        auto lh_item_rect(int row) const noexcept -> QRect;
        auto th_item_rect(int col) const noexcept -> QRect;
        auto item_rect(int row, int col) const noexcept -> QRect;
        auto item_rect(const RowCol& row_col) const noexcept -> QRect;
        auto item(const QPoint& pos) const noexcept -> Item;
    };

    auto map_size() const noexcept -> size_t;

    auto calc_layout() -> void;

    uint8_t max_neighborhood_size_{};
    QFont font_;
    QMenu* menu_;
    QAction* paste_action_;

    uint8_t state_count_{2};
    int8_t min_state_{0};
    std::vector<int8_t> map_;

    Layout layout_;
};

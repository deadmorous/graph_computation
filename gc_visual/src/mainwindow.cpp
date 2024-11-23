#include "gc_visual/mainwindow.hpp"

#include "gc_visual/computation_progress_widget.hpp"
#include "gc_visual/parse_layout.hpp"

#include "gc_app/node_registry.hpp"
#include "gc_app/type_registry.hpp"

#include "gc/computation_node_registry.hpp"
#include "gc/yaml/parse_graph.hpp"

#include "yaml-cpp/yaml.h"

#include <QAction>
#include <QDesktopServices>
#include <QFileDialog>
#include <QKeyCombination>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStatusBar>
#include <QUrl>


namespace {

auto app_settings()
    -> QSettings
{
    constexpr std::string_view organization = "deadmorous";
    constexpr std::string_view application = "gc_visual";
    return QSettings{ QString::fromUtf8(organization),
                      QString::fromUtf8(application) };
}

// Function to retrieve a list of recently opened files
auto recent_files()
    -> QStringList
{
    auto settings = app_settings();
    return settings.value("RecentFiles").toStringList();
}

// Function to add a file to the recent file list
void add_recent_file(const QString& file_path)
{
    constexpr auto max_recent_files = 5;

    auto settings = app_settings();
    auto recent_files = settings.value("RecentFiles").toStringList();

    // Remove the file from the list if it already exists
    recent_files.removeAll(file_path);

    // Add the file to the beginning of the list
    recent_files.prepend(file_path);

    // Truncate the list if it exceeds the maximum number of recent files
    if (recent_files.size() > max_recent_files)
        recent_files.remove(max_recent_files,
                            recent_files.size() - max_recent_files);

    // Save the updated list of recent files
    settings.setValue("RecentFiles", recent_files);
}

auto load_config(const gc_visual::ConfigSpecification& spec)
    -> YAML::Node
{
    switch (spec.type)
    {
    case gc_visual::ConfigSpecificationType::Content:
        return YAML::Load(spec.spec.toUtf8().toStdString());
    case gc_visual::ConfigSpecificationType::FileName:
        return YAML::LoadFile(spec.spec.toUtf8().toStdString());
    }
    __builtin_unreachable();
}

constexpr auto FileSpec =
    gc_visual::ConfigSpecificationType::FileName;

} // anonymous namespace


MainWindow::MainWindow(const gc_visual::ConfigSpecification& spec,
                       QWidget *parent) :
    QMainWindow(parent),
    recents_mapper_{ new QSignalMapper(this) }
{
    resize(800, 600);

    auto menu_bar = new QMenuBar{};

    auto* file_menu = menu_bar->addMenu("&File");
    auto* open_action = file_menu->addAction("&Open", QKeySequence::Open);
    recent_files_ = file_menu->addMenu("Open re&cent");
    reload_recent_files_menu();
    auto* edit_action =
        file_menu->addAction("&Edit", QKeyCombination{Qt::Key_F4});
    auto* reload_action =
        file_menu->addAction("&Reload", QKeyCombination{Qt::Key_F5});
    file_menu->addSeparator();
    auto* quit_action = file_menu->addAction("&Quit", QKeySequence::Quit);

    auto* comp_menu = menu_bar->addMenu("&Computation");
    auto stop_action =
        comp_menu->addAction("&Stop", QKeyCombination{Qt::CTRL, Qt::Key_K});

    setMenuBar(menu_bar);

    computing_time_indicator_ = new QLabel{};
    statusBar()->addPermanentWidget(computing_time_indicator_);

    auto progress_widget = new ComputationProgressWidget{};
    statusBar()->addPermanentWidget(progress_widget);

    connect(open_action, &QAction::triggered, this, &MainWindow::open);
    connect(edit_action, &QAction::triggered, this, &MainWindow::edit);
    connect(reload_action, &QAction::triggered, this, &MainWindow::reload);
    connect(quit_action, &QAction::triggered, this, &QWidget::close);

    connect(recents_mapper_, &QSignalMapper::mappedString,
            this, &MainWindow::open_recent);

    connect(this, &MainWindow::load_finished,
            this, &MainWindow::on_load_finished);

    connect(stop_action, &QAction::triggered,
            &computation_thread_, &ComputationThread::stop);

    connect(&computation_thread_, &ComputationThread::running_state_changed,
            stop_action, &QAction::setEnabled);
    stop_action->setEnabled(false);

    connect(&computation_thread_, &ComputationThread::progress,
            progress_widget, &ComputationProgressWidget::set_progress);
    connect(&computation_thread_, &ComputationThread::running_state_changed,
            progress_widget, &ComputationProgressWidget::setVisible);
    connect(progress_widget, &ComputationProgressWidget::stop,
            stop_action, &QAction::trigger);

    auto set_computing_start_time = [this]
    {
        computing_start_time_ = std::chrono::steady_clock::now();
        computing_end_time_ = computing_start_time_;
        update_computing_time_indicator();
    };

    auto set_computing_end_time = [this]
    {
        computing_end_time_ = std::chrono::steady_clock::now();
        update_computing_time_indicator();
    };

    connect(&computation_thread_, &ComputationThread::started,
            set_computing_start_time);
    connect(&computation_thread_, &ComputationThread::finished,
            set_computing_end_time);
    connect(&computation_thread_, &ComputationThread::progress,
            set_computing_end_time);

    load(spec);
}

auto MainWindow::open()
    -> void
{
    constexpr auto FileSpec =
        gc_visual::ConfigSpecificationType::FileName;

    QString dir;
    if (spec_.type == FileSpec)
        dir = spec_.spec;

    auto file_name =
        QFileDialog::getOpenFileName(
        this,
        tr("Open file"),
        dir,
        "Graph computations (*.gc *.yaml);;All files (*)");

    if (file_name.isEmpty())
        return;

    load({ .type = FileSpec, .spec = file_name });
}

auto MainWindow::open_recent(const QString& file_name)
    -> void
{ load({ .type = FileSpec, .spec = file_name }); }

auto MainWindow::edit()
    -> void
{
    if (spec_.type != FileSpec)
    {
        QMessageBox::critical(
            this,
            tr("Edit current file"),
            tr("Cannot edit current file because it is not specified"));
        return;
    }

    auto fileUrl = QUrl::fromLocalFile(spec_.spec);
    QDesktopServices::openUrl(fileUrl);
}

auto MainWindow::reload()
    -> void
{ load(spec_); }

auto MainWindow::on_load_finished(const gc_visual::ConfigSpecification& spec)
    -> void
{
    if (spec.type == FileSpec)
    {
        add_recent_file(spec.spec);
        reload_recent_files_menu();
    }
}

auto MainWindow::closeEvent(QCloseEvent*)
    -> void
{ computation_thread_.stop(); }

auto MainWindow::load(const gc_visual::ConfigSpecification& spec)
    -> void
{
    try {
        // Parse YAML config
        auto config = load_config(spec);

        // Initialize node registry and type registry
        auto node_registry = gc::computation_node_registry();
        gc_app::populate_node_registry(node_registry);
        auto type_registry = gc::type_registry();
        gc_app::populate_type_registry(type_registry);

        // Parse graph from the node object.
        auto graph_config = config["graph"];
        auto [g, provided_inputs, node_map, input_names] =
            gc::yaml::parse_graph(graph_config, node_registry, type_registry);

        // Compile and compute the graph
        computation_thread_.set_graph(std::move(g), provided_inputs);

        connect(
            &computation_thread_,
            &ComputationThread::finished,
            this,
            [config, node_map, input_names, spec, this]{
                computation_thread_.disconnect(this);

                try {
                    if (!computation_thread_.ok())
                        common::throw_("Graph computation has been terminated");

                    // Create visual layout
                    auto layout = config["layout"];
                    auto central_widget =
                        parse_layout(layout,
                                     computation_thread_,
                                     node_map,
                                     input_names);
                    setCentralWidget(central_widget);

                    spec_ = spec;
                    emit load_finished(spec);
                }
                catch (std::exception& e)
                {
                    setCentralWidget(nullptr);
                    QMessageBox::critical(
                        this, "Load failed", QString::fromUtf8(e.what()));
                }
            });

        computation_thread_.start();
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(this, "Load failed", QString::fromUtf8(e.what()));
    }
}

auto MainWindow::reload_recent_files_menu()
    -> void
{
    recent_files_->clear();
    auto index = 1;
    for (const auto& f : recent_files())
    {
        auto action = new QAction("&" + QString::number(index) + ": " + f);
        recents_mapper_->setMapping(action, f);
        connect(action, &QAction::triggered,
                recents_mapper_, qOverload<>(&QSignalMapper::map));
        recent_files_->addAction(action);
        ++index;
    }
}

auto MainWindow::update_computing_time_indicator()
    -> void
{
    auto dt =
        std::chrono::nanoseconds{ computing_end_time_ - computing_start_time_ }
        .count() / 1e9;
    computing_time_indicator_->setText(QString::number(dt) + " s");
}

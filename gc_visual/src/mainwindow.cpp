#include "gc_visual/mainwindow.hpp"

#include "gc_visual/parse_layout.hpp"

#include "gc_app/node_registry.hpp"
#include "gc_app/type_registry.hpp"

#include "gc/yaml/parse_graph.hpp"

#include "yaml-cpp/yaml.h"

#include <QAction>
#include <QDesktopServices>
#include <QFileDialog>
#include <QKeyCombination>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
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

    setMenuBar(menu_bar);

    connect(open_action, &QAction::triggered, this, &MainWindow::open);
    connect(edit_action, &QAction::triggered, this, &MainWindow::edit);
    connect(reload_action, &QAction::triggered, this, &MainWindow::reload);
    connect(quit_action, &QAction::triggered, this, &QWidget::close);

    connect(recents_mapper_, &QSignalMapper::mappedString,
            this, &MainWindow::open_recent);

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

    if (!load({ .type = FileSpec, .spec = file_name }))
        return;

    if (spec_.type == FileSpec)
    {
        add_recent_file(file_name);
        reload_recent_files_menu();
    }
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


auto MainWindow::load(const gc_visual::ConfigSpecification& spec)
    -> bool
{
    try {
        // Parse YAML config
        auto config = load_config(spec);

        // Initialize node registry and type registry
        auto node_registry = gc::node_registry();
        gc_app::populate_node_registry(node_registry);
        auto type_registry = gc::type_registry();
        gc_app::populate_type_registry(type_registry);

        // Parse graph from the node object.
        auto graph_config = config["graph"];
        auto [g, node_map] =
            gc::yaml::parse_graph(graph_config, node_registry, type_registry);

        // Compile and compute the graph
        computation_ = computation(std::move(g));
        compute(computation_);

        // Create visual layout
        auto layout = config["layout"];
        auto central_widget = parse_layout(layout, computation_, node_map);
        setCentralWidget(central_widget);

        spec_ = spec;
        return true;
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(this, "Load failed", QString::fromUtf8(e.what()));
        return false;
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

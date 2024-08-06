#include <core/HcStorePluginWorker.h>
#include <ui/HcStoreWidget.h>
#include <core/HcHelper.h>

//
// plugin worker thread that downloads plugins to the local filesystem,
// retrieves repository list and displays them into the market store, etc.
//

auto HcStorePluginWorker::RegisterRepository(
    const std::string&              repository,
    const std::vector<std::string>& plugins
) -> void {
    auto repo_file   = QFile();
    auto repo_dir    = QDir();
    auto repo_url    = QUrl( repository.c_str() );
    auto repo_name   = repo_url.path().toStdString();
    auto repo_list   = std::optional<std::string>();
    auto repo_json   = json();
    auto reg_plugins = std::vector<PluginView*>();

    repo_name = repo_name.substr( repo_name.find_last_of( "/" ) + 1 );
    repo_dir  = QDir::homePath() + "/.havoc/client/repositories" + "/" + repo_name.c_str();

    if ( ! repo_dir.exists() ) {
        //
        // create the repository path and the plugins directory
        //
        repo_dir.mkpath( "." );
        QDir( repo_dir.path() + "/plugins" ).mkpath( "." );
    }

    repo_url.setHost( "raw.githubusercontent.com" );
    repo_url.setPath( repo_url.path() + "/main/repository.json" );

    //
    // TODO: only get the content either when the user specifically
    //       wants to update the repo or when there is no file
    //
    repo_file.setFileName( repo_dir.path() + "/repository.json" );

    if ( ! repo_file.exists() ) {
        //
        // install the repository json list locally
        //
        repo_list = HcStoreWidget::HttpGet( repo_url.toString().toStdString() );
        if ( repo_list.has_value() ) {
            if ( repo_file.open( QIODevice::ReadWrite ) ) {
                repo_file.write( repo_list->c_str() );
            } else {
                spdlog::error( "failed to open file {}: {}", repo_file.fileName().toStdString(), repo_file.errorString().toStdString() );
            }
        } else {
            spdlog::error( "failed to retrieve repo list from {}", repository );
        }
    } else {
        if ( repo_file.open( QIODevice::ReadOnly ) ) {
            repo_list = repo_file.readAll().toStdString();
        } else {
            spdlog::error( "failed to open file {}: {}", repo_file.fileName().toStdString(), repo_file.errorString().toStdString() );
            return;
        }
    }

    try {
        if ( ( repo_json = json::parse( repo_list->c_str() ) ).is_discarded() ) {
            spdlog::debug( "invalid repository list from {}: json object has been discarded", repository );
            return;
        }
    } catch ( std::exception& e ) {
        spdlog::debug( "invalid repository list from {}: {}", repository, e.what() );
        return;
    }

    //
    // now iterate over the plugins list and add
    // every single one of them to the store
    //

    if ( repo_json.is_array() ) {
        for ( const auto& plugin : repo_json.get<std::vector<json>>() ) {
            auto process = PluginProcess( repo_dir.path().toStdString(), plugin );

            if ( process.has_value() ) {
                reg_plugins.push_back( process.value() );
            }
        }
    }

    //
    // install plugins specified to be installed
    // after registering the repository
    //

    spdlog::debug( "plugins registered: [plugins: {}] [reg_plugins: {}]", plugins.size(), reg_plugins.size() );

    for ( const auto plugin_install : plugins ) {
        for ( auto plugin : reg_plugins ) {
            //
            // check if the request plugin is in the
            // list and if the plugin is already installed
            //
            if ( ( plugin_install == plugin->name ) && ! plugin->plugin_dir.exists() ) {
                emit PluginIsInstalled( plugin );
            }
        }
    }
}

auto HcStorePluginWorker::RegisterPlugin(
    const std::string& directory,
    const json&        object
) -> void {
    auto plugin = PluginProcess( directory, object, true );

    if ( plugin.has_value() ) {
        emit PluginIsInstalled( plugin.value() );
    }
};

auto HcStorePluginWorker::PluginProcess(
    const std::string& parent,
    const json&        object,
    const bool         local
) -> std::optional<PluginView*> {
    auto readme = std::string();
    auto plugin = new PluginView;

    //
    // parse the json object containing the plugin information
    // such as name, description, readme, categories, etc.
    //
    plugin->mutex.lock();
    plugin->object     = object;

    if ( plugin->object.contains( "name" ) && plugin->object[ "name" ].is_string() ) {
        plugin->name = plugin->object[ "name" ].get<std::string>();
    } else {
        spdlog::error( "PluginProcess error: failed to retrieve plugin name (either not found or not a string)" );
        plugin->mutex.unlock();
        return std::nullopt;
    }

    if ( local ) {
        plugin->plugin_dir = QDir( parent.c_str() );
    } else {
        plugin->plugin_dir.setPath( QString( ( parent + "/plugins/" + plugin->name ).c_str() ) );
    }

    if ( ! local ) {
        if ( plugin->object.contains( "project_url" ) && plugin->object[ "project_url" ].is_string() ) {
            plugin->repo = plugin->object[ "project_url" ].get<std::string>();
        } else {
            spdlog::error( "PluginProcess error: failed to retrieve plugin project url (either not found or not a string)" );
            plugin->mutex.unlock();
            return std::nullopt;
        }
    }

    if ( plugin->object.contains( "readme" ) || plugin->object.contains( "readme_path" ) ) {
        if ( ! plugin->object.contains( "readme_path" ) && ( plugin->object.contains( "readme" ) && plugin->object[ "readme" ].is_string() ) ) {
            readme = plugin->object[ "readme" ].get<std::string>();
        } else if ( plugin->object.contains( "readme_path" ) && plugin->object[ "readme_path" ].is_string() ) {

            if ( local ) {
                auto readme_file = QFile( plugin->plugin_dir.path() + "/" + QString( plugin->object[ "readme_path" ].get<std::string>().c_str() ) );

                if ( ! readme_file.exists() ) {
                    spdlog::error( "PluginProcess error: failed to retrieve plugin readme from {}", readme_file.fileName().toStdString() );
                    plugin->mutex.unlock();
                    return std::nullopt;
                }

                if ( ! readme_file.open( QFile::ReadOnly ) ) {
                    spdlog::error( "PluginProcess error: failed to open plugin readme from {}", readme_file.fileName().toStdString() );
                    plugin->mutex.unlock();
                    return std::nullopt;
                }

                plugin->object[ "readme" ] = readme_file.readAll().toStdString();
            } else {
                auto url  = QUrl( plugin->repo.c_str() );
                auto body = std::optional<std::string>();

                url.setHost( "raw.githubusercontent.com" );
                url.setPath( url.path() + "/main/" + QString( plugin->object[ "readme_path" ].get<std::string>().c_str() ) );

                if ( ( body = HcStoreWidget::HttpGet( url.toString().toStdString() ) ).has_value() ) {
                    plugin->object[ "readme" ] = body.value().c_str();
                } else {
                    spdlog::error( "PluginProcess error: failed to retrieve plugin readme from {}", url.toString().toStdString() );
                    plugin->mutex.unlock();
                    return std::nullopt;
                }
            }
        } else {
            spdlog::error( "PluginProcess error: failed to retrieve plugin readme (either not found or not a string)" );
            plugin->mutex.unlock();
            return std::nullopt;
        }
    } else {
        spdlog::error( "PluginProcess error: failed to retrieve plugin readme name (not found)" );
        plugin->mutex.unlock();
        return std::nullopt;
    }

    emit AddPlugin( parent, plugin->repo, plugin );

    //
    // now just wait til we can lock it again.
    // if we can then the processing of the
    // plugin has been finished.
    //
    plugin->mutex.lock();
    plugin->mutex.unlock();

    return ( plugin );
}

auto HcStorePluginWorker::PluginInstall(
    PluginView* plugin
) -> void {
    auto process   = QProcess();
    auto command   = QStringList();
    auto installed = false;

    //
    // check if the folder already exists.
    // if it does already exist then the
    // worker function is going to uninstall/remove it
    //

    if ( plugin->plugin_dir.exists() ) {
        //
        // uninstall plugin locally
        //

        spdlog::debug( "plugin uninstall: {}", plugin->name );

        if ( ! plugin->plugin_dir.removeRecursively() ) {
            emit MessageBox(
                QMessageBox::Icon::Critical,
                "plugin uninstallation",
                std::format( "failed to remove plugin: {}", plugin->plugin_dir.path().toStdString() )
            );
        } else {
            emit MessageBox(
                QMessageBox::Icon::Information,
                "plugin uninstallation",
                "restart the client to fully remove the loaded plugin"
            );
        }
    } else {
        //
        // installing plugin from remote repo
        //

        spdlog::debug( "installing plugin {}: {}", plugin->repo, plugin->plugin_dir.path().toStdString() );

        command << "-c";
        command << std::format( "git clone {} {}", plugin->repo, plugin->plugin_dir.path().toStdString() ).c_str();

        process.start( "sh", command );

        emit PluginIsInstalling( plugin );

        process.waitForFinished( -1 );

        if ( process.exitCode() ) {
            spdlog::error( "plugin install process failed with code {}:\n{}", process.exitCode(), process.readAll().toStdString() );

            emit MessageBox(
                QMessageBox::Icon::Critical,
                "plugin installation",
                std::format( "failed to install plugin: process existed with {}", process.exitCode() )
            );
        } else {
            if ( ! plugin->plugin_dir.exists() ) {
                emit MessageBox(
                    QMessageBox::Icon::Critical,
                    "plugin installation",
                    std::format( "failed to install plugin: folder not found {}", plugin->plugin_dir.path().toStdString() )
                );
            }
        }
    }

    emit PluginIsInstalled( plugin );
}


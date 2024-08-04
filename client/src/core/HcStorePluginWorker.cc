#include <core/HcStorePluginWorker.h>
#include <ui/HcStoreWidget.h>

//
// plugin worker thread that downloads plugins to the local filesystem,
// retrieves repository list and displays them into the market store, etc.
//

auto HcStorePluginWorker::RegisterRepository(
    const std::string& repository
) -> void {
    auto repo_file = QFile();
    auto repo_dir  = QDir();
    auto repo_url  = QUrl( repository.c_str() );
    auto repo_name = repo_url.path().toStdString();
    auto repo_list = std::optional<std::string>();
    auto repo_json = json();

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
            PluginProcess( repository, plugin );
        }
    }
}

auto HcStorePluginWorker::PluginProcess(
    const std::string& parent,
    const json&        object
) -> void {
    auto plugin_name = std::string();
    auto project_url = std::string();
    auto categories  = std::vector<std::string>();
    auto description = std::string();
    auto version     = std::string();
    auto license     = std::string();
    auto readme      = std::string();
    auto author_name = std::string();
    auto author_url  = std::string();
    auto plugin      = new PluginView;

    //
    // parse the json object containing the plugin information
    // such as name, description, readme, categories, etc.
    //
    plugin->object = object;

    if ( plugin->object.contains( "project_url" ) && plugin->object[ "project_url" ].is_string() ) {
        project_url = plugin->object[ "project_url" ].get<std::string>();
    } else {
        spdlog::error( "PluginProcess error: failed to retrieve plugin project url (either not found or not a string)" );
        return;
    }

    if ( plugin->object.contains( "readme" ) || plugin->object.contains( "readme_path" ) ) {
        if ( ! plugin->object.contains( "readme_path" ) && plugin->object.contains( "readme" ) && plugin->object[ "readme" ].is_string() ) {
            readme = plugin->object[ "readme" ].get<std::string>();
        } else if ( plugin->object.contains( "readme_path" ) && plugin->object[ "readme_path" ].is_string() ) {
            auto url  = QUrl( project_url.c_str() );
            auto body = std::optional<std::string>();

            url.setHost( "raw.githubusercontent.com" );
            url.setPath( url.path() + "/main/" + QString( plugin->object[ "readme_path" ].get<std::string>().c_str() ) );

            if ( ( body = HcStoreWidget::HttpGet( url.toString().toStdString() ) ).has_value() ) {
                plugin->object[ "readme" ] = body.value().c_str();
            } else {
                spdlog::error( "PluginProcess error: failed to retrieve plugin readme from {}", url.toString().toStdString() );
            }
        } else {
            spdlog::error( "PluginProcess error: failed to retrieve plugin readme (either not found or not a string)" );
            return;
        }
    } else {
        spdlog::error( "PluginProcess error: failed to retrieve plugin readme name (not found)" );
        return;
    }

    emit AddPlugin( parent, project_url, plugin );
};
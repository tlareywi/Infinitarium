#include "../config.h"

#if (USE_CESIUM)

#include "Cesium.hpp"

#include "GenericAssetAccessor.hpp"
#include "HttpAssetAccessor.hpp"
#include "TaskProcessor.hpp"

using namespace Cesium;

BOOST_CLASS_EXPORT_IMPLEMENT(Terrain)

static std::unique_ptr<CesiumSystem> cesuim = std::make_unique<CesiumSystem>();

void Terrain::LoadTileset()
{
    if (!renderResourcePreparer)
        renderResourcePreparer = std::make_shared<RenderResourcesPreparer>();

    switch (sourceType)
    {
    case Terrain::Source::Local:
        //LoadTilesetFromLocalFile(*tilesetSource.GetLocalFile(), renderConfiguration);
        break;
    case Terrain::Source::Remote:
        //LoadTilesetFromUrl(*tilesetSource.GetUrl(), renderConfiguration);
        break;
    case Terrain::Source::Ion:
        LoadTilesetFromCesiumIon();
        break;
    default:
        break;
    }
}

/* void Terrain::LoadTilesetFromLocalFile(const TilesetLocalFileSource& source, const TilesetRenderConfiguration& renderConfiguration)
{
    if (source.m_filePath.empty())
    {
        return;
    }

    Cesium3DTilesSelection::TilesetExternals externals = CreateTilesetExternal(IOKind::LocalFile);
    Cesium3DTilesSelection::TilesetOptions options;
    options.contentOptions.generateMissingNormalsSmooth = renderConfiguration.m_generateMissingNormalAsSmooth;
    m_tileset = AZStd::make_unique<Cesium3DTilesSelection::Tileset>(externals, source.m_filePath.c_str(), options);
}

void Terrain::LoadTilesetFromUrl(const TilesetUrlSource& source, const TilesetRenderConfiguration& renderConfiguration)
{
    if (source.m_url.empty())
    {
        return;
    }

    Cesium3DTilesSelection::TilesetExternals externals = CreateTilesetExternal(IOKind::Http);
    Cesium3DTilesSelection::TilesetOptions options;
    options.contentOptions.generateMissingNormalsSmooth = renderConfiguration.m_generateMissingNormalAsSmooth;
    m_tileset = AZStd::make_unique<Cesium3DTilesSelection::Tileset>(externals, source.m_url.c_str(), options);
} */

void Terrain::LoadTilesetFromCesiumIon()
{
    if (ionToken.empty()) {
        std::cout << "Missing access token for Ion terrain source." << std::endl;
        return;
    }

    Cesium3DTilesSelection::TilesetExternals externals = CreateTilesetExternal(CesiumSystem::IOKind::Http);
    Cesium3DTilesSelection::TilesetOptions options;
    options.contentOptions.generateMissingNormalsSmooth = true;
    tileSet = std::make_unique<Cesium3DTilesSelection::Tileset>(
        externals, sourceStr.c_str() /*Asset Id*/, ionToken.c_str(), options);
}

Cesium3DTilesSelection::TilesetExternals Terrain::CreateTilesetExternal(CesiumSystem::IOKind kind)
{
    return Cesium3DTilesSelection::TilesetExternals{
        cesuim->GetAssetAccessor(kind),
        renderResourcePreparer,
        CesiumAsync::AsyncSystem(cesuim->GetTaskProcessor()),
        cesuim->GetCreditSystem(),
        cesuim->GetLogger(),
    };
}

template<class Archive> void Terrain::serialize(Archive& ar, const unsigned int version) {
    ar & sourceType;
    ar & sourceStr;
    ar & ionToken;

    std::cout << "Serializing Terrain" << std::endl;
}

CesiumSystem::CesiumSystem()
{
    // initialize IO managers
    httpManager = std::make_unique<HttpManager>();
    localFileManager = std::make_unique<LocalFileManager>();

    // initialize asset accessors
    httpAssetAccessor = std::make_shared<HttpAssetAccessor>(httpManager.get());
    localFileAssetAccessor = std::make_shared<GenericAssetAccessor>(localFileManager.get(), "");

    // initialize task processor
    taskProcessor = std::make_shared<TaskProcessor>();

    // initialize credit system
    creditSystem = std::make_shared<Cesium3DTilesSelection::CreditSystem>();

    // initialize logger
    logger = spdlog::default_logger();
//    logger->sinks().clear();
//    logger->sinks().push_back(std::make_shared<LoggerSink>());
}

GenericIOManager& CesiumSystem::GetIOManager(IOKind kind)
{
    switch (kind)
    {
    case CesiumSystem::IOKind::LocalFile:
        return *localFileManager;
    case CesiumSystem::IOKind::Http:
        return *httpManager;
    default:
        return *httpManager;
    }
}

const std::shared_ptr<CesiumAsync::IAssetAccessor>& CesiumSystem::GetAssetAccessor(IOKind kind) const
{
    switch (kind)
    {
    case CesiumSystem::IOKind::LocalFile:
        return localFileAssetAccessor;
    case CesiumSystem::IOKind::Http:
        return httpAssetAccessor;
    default:
        return httpAssetAccessor;
    }
}

const std::shared_ptr<CesiumAsync::ITaskProcessor>& CesiumSystem::GetTaskProcessor() const
{
    return taskProcessor;
}

const std::shared_ptr<spdlog::logger>& CesiumSystem::GetLogger() const
{
    return logger;
}

const std::shared_ptr<Cesium3DTilesSelection::CreditSystem>& CesiumSystem::GetCreditSystem() const
{
    return creditSystem;
}

const CriticalAssetManager& CesiumSystem::GetCriticalAssetManager() const
{
    return criticalAssetManager;
}

#endif
#if (USE_CESIUM)

#pragma once

#include "../Renderable.hpp"

#include "LocalFileManager.hpp"
#include "HttpManager.hpp"
#include "CriticalAssetManager.hpp"
#include "RenderResourcesPreparer.hpp"

#include <Cesium3DTilesSelection/Tileset.h>
#include <Cesium3DTilesSelection/TilesetExternals.h>
#include <Cesium3DTilesSelection/RasterOverlay.h>

#include <memory>
#include <string>

#include <boost/serialization/export.hpp>

class Terrain : public IRenderable {
public:
	enum class Source {
		Local,
		Remote,
		Ion
	};

	void setIonToken(const std::string& t) {
		ionToken = t;
	}

	void setSourcePath(const std::string& p) {
		sourceStr = p;
	}

	void setSourceType( Source s ) {
		sourceType = s;
	}

private:
	void LoadTileset();
	Cesium3DTilesSelection::TilesetExternals CreateTilesetExternal(CesiumSystem::IOKind kind);
	void LoadTilesetFromCesiumIon();
	friend class boost::serialization::access;
	template<class Archive> void serialize(Archive&, unsigned int);

	Source sourceType;
	std::string sourceStr;
	std::string ionToken;

	std::unique_ptr<Cesium3DTilesSelection::Tileset> tileSet{ nullptr };
	std::shared_ptr<Cesium3DTilesSelection::IPrepareRendererResources> renderResourcePreparer{ nullptr };
};

class CesiumSystem final
{
public:
	enum class IOKind
	{
		LocalFile,
		Http
	};

	CesiumSystem();
    ~CesiumSystem() noexcept = default;

	Cesium::GenericIOManager& GetIOManager(IOKind kind);

    const std::shared_ptr<CesiumAsync::IAssetAccessor>& GetAssetAccessor(IOKind kind) const;

    const std::shared_ptr<CesiumAsync::ITaskProcessor>& GetTaskProcessor() const;

    const std::shared_ptr<spdlog::logger>& GetLogger() const;

    const std::shared_ptr<Cesium3DTilesSelection::CreditSystem>& GetCreditSystem() const;

    const Cesium::CriticalAssetManager& GetCriticalAssetManager() const;

private:
    std::unique_ptr<Cesium::HttpManager> httpManager;
	std::unique_ptr<Cesium::LocalFileManager> localFileManager;
    std::shared_ptr<CesiumAsync::IAssetAccessor> httpAssetAccessor;
    std::shared_ptr<CesiumAsync::IAssetAccessor> localFileAssetAccessor;
    std::shared_ptr<CesiumAsync::ITaskProcessor> taskProcessor;
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<Cesium3DTilesSelection::CreditSystem> creditSystem;
	Cesium::CriticalAssetManager criticalAssetManager;
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(IRenderTarget)
BOOST_CLASS_EXPORT_KEY(Terrain)

#endif
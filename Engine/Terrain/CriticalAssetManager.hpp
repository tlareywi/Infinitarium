#pragma once

//#include <Atom/RPI.Reflect/Material/MaterialTypeAsset.h>
//#include <AzFramework/Asset/AssetCatalogBus.h>
//#include <AzCore/Asset/AssetCommon.h>
#include <atomic>

namespace AzFramework {
    namespace AssetCatalogEventBus {
        class Handler {
        public:
            virtual void OnCatalogLoaded(const char* catalogFile) = 0;
        };
    }
}

namespace Cesium
{
    class CriticalAssetManager : public AzFramework::AssetCatalogEventBus::Handler
    {
    public:
        CriticalAssetManager();

        ~CriticalAssetManager() noexcept;

        void OnCatalogLoaded(const char* catalogFile) override;

       // AZ::Data::AssetId GenerateRandomAssetId() const;

       // AZ::Data::Asset<AZ::RPI::MaterialTypeAsset> m_standardPbrMaterialType;
       // AZ::Data::Asset<AZ::RPI::MaterialTypeAsset> m_rasterMaterialType;

    private:
        static constexpr const char* const STANDARD_PBR_MAT_TYPE = "Materials/Types/StandardPBR.azmaterialtype";
        static constexpr const char* const RASTER_MAT_TYPE = "Materials/Types/GltfStandardPBR.azmaterialtype";
    };
} // namespace Cesium

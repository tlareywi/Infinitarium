#pragma once

#include <CesiumAsync/AsyncSystem.h>
#include <CesiumAsync/Future.h>

#include <string>
#include <cstddef>
#include <vector>

namespace Cesium
{
    struct IORequestParameter
    {
        std::string m_parentPath;
        std::string m_path;
    };

    using IOContent = std::vector<std::byte>;

    class GenericIOManager
    {
    public:
        virtual ~GenericIOManager() = default;

        virtual std::string GetParentPath(const std::string& path) = 0;

        virtual IOContent GetFileContent(const IORequestParameter& request) = 0;

        virtual IOContent GetFileContent(IORequestParameter&& request) = 0;

        virtual CesiumAsync::Future<IOContent> GetFileContentAsync(
            const CesiumAsync::AsyncSystem& asyncSystem, const IORequestParameter& request) = 0;

        virtual CesiumAsync::Future<IOContent> GetFileContentAsync(
            const CesiumAsync::AsyncSystem& asyncSystem, IORequestParameter&& request) = 0;
    };
} // namespace Cesium

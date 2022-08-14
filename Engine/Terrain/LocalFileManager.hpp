#pragma once

#include "GenericIOManager.hpp"
#include <CesiumAsync/AsyncSystem.h>
#include <CesiumAsync/Future.h>

#include <string>
#include <memory>

namespace AZ
{
    class JobManager;
    class JobContext;
    class Job;
} // namespace AZ

namespace Cesium
{
    class LocalFileManager final : public GenericIOManager
    {
        struct RequestHandler;

    public:
        LocalFileManager();

        std::string GetParentPath(const std::string& path) override;

        IOContent GetFileContent(const IORequestParameter& request) override;

        IOContent GetFileContent(IORequestParameter&& request) override;

        CesiumAsync::Future<IOContent> GetFileContentAsync(
            const CesiumAsync::AsyncSystem& asyncSystem, const IORequestParameter& request) override;

        CesiumAsync::Future<IOContent> GetFileContentAsync(
            const CesiumAsync::AsyncSystem& asyncSystem, IORequestParameter&& request) override;

    private:
        std::unique_ptr<AZ::JobManager> m_ioJobManager;
        std::unique_ptr<AZ::JobContext> m_ioJobContext;
    };
} // namespace Cesium

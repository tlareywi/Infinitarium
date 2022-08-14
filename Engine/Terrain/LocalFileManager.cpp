#include "LocalFileManager.hpp"
#include <CesiumAsync/Promise.h>
#include <fstream>

namespace Cesium
{
    struct LocalFileManager::RequestHandler
    {
        RequestHandler(const IORequestParameter& request, const CesiumAsync::Promise<IOContent>& promise)
            : m_request{ request }
            , m_promise{ promise }
        {
        }

        RequestHandler(IORequestParameter&& request, const CesiumAsync::Promise<IOContent>& promise)
            : m_request{ std::move(request) }
            , m_promise{ promise }
        {
        }

        void operator()()
        {
            std::string absolutePath;
            if (m_request.m_parentPath.empty())
            {
                absolutePath = m_request.m_path;
            }
            else if (m_request.m_path.empty())
            {
                absolutePath = m_request.m_parentPath;
            }
            else
            {
                assert(false);
//                AZ::StringFunc::Path::Join(m_request.m_parentPath.c_str(), m_request.m_path.c_str(), absolutePath);
            }

            std::ifstream stream(absolutePath.c_str(), std::fstream::binary );
            if (!stream.is_open())
            {
                m_promise.resolve(IOContent{});
            }
            else
            {
                // Create a buffer.
                std::size_t fileSize = stream;
                IOContent content(fileSize);
                stream.read(fileSize, content.data());
                m_promise.resolve(std::move(content));
            }
        }

        IORequestParameter m_request;
        CesiumAsync::Promise<IOContent> m_promise;
    };

    LocalFileManager::LocalFileManager()
    {
        AZ::JobManagerDesc jobDesc;
        for (size_t i = 0; i < 2; ++i)
        {
            jobDesc.m_workerThreads.push_back({ static_cast<int>(i) });
        }
        m_ioJobManager = AZStd::make_unique<AZ::JobManager>(jobDesc);
        m_ioJobContext = AZStd::make_unique<AZ::JobContext>(*m_ioJobManager);
    }

    std::string LocalFileManager::GetParentPath(const std::string& path)
    {
        std::string parentPath(path);
        AZ::StringFunc::Path::StripFullName(parentPath);
        return parentPath;
    }

    IOContent LocalFileManager::GetFileContent(const IORequestParameter& request)
    {
        std::string absolutePath;
        if (request.m_parentPath.empty())
        {
            absolutePath = request.m_path;
        }
        else if (request.m_path.empty())
        {
            absolutePath = request.m_parentPath;
        }
        else
        {
            assert(false);
//            AZ::StringFunc::Path::Join(request.m_parentPath.c_str(), request.m_path.c_str(), absolutePath);
        }

        AZ::IO::FileIOStream stream(absolutePath.c_str(), AZ::IO::OpenMode::ModeRead | AZ::IO::OpenMode::ModeBinary);
        if (!stream.IsOpen())
        {
            return {};
        }

        // Create a buffer.
        std::size_t fileSize = stream.GetLength();
        IOContent content(fileSize);
        stream.Read(fileSize, content.data());
        return content;
    }

    IOContent LocalFileManager::GetFileContent(IORequestParameter&& request)
    {
        return GetFileContent(request);
    }

    CesiumAsync::Future<IOContent> LocalFileManager::GetFileContentAsync(
        const CesiumAsync::AsyncSystem& asyncSystem, const IORequestParameter& request)
    {
        auto promise = asyncSystem.createPromise<IOContent>();
        std::function<void()>* job = new std::function<void()>(RequestHandler{ request, promise });
        std::thread(job).detach();
        return promise.getFuture();
    }

    CesiumAsync::Future<IOContent> LocalFileManager::GetFileContentAsync(
        const CesiumAsync::AsyncSystem& asyncSystem, IORequestParameter&& request)
    {
        auto promise = asyncSystem.createPromise<IOContent>();
        std::function<void()>* job =
            new std::function<void()>(RequestHandler{ std::move(request), promise });
        std::thread(job).detach();
        return promise.getFuture();
    }
} // namespace Cesium

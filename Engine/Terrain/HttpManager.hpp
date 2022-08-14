#pragma once

#include "GenericIOManager.hpp"

#include <string>
#include <memory>

#include <CesiumAsync/AsyncSystem.h>
#include <CesiumAsync/Future.h>
#include <CesiumAsync/HttpHeaders.h>

#include <aws/core/http/HttpResponse.h>

namespace AZ
{
    class JobManager;
    class JobContext;
    class Job;
} // namespace AZ

namespace Aws
{
    namespace Http
    {
        class HttpClient;
    }
} // namespace Aws

namespace Cesium
{
    struct HttpRequestParameter final
    {
        HttpRequestParameter(std::string&& url, Aws::Http::HttpMethod method)
            : m_url{ std::move(url) }
            , m_method{ method }
        {
        }

        HttpRequestParameter(std::string&& url, Aws::Http::HttpMethod method, CesiumAsync::HttpHeaders&& headers)
            : m_url{ std::move(url) }
            , m_method{ method }
            , m_headers{ std::move(headers) }
        {
        }

        HttpRequestParameter(std::string&& url, Aws::Http::HttpMethod method, CesiumAsync::HttpHeaders&& headers, std::string&& body)
            : m_url{ std::move(url) }
            , m_method{ method }
            , m_headers{ std::move(headers) }
            , m_body{ std::move(body) }
        {
        }

        std::string m_url;

        Aws::Http::HttpMethod m_method;

        CesiumAsync::HttpHeaders m_headers;

        std::string m_body;
    };

    struct HttpResult final
    {
        std::shared_ptr<Aws::Http::HttpRequest> m_request;
        std::shared_ptr<Aws::Http::HttpResponse> m_response;
    };

    class HttpManager final : public GenericIOManager
    {
        struct RequestHandler;
        struct GenericIORequestHandler;

    public:
        HttpManager();

        ~HttpManager() noexcept;

        CesiumAsync::Future<HttpResult> AddRequest(
            const CesiumAsync::AsyncSystem& asyncSystem, HttpRequestParameter&& httpRequestParameter);

        std::string GetParentPath(const std::string& path) override;

        IOContent GetFileContent(const IORequestParameter& request) override;

        IOContent GetFileContent(IORequestParameter&& request) override;

        CesiumAsync::Future<IOContent> GetFileContentAsync(
            const CesiumAsync::AsyncSystem& asyncSystem, const IORequestParameter& request) override;

        CesiumAsync::Future<IOContent> GetFileContentAsync(
            const CesiumAsync::AsyncSystem& asyncSystem, IORequestParameter&& request) override;

        static IOContent GetResponseBodyContent(Aws::Http::HttpResponse& response);

    private:
        std::unique_ptr<AZ::JobManager> m_ioJobManager;
        std::unique_ptr<AZ::JobContext> m_ioJobContext;
        std::shared_ptr<Aws::Http::HttpClient> m_awsHttpClient;
    };
} // namespace Cesium

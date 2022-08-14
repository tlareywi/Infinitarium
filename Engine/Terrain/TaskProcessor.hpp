#pragma once

#include <memory>
#include <CesiumAsync/ITaskProcessor.h>

namespace Cesium
{
    class TaskProcessor : public CesiumAsync::ITaskProcessor
    {
    public:
        TaskProcessor();

        ~TaskProcessor() noexcept;

        void startTask(std::function<void()> task) override;

    private:
    };
} // namespace Cesium

#include "TaskProcessor.hpp"
#include <thread>

namespace Cesium
{
    TaskProcessor::TaskProcessor()
    {

    }

    TaskProcessor::~TaskProcessor() noexcept
    {
    }

    void TaskProcessor::startTask(std::function<void()> task)
    {
        std::thread(task).detach();
    }
} // namespace Cesium

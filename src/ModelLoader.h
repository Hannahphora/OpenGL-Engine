#pragma once

#include "Model.h"

#include <future>
#include <vector>
#include <memory>
#include <chrono>

class ModelLoader {
public:

    struct Task {
        std::string path;
        std::shared_ptr<Model> model;
        std::future<void> asyncTask;
    };

    void loadModelAsync(const std::string& path);
    void update();
    bool allModelsLoaded() const;

    std::vector<std::shared_ptr<Model>> getLoadedModels() const;

private:
    std::vector<Task> tasks;
    std::vector<std::shared_ptr<Model>> loadedModels;
};
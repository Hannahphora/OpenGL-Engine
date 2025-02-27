#include "ModelLoader.h"

void ModelLoader::loadModelAsync(const std::string& path) {
    Task task;
    task.path = path;
    task.model = std::make_shared<Model>();
    task.asyncTask = std::async(std::launch::async, [model = task.model, path]() {
        model->loadRawData(path);
    });
    tasks.emplace_back(std::move(task));
}

void ModelLoader::update() {
    for (int i = 0; i < tasks.size(); i++) {
        if (tasks[i].asyncTask.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            tasks[i].model->createGLResources();
            loadedModels.emplace_back(tasks[i].model);
            tasks.erase(tasks.begin() + i);
        }
    }
}

bool ModelLoader::allModelsLoaded() const {
    return tasks.size();
}

std::vector<std::shared_ptr<Model>> ModelLoader::getLoadedModels() const {
    return loadedModels;
}
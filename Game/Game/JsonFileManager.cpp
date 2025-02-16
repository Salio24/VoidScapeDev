#include "JsonFileManager.hpp"

bool JsonManager::saveSettings(const Settings& settings, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) return false;

        json j = settings;
        file << j.dump(4);
        return true;
    }
    catch (...) {
        return false;
    }
}

Settings JsonManager::loadSettings(const std::string& filename) {
    Settings settings;
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            saveSettings(settings, filename);
            return settings;
        }

        json j;
        file >> j;
        settings = j.get<Settings>();

        settings.SFXVolume = std::clamp(settings.SFXVolume, 0, 10);
        settings.MusicVolume = std::clamp(settings.MusicVolume, 0, 10);
        settings.textSizeMultiplier = std::max(0.1f, settings.textSizeMultiplier);
    }
    catch (...) {
        settings = Settings{};
        saveSettings(settings, filename);
    }
    return settings;
}

bool JsonManager::saveSaveData(const SaveData& saveData, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) return false;

        json j = saveData;
        file << j.dump(4);
        return true;
    }
    catch (...) {
        return false;
    }
}

SaveData JsonManager::loadSaveData(const std::string& filename) {
    SaveData saveData;
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            saveSaveData(saveData, filename);
            return saveData;
        }

        json j;
        file >> j;
        saveData = j.get<SaveData>();

        saveData.Level_1_best_levelTime_minutes = std::max(0, saveData.Level_1_best_levelTime_minutes);
        saveData.Level_1_best_levelTime_seconds = std::clamp(saveData.Level_1_best_levelTime_seconds, 0, 59);
        saveData.Level_1_best_levelTime_centiseconds = std::clamp(saveData.Level_1_best_levelTime_centiseconds, 0, 99);
    }
    catch (...) {
        saveData = SaveData{};
        saveSaveData(saveData, filename);
    }
    return saveData;
}

bool JsonManager::saveControls(const Controls& controls, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) return false;

        json j = controls;
        file << j.dump(4);
        return true;
    }
    catch (...) {
        return false;
    }
}

Controls JsonManager::loadControls(const std::string& filename) {
    Controls controls;
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            saveControls(controls, filename);
            return controls;
        }

        json j;
        file >> j;
        controls = j.get<Controls>();
    }
    catch (...) {
        controls = Controls{};
        saveControls(controls, filename);
    }
    return controls;
}

bool JsonManager::saveConfig(const Config& config, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) return false;

        json j = config;
        file << j.dump(4);
        return true;
    }
    catch (...) {
        return false;
    }
}


Config JsonManager::loadConfig(const std::string& filename) {
    Config config;
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            saveConfig(config, filename);
            return config;
        }

        json j;
        file >> j;
        config = j.get<Config>();
    }
    catch (...) {
        config = Config{};
        saveConfig(config, filename);
    }
    return config;
}
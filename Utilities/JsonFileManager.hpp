#pragma once
#include <nlohmannjson/json.hpp>
#include "HelperStructs.hpp"
#include <string>

using json = nlohmann::json;

class JsonManager {
public:
    static bool saveSettings(const Settings& settings, const std::string& filename = "saveData/settings.json");
    static Settings loadSettings(const std::string& filename = "saveData/settings.json");

    static bool saveSaveData(const SaveData& saveData, const std::string& filename = "saveData/save_data.json");
    static SaveData loadSaveData(const std::string& filename = "saveData/save_data.json");

    static bool saveControls(const Controls& controls, const std::string& filename = "saveData/controls.json");
    static Controls loadControls(const std::string& filename = "saveData/controls.json");

    static bool saveConfig(const Config& config, const std::string& filename = "saveData/config.json");
    static Config loadConfig(const std::string& filename = "saveData/config.json");
};
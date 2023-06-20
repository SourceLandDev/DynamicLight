//
// Created by RedbeanW on 10/30/2022.
//

#include "Config.h"

#include "llapi/mc/BlockTypeRegistry.hpp"
#include "llapi/mc/Brightness.hpp"
#include "llapi/mc/Block.hpp"

Config config;

void Config::loadFromFile(const string &path) {
    mPath = path;
    if (!std::filesystem::exists(mPath)) {
        logger.warn("Configuration file not found, creating...");
        _save();
    }
    std::ifstream file;
    file.open(mPath);
    if (!file.is_open()) {
        logger.warn("Unable to open file, using default configuration.");
        return;
    }
    try {
        auto j = json::parse(file);
        _fromJson(j);
    } catch(json::exception& e) {
        logger.error("An exception occurred while reading the configuration file.");
        logger.error(e.what());
        logger.error("Preparing new configuration file, please wait...");
        _save();
    }
    file.close();
}

string Config::_toString() {
    json cfg = {
            {"version", mVersion},
            {"enabled", mEnabled},
            {"enableItemEntity", mEnableItemActor},
            {"enableAutoGlowingBlockDiscover", mAutoDiscoverGlowingBlock},
            {"enableUnderWater", mEnableUnderWater}
    };
    return cfg.dump(4);
}

bool Config::_fromJson(json &cfg) {
    _update(cfg);
    cfg.at("enabled").get_to(mEnabled);
    cfg.at("enableItemEntity").get_to(mEnableItemActor);
    cfg.at("enableAutoGlowingBlockDiscover").get_to(mAutoDiscoverGlowingBlock);
    cfg.at("enableUnderWater").get_to(mEnableUnderWater);
    return true;
}

void Config::_update(json& cfg) {
    if (!cfg.contains("version") || !cfg.at("version").is_number_integer()) {
        _save();
        return;
    }
    int version = cfg["version"];
    bool needSave = false;
    if (version < 200) {
        cfg["enableItemEntity"] = true;
        cfg["enableAutoGlowingBlockDiscover"] = true;
        cfg["version"] = 200;
        needSave = true;
    }
    if (version < 211) {
        cfg["enableUnderWater"] = true;
        cfg["version"] = 211;
        needSave = true;
    }
    if (needSave) {
        logger.warn("The configuration file has been updated to v{}.", mVersion);
        _save();
    }
}

void Config::_save() {
    std::ofstream ofile;
    ofile.open(mPath);
    ofile << _toString();
    ofile.close();
}

unsigned int Config::getBrightness(const ItemStack& item, bool isInWaterOrRain) {
    if (item.isNull() || !item.isBlock())
        return 0;
    if (isInWaterOrRain) {
        auto typeName = item.getTypeName();
        if (typeName.contains("torch") || typeName.contains("fire"))
            return 0;
    }
    return item.getBlock()->getLightEmission().value;
}

bool Config::isEnabled() const {
    return mEnabled;
}

bool Config::isItemActorEnabled() const {
    return mEnableItemActor;
}

bool Config::isUnderWaterEnabled() const {
    return mEnableUnderWater;
}

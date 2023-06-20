//
// Created by RedbeanW on 10/30/2022.
//

#pragma once

#include "Plugin.h"

#include "Nlohmann/json.hpp"

#include "llapi/EventAPI.h"
#include "llapi/mc/HashedString.hpp"
#include "llapi/mc/ItemStack.hpp"

using json = nlohmann::json;

class Config {
public:

    void loadFromFile(const string&);

    [[nodiscard]] bool isEnabled() const;

    [[nodiscard]] bool isItemActorEnabled() const;

    [[nodiscard]] unsigned int getBrightness(const ItemStack&, bool = false);

    [[nodiscard]] bool isUnderWaterEnabled() const;

private:

    bool _fromJson(json&);

    [[nodiscard]] std::string _toString();

    // Call after read is complete.
    void _computeLightBlocks();

    void _update(json&);

    void _save();

private:

    std::string mPath;

    const unsigned int mVersion = 211;

    bool mEnabled = true;
    bool mEnableItemActor = true;
    bool mAutoDiscoverGlowingBlock = true;
    bool mEnableUnderWater = true;

};

extern Config config;

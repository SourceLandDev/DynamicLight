//
// Created by RedbeanW on 10/30/2022.
//

#include "Plugin.h"
#include "Config.h"

#include "llapi/EventAPI.h"

#include "llapi/mc/Player.hpp"
#include "llapi/mc/Container.hpp"

#include <filesystem>

using namespace ll;
#define DATA_PATH "plugins/MovingLight/"

Logger logger("MovingLight");
Version PLUGIN_VERSION { 2, 1, 2, Version::Release };

void PluginMain() {
    logger.info("loaded, ver " + PLUGIN_VERSION.toString() + ", author: redbeanw.");
}

void PluginInit() {

    if (!std::filesystem::exists(DATA_PATH)) {
        logger.warn("Could not find the directory for the configuration file, creating...");
        std::filesystem::create_directory(DATA_PATH);
    }

    config.loadFromFile(DATA_PATH"config.json");
    
    // Register plugin to LL.

    ll::registerPlugin("MovingLight", "Items always emit light~", PLUGIN_VERSION, {
        {"Author","RedbeanW"},
        {"Github","https://github.com/Redbeanw44602/MovingLight"}
    });

    Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) {
        PluginMain();
        return true;
    });

    // OffHand Helper

    Event::PlayerAttackBlockEvent::subscribe([](Event::PlayerAttackBlockEvent ev) {
        if (!config.isEnabled() ||
            ev.mItemStack->isNull() ||
            !ev.mPlayer->getOffhandSlot().isNull() ||
            !config.getBrightness(*ev.mItemStack))
            return true;
        ev.mPlayer->setOffhandSlot(*ev.mItemStack);
        ev.mPlayer->getInventory().removeItem_s(ev.mPlayer->getSelectedItemSlot(), ev.mItemStack->getCount());
        ev.mPlayer->sendInventory(true);
        return false;
    });

}
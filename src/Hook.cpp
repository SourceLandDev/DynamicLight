//
// Created by RedbeanW on 10/30/2022.
//

#include "Plugin.h"

#include "Config.h"
#include "LightMgr.h"

#include "llapi/mc/Player.hpp"
#include "llapi/mc/Container.hpp"
#include "llapi/mc/ItemActor.hpp"
#include "llapi/mc/Level.hpp"
#include "llapi/mc/PrimedTnt.hpp"
#include "llapi/mc/StaticVanillaBlocks.hpp"

// Remove

TInstanceHook(ItemActor *, "??_EItemActor@@UEAAPEAXI@Z",
              ItemActor, char a2) {
    lightMgr.clear((identity_t)this);
    return original(this, a2);
}

TInstanceHook(void, "??1Player@@UEAA@XZ",
              Player) {
    original(this);
    lightMgr.clear((identity_t)this);
}

TInstanceHook(void, "?explode@Level@@UEAAXAEAVBlockSource@@PEAVActor@@AEBVVec3@@M_N3M3@Z",
              Level, class BlockSource& a2, class Actor* a3, class Vec3 const& a4, float a5, bool a6, bool a7, float a8, bool a9) {
    original(this, a2, a3, a4, a5, a6, a7, a8, a9);
    lightMgr.clear((identity_t)a3);
}

// Tick

TInstanceHook(void, "?normalTick@Mob@@UEAAXXZ",
              Mob) {
    original(this);
    if (!config.isEnabled() || !hasDimension())
        return;
    if (isInvisible() || isSpectator())
        lightMgr.clear((identity_t)this);
    else if (isOnFire() || isIgnited())
        lightMgr.turnOn((identity_t)this, getDimension(), getBlockPos(), LightMgr::fireLightLevel, isImmersedInWater());
    else {
        auto inWaterOrRain = isInWaterOrRain();
        auto light = std::max(config.getBrightness(*getHandSlot(), inWaterOrRain), config.getBrightness(getOffhandSlot(), inWaterOrRain));
        if (light > 0) {
            lightMgr.turnOn((identity_t)this, getDimension(), getBlockPos(), light, isImmersedInWater());
        } else {
            lightMgr.turnOff((identity_t)this);
        }
    }
}

TInstanceHook(void, "?postNormalTick@ItemActor@@QEAAXXZ",
    ItemActor) {
    original(this);
    if (!config.isEnabled() || !config.isItemActorEnabled() || !hasDimension()) return;
    auto item = getItemStack();
    if (!item) return;
    auto light = config.getBrightness(*item, isInWaterOrRain());
    if (light > 0) {
        lightMgr.turnOn((identity_t)this, getDimension(), getBlockPos(), light, isImmersedInWater());
    } else {
        lightMgr.turnOff((identity_t)this);
    }
}

TInstanceHook(void, "?postNormalTick@PrimedTnt@@QEAAXXZ",
              PrimedTnt) {
    original(this);
    if (!config.isEnabled() || !hasDimension())
        return;
    lightMgr.turnOn((identity_t)this, getDimension(), getBlockPos(), LightMgr::fireLightLevel, isImmersedInWater());
}

//
// Created by RedbeanW on 10/30/2022.
//

#include "Config.h"
#include "LightMgr.h"

#include "llapi/mc/Actor.hpp"
#include "llapi/mc/EnderCrystal.hpp"
#include "llapi/mc/ExperienceOrb.hpp"
#include "llapi/mc/FireworksRocketActor.hpp"
#include "llapi/mc/ItemActor.hpp"
#include "llapi/mc/LightningBolt.hpp"
#include "llapi/mc/MinecartTNT.hpp"
#include "llapi/mc/Mob.hpp"
#include "llapi/mc/PrimedTnt.hpp"

// Remove

TInstanceHook(void, "?remove@Actor@@UEAAXXZ", Actor) {
    if (lightMgr.isValid((identity_t)this)) {
        lightMgr.clear((identity_t)this);
    }
    original(this);
}

// Tick

TInstanceHook(void, "?normalTick@Actor@@UEAAXXZ", Actor) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    } {
        auto valid = lightMgr.isValid((identity_t)this);
        if (valid && isSpectator()) {
            lightMgr.turnOff((identity_t)this);
            goto END;
        }
        if (isOnFire() || isIgnited()) {
            if (!valid) {
                lightMgr.init((identity_t)this);
            }
            lightMgr.turnOn((identity_t)this, getDimension(), getBlockPos(), LightMgr::fireLightLevel, _isHeadInWater());
            goto END;
        }
    }
END:
    original(this);
}

TInstanceHook(void, "?normalTick@Mob@@UEAAXXZ", Mob) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    } {
        auto inWater = _isHeadInWater();
        auto light = std::max(config.getBrightness(*getHandSlot(), inWater), config.getBrightness(getOffhandSlot(), inWater));
        if (light <= 0) {
            lightMgr.turnOff((identity_t)this);
            goto END;
        }
        lightMgr.turnOn((identity_t)this, getDimension(), getCameraPos(), light, inWater);
    }
END:
    original(this);
}

TInstanceHook(void, "?postNormalTick@ItemActor@@QEAAXXZ", ItemActor) {
    if (!config.isEnabled() || !hasDimension() || !config.isItemActorEnabled()) {
        goto END;
    } {
        auto light = config.getBrightness(*getItemStack(), isInWater());
        auto valid = lightMgr.isValid((identity_t)this);
        if (light <= 0) {
            if (valid) {
                lightMgr.turnOff((identity_t)this);
            }
            goto END;
        }
        if (!valid) {
            lightMgr.init((identity_t)this);
        }
        lightMgr.turnOn((identity_t)this, getDimension(), getCameraPos(), light, _isHeadInWater());
        goto END;
    }
END:
    original(this);
}

TInstanceHook(void, "?postNormalTick@ExperienceOrb@@QEAAXXZ", ExperienceOrb) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getCameraPos(), 1, _isHeadInWater());
END:
    original(this);
}

TInstanceHook(void, "?normalTick@PrimedTnt@@UEAAXXZ", PrimedTnt) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getCameraPos(), LightMgr::fireLightLevel, _isHeadInWater());
END:
    original(this);
}

TInstanceHook(void, "?primeFuse@MinecartTNT@@QEAAXW4ActorDamageCause@@@Z", MinecartTNT, ActorDamageCause a2) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    lightMgr.init((identity_t)this);
END:
    original(this, a2);
}

TInstanceHook(void, "?normalTick@MinecartTNT@@UEAAXXZ", MinecartTNT) {
    if (!config.isEnabled() || !hasDimension() || !lightMgr.isValid((identity_t)this)) {
        goto END;
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getCameraPos(), LightMgr::fireLightLevel, _isHeadInWater());
END:
    original(this);
}

TInstanceHook(void, "?normalTick@LightningBolt@@UEAAXXZ", LightningBolt) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getCameraPos(), LightMgr::fireLightLevel, _isHeadInWater());
END:
    original(this);
}

TInstanceHook(void, "?postNormalTick@FireworksRocketActor@@QEAAXXZ", FireworksRocketActor) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getCameraPos(), LightMgr::fireLightLevel, _isHeadInWater());
END:
    original(this);
}

TInstanceHook(void, "?normalTick@EnderCrystal@@UEAAXXZ", EnderCrystal) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getCameraPos(), LightMgr::fireLightLevel, _isHeadInWater());
END:
    original(this);
}
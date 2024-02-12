//
// Created by RedbeanW on 10/30/2022.
//

#include "ll/api/memory/Hook.h"

#include "Config.h"
#include "LightMgr.h"
#include "mc/world/actor/Actor.h"
#include "mc/world/actor/EnderCrystal.h"
#include "mc/world/actor/Mob.h"
#include "mc/world/actor/global/LightningBolt.h"
#include "mc/world/actor/item/ExperienceOrb.h"
#include "mc/world/actor/item/FireworksRocketActor.h"
#include "mc/world/actor/item/ItemActor.h"
#include "mc/world/actor/item/MinecartTNT.h"
#include "mc/world/actor/item/PrimedTnt.h"

#include <mc/deps/puv/EquipmentSlot.h>

// Remove

LL_AUTO_TYPE_INSTANCE_HOOK(ActorRemoveHook, ll::memory::HookPriority::Normal, Actor, "?remove@Actor@@UEAAXXZ", void) {
    if (lightMgr.isValid((identity_t)this)) {
        lightMgr.clear((identity_t)this);
    }
    origin();
}

// Tick

LL_AUTO_TYPE_INSTANCE_HOOK(ActorTickHook, ll::memory::HookPriority::Normal, Actor, "?normalTick@Actor@@UEAAXXZ", void) {
    if (!config.isEnabled() || !hasDimension() || isType(ActorType::FallingBlock)) {
        goto END;
    }
    {
        auto valid = lightMgr.isValid((identity_t)this);
        if (valid && isSpectator()) {
            lightMgr.turnOff((identity_t)this);
            goto END;
        }
        if (isOnFire() || isIgnited()) {
            if (!valid) {
                lightMgr.init((identity_t)this);
            }
            lightMgr.turnOn((identity_t)this, getDimension(), getHeadPos(), LightMgr::fireLightLevel, _isHeadInWater());
            goto END;
        }
    }
END:
    origin();
}

LL_AUTO_TYPE_INSTANCE_HOOK(MobTickHook, ll::memory::HookPriority::Normal, Mob, "?normalTick@Mob@@UEAAXXZ", void) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    {
        auto valid = lightMgr.isValid((identity_t)this);
        if (isSwimming()) {
            if (valid) {
                lightMgr.turnOff((identity_t)this);
            }
            goto END;
        }
        auto inWater = _isHeadInWater();
        auto light =
            std::max(config.getBrightness(getCarriedItem(), inWater), config.getBrightness(getOffhandSlot(), inWater));
        if (light <= 0) {
            if (valid) {
                lightMgr.turnOff((identity_t)this);
            }
            goto END;
        }
        if (!valid) {
            lightMgr.init((identity_t)this);
        }
        lightMgr.turnOn((identity_t)this, getDimension(), getHeadPos(), light, inWater);
    }
END:
    origin();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    ItemActorTickHook,
    ll::memory::HookPriority::Normal,
    ItemActor,
    "?postNormalTick@ItemActor@@QEAAXXZ",
    void
) {
    if (!config.isEnabled() || !hasDimension() || !config.isItemActorEnabled()) {
        goto END;
    }
    {
        auto valid = lightMgr.isValid((identity_t)this);
        auto light = config.getBrightness(item(), isInWater());
        if (light <= 0) {
            if (valid) {
                lightMgr.turnOff((identity_t)this);
            }
            goto END;
        }
        if (!valid) {
            lightMgr.init((identity_t)this);
        }
        lightMgr.turnOn((identity_t)this, getDimension(), getHeadPos(), light, _isHeadInWater());
        goto END;
    }
END:
    origin();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    ExperienceOrbTickHook,
    ll::memory::HookPriority::Normal,
    ExperienceOrb,
    "?postNormalTick@ExperienceOrb@@QEAAXXZ",
    void
) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getHeadPos(), 1, _isHeadInWater());
END:
    origin();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    PrimedTntTickHook,
    ll::memory::HookPriority::Normal,
    PrimedTnt,
    "?normalTick@PrimedTnt@@UEAAXXZ",
    void
) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getHeadPos(), LightMgr::fireLightLevel, _isHeadInWater());
END:
    origin();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    MinecartTNTPrimeFuseHook,
    ll::memory::HookPriority::Normal,
    MinecartTNT,
    "?primeFuse@MinecartTNT@@QEAAXW4ActorDamageCause@@@Z",
    void,
    ActorDamageCause a2
) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    lightMgr.init((identity_t)this);
END:
    origin(a2);
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    MinecartTNTTickHook,
    ll::memory::HookPriority::Normal,
    MinecartTNT,
    "?normalTick@MinecartTNT@@UEAAXXZ",
    void
) {
    if (!config.isEnabled() || !hasDimension() || !lightMgr.isValid((identity_t)this)) {
        goto END;
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getHeadPos(), LightMgr::fireLightLevel, _isHeadInWater());
END:
    origin();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    LightningBoltTickHook,
    ll::memory::HookPriority::Normal,
    LightningBolt,
    "?normalTick@LightningBolt@@UEAAXXZ",
    void
) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getHeadPos(), LightMgr::fireLightLevel, _isHeadInWater());
END:
    origin();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    FireworksRocketActorTickHook,
    ll::memory::HookPriority::Normal,
    FireworksRocketActor,
    "?postNormalTick@FireworksRocketActor@@QEAAXXZ",
    void
) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getHeadPos(), LightMgr::fireLightLevel, _isHeadInWater());
END:
    origin();
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    EnderCrystalTickHook,
    ll::memory::HookPriority::Normal,
    EnderCrystal,
    "?normalTick@EnderCrystal@@UEAAXXZ",
    void
) {
    if (!config.isEnabled() || !hasDimension()) {
        goto END;
    }
    if (!lightMgr.isValid((identity_t)this)) {
        lightMgr.init((identity_t)this);
    }
    lightMgr.turnOn((identity_t)this, getDimension(), getHeadPos(), LightMgr::fireLightLevel, _isHeadInWater());
END:
    origin();
}
#include "Main.h"

#include "Config.h"
#include "ll/api/event/server/ServerStartedEvent.h"
#include "mc/world/actor/player/Player.h"

#include <filesystem>
#include <ll/api/plugin/NativePlugin.h>
#include <memory>

#define DATA_PATH "plugins/DynamicLight/"

namespace dynamic_light {

DyncmicLight::DyncmicLight() = default;

DyncmicLight& DyncmicLight::getInstance() {
    static DyncmicLight instance;
    return instance;
}

ll::plugin::NativePlugin& DyncmicLight::getSelf() const { return *mSelf; }

bool DyncmicLight::load(ll::plugin::NativePlugin& self) {
    mSelf = std::addressof(self);

    if (!std::filesystem::exists(DATA_PATH)) {
        getSelf().getLogger().warn("Could not find the directory for the configuration file, creating...");
        std::filesystem::create_directory(DATA_PATH);
    }

    config.loadFromFile(DATA_PATH "config.json");

    return true;
}

bool DyncmicLight::enable() { return true; }

bool DyncmicLight::disable() { return true; }

extern "C" {
_declspec(dllexport) bool ll_plugin_load(ll::plugin::NativePlugin& self) {
    return DyncmicLight::getInstance().load(self);
}

_declspec(dllexport) bool ll_plugin_enable(ll::plugin::NativePlugin&) { return DyncmicLight::getInstance().enable(); }

_declspec(dllexport) bool ll_plugin_disable(ll::plugin::NativePlugin&) { return DyncmicLight::getInstance().disable(); }

/// @warning Unloading the plugin may cause a crash if the plugin has not released all of its
/// resources. If you are unsure, keep this function commented out.
// _declspec(dllexport) bool ll_plugin_unload(ll::plugin::NativePlugin&) {
//     return DyncmicLight::getInstance().unload();
// }
}

} // namespace dynamic_light

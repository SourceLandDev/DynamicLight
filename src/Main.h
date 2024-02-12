#pragma once

#include <ll/api/plugin/NativePlugin.h>

namespace dynamic_light {

class DyncmicLight {
    DyncmicLight();

public:
    DyncmicLight(DyncmicLight&&)                 = delete;
    DyncmicLight(const DyncmicLight&)            = delete;
    DyncmicLight& operator=(DyncmicLight&&)      = delete;
    DyncmicLight& operator=(const DyncmicLight&) = delete;

    static DyncmicLight& getInstance();

    [[nodiscard]] ll::plugin::NativePlugin& getSelf() const;

    /// @return True if the plugin is loaded successfully.
    bool load(ll::plugin::NativePlugin&);

    /// @return True if the plugin is enabled successfully.
    bool enable();

    /// @return True if the plugin is disabled successfully.
    bool disable();

private:
    ll::plugin::NativePlugin* mSelf{};
};

} // namespace dynamic_light

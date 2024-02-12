//
// Created by RedbeanW on 10/30/2022.
//

#pragma once

#include "mc/world/level/BlockPos.h"
#include "mc/world/level/dimension/Dimension.h"

using identity_t = int64_t;

class LightMgr {
public:

    explicit LightMgr() noexcept;

    void init(identity_t);

    [[nodiscard]] bool isValid(identity_t);

    void turnOff(identity_t);

    void turnOn(identity_t, Dimension&, BlockPos, unsigned int = fireLightLevel, bool = false);

    void clear(identity_t);

    static unsigned int fireLightLevel;

private:

    struct LightInfo {
        bool mLighting = false;
        unsigned int mLevel = 0;
        BlockPos mPos = BlockPos::ZERO;
        int mDimId = -1;
        unsigned int mType = -1;
    };

    std::unordered_map<identity_t, LightInfo> mRecordedInfo;

    void _sendPacket(Dimension* dim, const BlockPos& pos, class Packet const& pkt);


};

extern LightMgr lightMgr;
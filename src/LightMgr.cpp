//
// Created by RedbeanW on 10/30/2022.
//

#include "LightMgr.h"
#include "Config.h"

#include "llapi/ScheduleAPI.h"

#include "llapi/mc/Material.hpp"
#include "llapi/mc/StaticVanillaBlocks.hpp"
#include "llapi/mc/Level.hpp"
#include "llapi/mc/UpdateBlockPacket.hpp"
#include "llapi/mc/Dimension.hpp"
#include "llapi/mc/VanillaBlockTypeIds.hpp"
#include "llapi/mc/BlockTypeRegistry.hpp"
#include "llapi/mc/Brightness.hpp"

LightMgr lightMgr;
unsigned int LightMgr::fireLightLevel;

LightMgr::LightMgr() noexcept {
    Event::ServerStartedEvent::subscribe([](Event::ServerStartedEvent ev) {
        fireLightLevel = StaticVanillaBlocks::mFire->getLightEmission().value;
        return true;
    });
}

bool LightMgr::isValid(identity_t id) {
    return mRecordedInfo.contains(id);
}

void LightMgr::init(identity_t id) {
    LightInfo info;
    mRecordedInfo[id] = info;
}

bool LightMgr::isTurningOn(identity_t id) {
    return isValid(id) && mRecordedInfo[id].mLighting;
}

void LightMgr::turnOff(identity_t id) {
    if (!isTurningOn(id))
        return;
    mRecordedInfo[id].mLighting = false;
    auto pos = mRecordedInfo[id].mPos;
    auto dim = Global<Level>->getDimension(mRecordedInfo[id].mDimId).get();
    if (!dim) return;
    auto& block = dim->getBlockSourceFromMainChunkSource().getBlock(pos);
    if (block == *StaticVanillaBlocks::mWater) {
        UpdateBlockPacket updateBlock(pos, 0, block.getRuntimeId(), 3);
        _sendPacket(dim, pos, updateBlock);
    } else {
        UpdateBlockPacket updateBlock(pos, 1, block.getRuntimeId(), 3);
        _sendPacket(dim, pos, updateBlock);
    }
}

void LightMgr::turnOn(identity_t id, Dimension& dim, BlockPos bp, unsigned int lightLv, bool underWater) {
    if (underWater && !config.isUnderWaterEnabled()) return;
    if (!isValid(id)) init(id);
    auto& rec = mRecordedInfo[id];
    bool isOpened = isTurningOn(id);
    bp.y = bp.y + 1;
    bool isSamePos = bp == rec.mPos;
    bool isSameLight = lightLv == rec.mLevel;
    if (isOpened && isSamePos && isSameLight) return;

    auto& region = dim.getBlockSourceFromMainChunkSource();
    auto& blk = region.getBlock(bp);
    if (underWater) {
        if (blk != *StaticVanillaBlocks::mWater) return;
        UpdateBlockPacket updateBlock(bp, 0, BlockTypeRegistry::lookupByName(VanillaBlockTypeIds::LightBlock, lightLv, true)->getRuntimeId(), 3);
        _sendPacket(&dim, bp, updateBlock);
    } else {
        if (!blk.isAir()) return;
        UpdateBlockPacket updateBlock(bp, 1, BlockTypeRegistry::lookupByName(VanillaBlockTypeIds::LightBlock, lightLv, true)->getRuntimeId(), 3);
        _sendPacket(&dim, bp, updateBlock);
    }

    if (!isSamePos && (isOpened || !isSameLight)) turnOff(id);

    rec.mLighting = true;
    rec.mDimId = dim.getDimensionId();
    rec.mPos = bp;
    rec.mLevel = lightLv;

}

void LightMgr::clear(identity_t id) {
    turnOff(id);
    mRecordedInfo.erase(id);
}

void LightMgr::_sendPacket(Dimension* dim, const BlockPos &pos, const Packet& pkt) {
    if (ll::isServerStopping()) return;
    dim->sendPacketForPosition(pos, pkt, nullptr);
}
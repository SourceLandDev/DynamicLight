//
// Created by RedbeanW on 10/30/2022.
//

#include "LightMgr.h"
#include "Config.h"

#include "llapi/EventAPI.h"

#include "llapi/mc/BlockSource.hpp"
#include "llapi/mc/BlockTypeRegistry.hpp"
#include "llapi/mc/LiquidBlock.hpp"
#include "llapi/mc/Brightness.hpp"
#include "llapi/mc/HashedString.hpp"
#include "llapi/mc/IConstBlockSource.hpp"
#include "llapi/mc/Level.hpp"
#include "llapi/mc/StaticVanillaBlocks.hpp"
#include "llapi/mc/UpdateBlockPacket.hpp"
#include "llapi/mc/VanillaBlockTypeIds.hpp"
#include "llapi/mc/Material.hpp"

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

void LightMgr::turnOff(identity_t id) {
    auto& rec = mRecordedInfo[id];
    if (!rec.mLighting) {
        return;
    }
    auto dim = Global<Level>->getDimension(rec.mDimId).get();
    if (!dim) {
        return;
    }
    auto& region = dim->getBlockSourceFromMainChunkSource();
    auto& blk = region.getBlock(rec.mPos);
    UpdateBlockPacket updateBlock(rec.mPos, rec.mType, blk.getRuntimeId(), 0);
    _sendPacket(dim, rec.mPos, updateBlock);
    rec.mLighting = false;
}

void LightMgr::turnOn(identity_t id, Dimension& dim, BlockPos bp, unsigned int lightLv, bool underWater) {
    auto& rec = mRecordedInfo[id];
    bool isOpened = rec.mLighting;
    bool isSamePos = bp == rec.mPos;
    bool isSameLight = lightLv == rec.mLevel;
    if (isOpened && isSamePos && isSameLight) {
        return;
    }
    auto& region = dim.getBlockSourceFromMainChunkSource();
    auto& blk = region.getBlock(bp);
    if (blk.getLightEmission().value >= lightLv ||
        (underWater && blk != *StaticVanillaBlocks::mWater) ||
        LiquidBlock::getDepth((IConstBlockSource&)region, bp, blk.getMaterial()) > 0) {
        turnOff(id);
        return;
    }
    UpdateBlockPacket updateBlock(bp, !underWater, BlockTypeRegistry::lookupByName(VanillaBlockTypeIds::LightBlock, lightLv, true)->getRuntimeId(), 0);
    _sendPacket(&dim, bp, updateBlock);

    if (!isSamePos && (isOpened || !isSameLight)) {
        turnOff(id);
    }

    rec.mType = !underWater;
    rec.mLighting = true;
    rec.mDimId = dim.getDimensionId();
    rec.mPos = bp;
    rec.mLevel = lightLv;
}

void LightMgr::clear(identity_t id) {
    turnOff(id);
    mRecordedInfo.erase(id);
}

void LightMgr::_sendPacket(Dimension* dim, const BlockPos& pos, const Packet& pkt) {
    if (ll::isServerStopping()) {
        return;
    }
    dim->sendPacketForPosition(pos, pkt, nullptr);
}
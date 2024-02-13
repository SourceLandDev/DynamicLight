//
// Created by RedbeanW on 10/30/2022.
//

#include "LightMgr.h"

#include "Config.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/server/ServerStartedEvent.h"
#include "ll/api/service/Bedrock.h"
#include "ll/api/service/ServerInfo.h"
#include "mc/deps/core/string/HashedString.h"
#include "mc/network/packet/UpdateBlockPacket.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/IConstBlockSource.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/LiquidBlock.h"
#include "mc/world/level/block/registry/BlockTypeRegistry.h"
#include "mc/world/level/block/utils/VanillaBlockTypeIds.h"

LightMgr     lightMgr;
unsigned int LightMgr::fireLightLevel;

LightMgr::LightMgr() noexcept {
    ll::event::EventBus::getInstance().emplaceListener<ll::event::ServerStartedEvent>(
        [](ll::event::ServerStartedEvent ev) {
            fireLightLevel =
                BlockTypeRegistry::lookupByName(VanillaBlockTypeIds::Fire, 0, true)->getLightEmission().value;
            return true;
        }
    );
}

bool LightMgr::isValid(identity_t id) { return mRecordedInfo.contains(id); }

void LightMgr::init(identity_t id) {
    LightInfo info;
    mRecordedInfo[id] = info;
}

void LightMgr::turnOff(identity_t id) {
    auto& rec = mRecordedInfo[id];
    if (!rec.mLighting) {
        return;
    }
    auto dim = ll::service::getLevel()->getDimension(rec.mDimId).get();
    if (!dim) {
        return;
    }
    auto&             region = dim->getBlockSourceFromMainChunkSource();
    auto&             blk    = region.getBlock(rec.mPos);
    UpdateBlockPacket updateBlock(rec.mPos, rec.mType, blk.getRuntimeId(), 0);
    _sendPacket(dim, rec.mPos, updateBlock);
    rec.mLighting = false;
}

void LightMgr::turnOn(identity_t id, Dimension& dim, BlockPos bp, unsigned int lightLv, bool underWater) {
    auto& rec         = mRecordedInfo[id];
    bool  isOpened    = rec.mLighting;
    bool  isSamePos   = bp == rec.mPos;
    bool  isSameLight = lightLv == rec.mLevel;
    if (isOpened && isSamePos && isSameLight) {
        return;
    }
    auto& region = dim.getBlockSourceFromMainChunkSource();
    auto& blk    = region.getBlock(bp);
    if (blk.getLightEmission().value >= lightLv || (underWater && blk.getTypeName() != VanillaBlockTypeIds::Water)
        || LiquidBlock::getDepth((IConstBlockSource&)region, bp, blk.getMaterial()) > 0) {
        turnOff(id);
        return;
    }
    UpdateBlockPacket updateBlock(
        bp,
        !underWater,
        BlockTypeRegistry::lookupByName(VanillaBlockTypeIds::LightBlock, lightLv, true)->getRuntimeId(),
        0
    );
    _sendPacket(&dim, bp, updateBlock);

    if (!isSamePos && (isOpened || !isSameLight)) {
        turnOff(id);
    }

    rec.mType     = !underWater;
    rec.mLighting = true;
    rec.mDimId    = dim.getDimensionId();
    rec.mPos      = bp;
    rec.mLevel    = lightLv;
}

void LightMgr::clear(identity_t id) {
    turnOff(id);
    mRecordedInfo.erase(id);
}

void LightMgr::_sendPacket(Dimension* dim, const BlockPos& pos, const Packet& pkt) {
    if (ll::getServerStatus() == ll::ServerStatus::Stopping) {
        return;
    }
    dim->sendPacketForPosition(pos, pkt, nullptr);
}
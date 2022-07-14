#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>
#include <MC/Level.hpp>
#include <MC/Actor.hpp>
#include <MC/Player.hpp>
#include <MC/MovePlayerPacket.hpp>
#include <MC/MoveActorAbsolutePacket.hpp>
#include <MC/ServerPlayer.hpp>
#include <MC/Dimension.hpp>
#include <MC/Level.hpp>
#include "Version.h"
#include <LLAPI.h>
#include <ServerAPI.h>
#include <HookAPI.h>

Logger logger(PLUGIN_NAME);

void PluginInit() {
    logger.info("Plugin inited");
}

TInstanceHook(void, "?handle@ServerNetworkHandler@@UEAAXAEBVNetworkIdentifier@@AEBVMovePlayerPacket@@@Z",
    ServerNetworkHandler, NetworkIdentifier const& netId, MovePlayerPacket const& oldPkt) {

    ServerPlayer *player = this->getServerPlayer(netId);
    if (!player) return;

    MoveActorAbsolutePacket newPkt;
    newPkt.runtimeId = oldPkt.runtimeId;
    newPkt.flags = (oldPkt.onGround ? 1 : 0);
    if (oldPkt.mode == Player::PositionMode::TELEPORT) {
        newPkt.flags |= 0x2;
    }

    newPkt.position = oldPkt.position;

    float bYaw = oldPkt.yaw * 0.71;
    float bPitch = oldPkt.pitch * 0.71;

    uint8_t convertYaw = (uint8_t)bYaw;
    uint8_t convertPitch = (uint8_t)bPitch;

    newPkt.pitch = convertPitch;
    newPkt.yaw = convertYaw;
    newPkt.headYaw = convertYaw;

    player->setPos(oldPkt.position);
    player->setRot(Vec2(oldPkt.yaw, oldPkt.pitch));
    player->getLevel().sendPacketForAllPlayer(newPkt);

    return;
}
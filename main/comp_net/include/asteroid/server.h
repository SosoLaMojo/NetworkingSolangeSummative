#pragma once
#include <array>
#include "asteroid/packet_type.h"
#include "asteroid/game_manager.h"

namespace neko::net
{
class Server : public pongsoso::PacketSenderInterface, public SystemInterface
{
protected:
    virtual void SpawnNewPlayer(ClientId clientId, PlayerNumber playerNumber) = 0;
    virtual void ReceivePacket(std::unique_ptr<pongsoso::Packet> packet);

    //Server game manager
    pongsoso::GameManager gameManager_;
    PlayerNumber lastPlayerNumber_ = 0;
    std::array<ClientId, pongsoso::maxPlayerNmb> clientMap_{};

};
}

#pragma once
#include "game_manager.h"
#include "packet_type.h"
#include "graphics/graphics.h"

namespace neko::net
{
class Client : public RenderProgram, public DrawImGuiInterface, public pongsoso::PacketSenderInterface
{
public:
    Client() : gameManager_(*this)
    {

    }
    void SetWindowSize(Vec2u windowSize)
    {
        gameManager_.SetWindowSize(windowSize);
    }
    virtual void ReceivePacket(const pongsoso::Packet *packet);
protected:

    pongsoso::ClientGameManager gameManager_;
    ClientId clientId_ = 0;
};
}

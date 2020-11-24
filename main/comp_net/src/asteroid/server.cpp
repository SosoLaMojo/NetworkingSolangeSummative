#include "asteroid/server.h"
#include "engine/log.h"
#include "engine/conversion.h"
#include "asteroid/game.h"

namespace neko::net
{


void Server::ReceivePacket(std::unique_ptr<pongsoso::Packet> packet)
{
    const auto packetType = static_cast<pongsoso::PacketType>(packet->packetType);
    switch (packetType)
    {
    case pongsoso::PacketType::JOIN:
    {
        const auto* joinPacket = static_cast<const pongsoso::JoinPacket*>(packet.get());
        const auto clientId = ConvertFromBinary<ClientId>(joinPacket->clientId);
        if (std::find(clientMap_.begin(), clientMap_.end(), clientId) != clientMap_.end())
        {
            //Player joined twice!
            return;
        }
        logDebug("Managing Received Packet Join from: " + std::to_string(clientId));
        clientMap_[lastPlayerNumber_] = clientId;
        SpawnNewPlayer(clientId, lastPlayerNumber_);

        lastPlayerNumber_++;

        if (lastPlayerNumber_ == pongsoso::maxPlayerNmb)
        {
            auto spawnBallPacket = std::make_unique<pongsoso::SpawnBallPacket>();
            spawnBallPacket->packetType = pongsoso::PacketType::SPAWN_BALL;
            auto startGamePacket = std::make_unique<pongsoso::StartGamePacket>(); // tout le if, commence la partie dans 3 sec
            startGamePacket->packetType = pongsoso::PacketType::START_GAME;
            using namespace std::chrono;
            unsigned long ms = (duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()
                ) + milliseconds(3000)).count();
            startGamePacket->startTime = ConvertToBinary(ms);
            SendReliablePacket(std::move(startGamePacket));
            Vec2f dir = Vec2f(RandomRange(-1.0f, 1.0f), RandomRange(-1.0f, 1.0f)).Normalized() * pongsoso::ballSpeed;// -> direction aléatoire et vitesse de la balle
            spawnBallPacket->pos = ConvertToBinary(Vec2f::zero);
            spawnBallPacket->velocity = ConvertToBinary(dir);
            SendReliablePacket(std::move(spawnBallPacket));// rajout envoi paquet SpawnBall
            gameManager_.SpawnBall(Vec2f::zero, dir);
        }

        break;
    }
    case pongsoso::PacketType::INPUT:
    {
        //Manage internal state
        const auto* playerInputPacket = static_cast<const pongsoso::PlayerInputPacket*>(packet.get());
        const auto playerNumber = playerInputPacket->playerNumber;
        const auto inputFrame = ConvertFromBinary<net::Frame>(playerInputPacket->currentFrame);

        for (std::uint32_t i = 0; i < playerInputPacket->inputs.size(); i++)
        {
            gameManager_.SetPlayerInput(playerNumber,
                playerInputPacket->inputs[i],
                inputFrame - i);
            if (inputFrame - i == 0)
            {
                break;
            }
        }

        SendUnreliablePacket(std::move(packet));

        //Validate new frame if needed
        std::uint32_t lastReceiveFrame = gameManager_.GetRollbackManager().GetLastReceivedFrame(0);
        for (PlayerNumber i = 1; i < pongsoso::maxPlayerNmb; i++)
        {
            const auto playerLastFrame = gameManager_.GetRollbackManager().GetLastReceivedFrame(i);
            if (playerLastFrame < lastReceiveFrame)
            {
                lastReceiveFrame = playerLastFrame;
            }
        }
        if (lastReceiveFrame > gameManager_.GetLastValidateFrame())
        {
            //Validate frame
            gameManager_.Validate(lastReceiveFrame);

            auto validatePacket = std::make_unique<pongsoso::ValidateFramePacket>();
            validatePacket->newValidateFrame = ConvertToBinary(lastReceiveFrame);

            //copy physics state
            for (PlayerNumber i = 0; i < pongsoso::maxPlayerNmb; i++)
            {
                auto physicsState = gameManager_.GetRollbackManager().GetValidatePhysicsState(i);
                const auto* statePtr = reinterpret_cast<const std::uint8_t*>(&physicsState);
                for (size_t j = 0; j < sizeof(pongsoso::PhysicsState); j++)
                {
                    validatePacket->physicsState[i * sizeof(pongsoso::PhysicsState) + j] = statePtr[j];
                }
            }
            SendUnreliablePacket(std::move(validatePacket));
            const auto winner = gameManager_.CheckWinner();
            if (winner != INVALID_PLAYER)
            {
                logDebug(fmt::format("Server declares P{} a winner", winner + 1));
                auto winGamePacket = std::make_unique<pongsoso::WinGamePacket>();
                winGamePacket->winner = winner;
                SendReliablePacket(std::move(winGamePacket));
                gameManager_.WinGame(winner);
            }
        }

        break;
    }
    default: break;
    }
}
}

#include "asteroid/client.h"

#include "engine/conversion.h"

namespace neko::net
{
void Client::ReceivePacket(const pongsoso::Packet* packet)
{
    const auto packetType = packet->packetType;
    switch (packetType)
    {
    case pongsoso::PacketType::SPAWN_PLAYER:
    {
        const auto* spawnPlayerPacket = static_cast<const pongsoso::SpawnPlayerPacket*>(packet);
        const auto clientId = ConvertFromBinary<ClientId>(spawnPlayerPacket->clientId);

        const PlayerNumber playerNumber = spawnPlayerPacket->playerNumber;
        if (clientId == clientId_)
        {
            gameManager_.SetClientPlayer(playerNumber);
        }

        const auto pos = ConvertFromBinary<Vec2f>(spawnPlayerPacket->pos);
        const auto rotation = ConvertFromBinary<degree_t>(spawnPlayerPacket->angle);

        gameManager_.SpawnPlayer(playerNumber, pos, rotation);
        break;
    }
    case pongsoso::PacketType::START_GAME:
    {
        const auto* startGamePacket = static_cast<const pongsoso::StartGamePacket*>(packet);
        unsigned long startingTime = ConvertFromBinary<unsigned long>(startGamePacket->startTime);
        gameManager_.StartGame(startingTime);
        break;
    }
    case pongsoso::PacketType::INPUT:
    {
        const auto* playerInputPacket = static_cast<const pongsoso::PlayerInputPacket*>(packet);
        const auto playerNumber = playerInputPacket->playerNumber;
        const auto inputFrame = ConvertFromBinary<Frame>(playerInputPacket->currentFrame);

        if (playerNumber == gameManager_.GetPlayerNumber())
        {
            //Verify the inputs coming back from the server
            const auto& inputs = gameManager_.GetRollbackManager().GetInputs(playerNumber);
            const auto currentFrame = gameManager_.GetRollbackManager().GetCurrentFrame();
            for (size_t i = 0; i < playerInputPacket->inputs.size(); i++)
            {
                const auto index = currentFrame - inputFrame + i;
                if (index > inputs.size())
                {
                    break;
                }
                if (inputs[index] != playerInputPacket->inputs[i])
                {
                    neko_assert(false, "Inputs coming back from server are not coherent!!!");
                }
                if (inputFrame - i == 0)
                {
                    break;
                }
            }
            break;
        }

        //discard delayed input packet
        if (inputFrame < gameManager_.GetRollbackManager().GetLastReceivedFrame(playerNumber))
        {
            break;
        }
        for (Frame i = 0; i < playerInputPacket->inputs.size(); i++)
        {
            gameManager_.SetPlayerInput(playerNumber,
                playerInputPacket->inputs[i],
                inputFrame - i);

            if (inputFrame - i == 0)
            {
                break;
            }
        }
        break;
    }
    case pongsoso::PacketType::VALIDATE_STATE:
    {
        const auto* validateFramePacket = static_cast<const pongsoso::ValidateFramePacket*>(packet);
        const auto newValidateFrame = ConvertFromBinary<Frame>(validateFramePacket->newValidateFrame);
        std::array<pongsoso::PhysicsState, pongsoso::maxPlayerNmb> physicsStates{};
        for (size_t i = 0; i < validateFramePacket->physicsState.size(); i++)
        {
            auto* statePtr = reinterpret_cast<std::uint8_t*>(physicsStates.data());
            statePtr[i] = validateFramePacket->physicsState[i];
        }
        gameManager_.ConfirmValidateFrame(newValidateFrame, physicsStates);
        //logDebug("Client received validate frame " + std::to_string(newValidateFrame));
        break;
    }
    case pongsoso::PacketType::WIN_GAME:
    {
        const auto* winGamePacket = static_cast<const pongsoso::WinGamePacket*>(packet);
        gameManager_.WinGame(winGamePacket->winner);
        break;
    }
    case pongsoso::PacketType::SPAWN_BULLET: break;
    {
	    
    }
    case pongsoso::PacketType::SPAWN_BALL:
    {
        const auto* spawnBallPacket = static_cast<const pongsoso::SpawnBallPacket*>(packet);

        const auto pos = ConvertFromBinary<Vec2f>(spawnBallPacket->pos);
        const auto velocity = ConvertFromBinary<Vec2f>(spawnBallPacket->velocity);

        gameManager_.SpawnBall(pos, velocity);
        break;
    }
    default:;
    }

}
}

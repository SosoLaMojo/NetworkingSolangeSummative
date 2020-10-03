#include "asteroid_simulation/simulation_server.h"

#include "asteroid_simulation/simulation_client.h"
#include "asteroid/game_manager.h"
#include "asteroid/packet_type.h"
#include "comp_net/packet.h"

namespace neko::net
{
SimulationServer::SimulationServer(std::array<std::unique_ptr<SimulationClient>, 2>& clients) : clients_(clients)
{
}

void SimulationServer::Init()
{
	gameManager_.Init();
	
}

void SimulationServer::Update(seconds dt)
{

	auto packetIt = receivedPackets_.begin();
	while (packetIt != receivedPackets_.end())
	{
		packetIt->currentTime -= dt.count();
		if (packetIt->currentTime <= 0.0f)
		{
            ProcessReceivePacket(std::move(packetIt->packet));
			
			packetIt = receivedPackets_.erase(packetIt);
		}
		else
		{
			++packetIt;
		}

	}

	packetIt = sentPackets_.begin();
	while (packetIt != sentPackets_.end())
	{
		packetIt->currentTime -= dt.count();
		if (packetIt->currentTime <= 0.0f)
		{
			for (auto& client: clients_)
			{
				client->ReceivePacket(packetIt->packet.get());
			}
            packetIt->packet = nullptr;
			packetIt = sentPackets_.erase(packetIt);
		}
		else
		{
			++packetIt;
		}
	}
}

void SimulationServer::Destroy()
{
}

void SimulationServer::SendPacket(std::unique_ptr<asteroid::Packet> packet)
{
    sentPackets_.push_back({ avgDelay, std::move(packet) });
}

void SimulationServer::ReceivePacket(std::unique_ptr<asteroid::Packet> packet)
{
    receivedPackets_.push_back({ avgDelay, std::move(packet) });
}

void SimulationServer::ProcessReceivePacket(std::unique_ptr<asteroid::Packet> packet)
{
	const auto packetType = static_cast<asteroid::PacketType>(packet->packetType);
	switch (packetType)
	{
	case asteroid::PacketType::JOIN:
	{
		const auto* joinPacket = static_cast<asteroid::JoinPacket*>(packet.get());
		ClientId clientId = 0;
		auto* clientIdPtr = reinterpret_cast<std::uint8_t*>(&clientId);
		for(size_t i = 0; i < sizeof(ClientId);i++)
        {
		    clientIdPtr[i] = joinPacket->clientId[i];
        }
		if(std::find(clientMap_.begin(), clientMap_.end(), clientId) != clientMap_.end())
        {
		    //Player joined twice!
		    return;
        }
		logDebug("Managing Received Packet Join from: "+std::to_string(clientId));
		auto spawnPlayer = std::make_unique<asteroid::SpawnPlayerPacket>();
		spawnPlayer->packetType = asteroid::PacketType::SPAWN_PLAYER;
		spawnPlayer->clientId = joinPacket->clientId;
		spawnPlayer->playerNumber = lastPlayerNumber_;

		const auto pos = spawnPositions_[lastPlayerNumber_]*3.0f;
		const auto* const posPtr = reinterpret_cast<const std::uint8_t*>(&pos[0]);
		for (size_t i = 0; i < sizeof(Vec2f);i++)
		{
			spawnPlayer->pos[i] = posPtr[i];
		}
		const auto rotation = spawnRotations_[lastPlayerNumber_];
		const auto* const rotationPtr = reinterpret_cast<const std::uint8_t*>(&rotation);
        for (size_t i = 0; i < sizeof(degree_t);i++)
        {
            spawnPlayer->angle[i] = rotationPtr[i];
        }
		clientMap_[lastPlayerNumber_] = clientId;
		gameManager_.SpawnPlayer(lastPlayerNumber_, pos, rotation);
		lastPlayerNumber_++;
		SendPacket(std::move(spawnPlayer));
		
		if(lastPlayerNumber_ == asteroid::maxPlayerNmb)
        {
		    auto startGamePacket = std::make_unique<asteroid::StartGamePacket>();
            startGamePacket->packetType = asteroid::PacketType::START_GAME;
            using namespace std::chrono;
            auto ms = (duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
            )+milliseconds(3000)).count();
            const auto* msPtr = reinterpret_cast<std::uint8_t*>(&ms);
            for(size_t i = 0; i < sizeof(ms); i++)
            {
                startGamePacket->startTime[i] = msPtr[i];
            }
            SendPacket(std::move(startGamePacket));
        }

		break;
	}
	case asteroid::PacketType::SPAWN_PLAYER: break;
	case asteroid::PacketType::INPUT:
	{
		//Manage internal state
		const auto* playerInputPacket = static_cast<const asteroid::PlayerInputPacket*>(packet.get());
		const auto playerNumber = playerInputPacket->playerNumber;
		std::uint32_t inputFrame = 0;
		auto* inputPtr = reinterpret_cast<std::uint8_t*>(&inputFrame);
		for (size_t i = 0; i < sizeof(std::uint32_t); i++)
		{
			inputPtr[i] = playerInputPacket->currentFrame[i];
		}
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
		//Send input packet to everyone
		SendPacket(std::move(packet));

		//Validate new frame if needed
		std::uint32_t lastReceiveFrame = gameManager_.GetRollbackManager().GetLastReceivedFrame(0);
		for(PlayerNumber i = 1; i < asteroid::maxPlayerNmb; i++)
		{
			const auto playerLastFrame = gameManager_.GetRollbackManager().GetLastReceivedFrame(i);
			if(playerLastFrame < lastReceiveFrame)
			{
				lastReceiveFrame = playerLastFrame;
			}
		}
		if(lastReceiveFrame > gameManager_.GetLastValidateFrame())
		{
			//Validate frame
            gameManager_.Validate(lastReceiveFrame);

			auto validatePacket = std::make_unique<asteroid::ValidateFramePacket>();
			validatePacket->packetType = asteroid::PacketType::VALIDATE_STATE;
			const auto* framePtr = reinterpret_cast<std::uint8_t*>(&lastReceiveFrame);
			for(size_t i = 0; i < sizeof(std::uint32_t); i++)
			{
				validatePacket->newValidateFrame[i] = framePtr[i];
			}
			//copy physics state
			for (PlayerNumber i = 0; i < asteroid::maxPlayerNmb; i++)
			{
				auto physicsState = gameManager_.GetRollbackManager().GetValidatePhysicsState(i);
				const auto* statePtr = reinterpret_cast<const std::uint8_t*>(&physicsState);
				for(size_t j = 0; j < sizeof(asteroid::PhysicsState); j++)
				{
					validatePacket->physicsState[i * sizeof(asteroid::PhysicsState) + j] = statePtr[j];
				}
			}
			SendPacket(std::move(validatePacket));
		}
		
		break;
	}

	case asteroid::PacketType::SPAWN_BULLET: break;
	case asteroid::PacketType::VALIDATE_STATE: break;
	case asteroid::PacketType::START_GAME: break;
	case asteroid::PacketType::NONE: break;
	default:;
	}
}
}
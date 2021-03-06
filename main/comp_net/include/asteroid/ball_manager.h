/*
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#pragma once
#include "game.h"
#include "engine/component.h"
#include "comp_net/type.h"
#include "asteroid/physics_manager.h"
#include "asteroid/player_character.h"

namespace neko::pongsoso
{
struct Ball
{
    float remainingTime = 0.0f;
    
    net::PlayerNumber playerNumber = net::INVALID_PLAYER;
};
class GameManager;
class BallManager : public ComponentManager<Ball, static_cast<EntityMask>(ComponentType::BALL)>
{
public:
    explicit BallManager(EntityManager& entityManager, GameManager& gameManager, PhysicsManager& physicsManager, PlayerCharacterManager& playerCharacterManager);
    BallManager& operator= (const BallManager&);
    void FixedUpdate(seconds dt);
private:
    std::reference_wrapper<GameManager> gameManager_;
    std::reference_wrapper<PhysicsManager> physicsManager_;
    std::reference_wrapper<PlayerCharacterManager> playerCharacterManager_;
};
}
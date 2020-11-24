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

#include "asteroid/ball_manager.h"
#include "asteroid/game.h"
#include "asteroid/game_manager.h"

namespace neko::pongsoso
{
BallManager::BallManager(EntityManager& entityManager, GameManager& gameManager, PhysicsManager& physicsManager, PlayerCharacterManager& playerCharacterManager) :
    ComponentManager(entityManager), gameManager_(gameManager), physicsManager_(physicsManager), playerCharacterManager_(playerCharacterManager)
{
}

BallManager& BallManager::operator=(const BallManager& ballManager)
{
    components_ = ballManager.components_;
    //We do NOT copy the physics manager
    return *this;
}

void BallManager::FixedUpdate(seconds dt)
{
    for(Entity ballEntity = 0; ballEntity < entityManager_.get().GetEntitiesSize(); ballEntity++)
    {
        if(entityManager_.get().HasComponent(ballEntity, EntityMask(ComponentType::BALL)))
        {
            auto& ball = components_[ballEntity];
            auto ballBody = physicsManager_.get().GetBody(ballEntity);
        	// Limitation de hauteur pour le rebond de la balle (wall up and down)
            if ((ballBody.position.y > ballMaxHeight && ballBody.velocity.y > 0) || (ballBody.position.y < ballMinHeight && ballBody.velocity.y < 0))
            {
                ballBody.velocity = Vec2f(ballBody.velocity.x, -ballBody.velocity.y);
                physicsManager_.get().SetBody(ballEntity, ballBody);
            }
	        if (ballBody.position.x > ballPoint)
	        {
				auto playerCharacter = playerCharacterManager_.get().GetComponent(gameManager_.get().GetEntityFromPlayerNumber(0));
	            playerCharacter.health--;
                playerCharacterManager_.get().SetComponent(gameManager_.get().GetEntityFromPlayerNumber(0), playerCharacter);
                ballBody.position = Vec2f(0, 0);
                ballBody.velocity = Vec2f::zero - ballBody.velocity.Normalized() * ballSpeed;
                physicsManager_.get().SetBody(ballEntity, ballBody);
	        }
            if (ballBody.position.x < -ballPoint)
            {
                auto playerCharacter = playerCharacterManager_.get().GetComponent(gameManager_.get().GetEntityFromPlayerNumber(1));
                playerCharacter.health--;
                playerCharacterManager_.get().SetComponent(gameManager_.get().GetEntityFromPlayerNumber(1), playerCharacter);
                ballBody.position = Vec2f (0, 0);
                ballBody.velocity = Vec2f::zero - ballBody.velocity.Normalized() * ballSpeed;
                physicsManager_.get().SetBody(ballEntity, ballBody);
            }
        }
    }
}
}
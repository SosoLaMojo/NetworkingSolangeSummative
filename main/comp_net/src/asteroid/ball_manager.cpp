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

namespace neko::pongsoso
{
BallManager::BallManager(EntityManager& entityManager, GameManager& gameManager, PhysicsManager& physicsManager) :
    ComponentManager(entityManager), gameManager_(gameManager), physicsManager_(physicsManager)
{
}

void BallManager::FixedUpdate(seconds dt)
{
    for(Entity ballEntity = 0; ballEntity < entityManager_.get().GetEntitiesSize(); ballEntity++)
    {
        if(entityManager_.get().HasComponent(ballEntity, EntityMask(ComponentType::BALL)))
        {
            auto& ball = components_[ballEntity];
            auto ballBody = physicsManager_.get().GetBody(ballEntity);
            if ((ballBody.position.y > ball.ballMaxHeight && ballBody.velocity.y > 0) || (ballBody.position.y < ball.ballMinHeight && ballBody.velocity.y < 0))
            {
                ballBody.velocity = Vec2f(ballBody.velocity.x, -ballBody.velocity.y);
                physicsManager_.get().SetBody(ballEntity, ballBody);
            }
        }

        
    }

    
}
}
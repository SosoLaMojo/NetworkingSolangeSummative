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
#include "asteroid/player_character.h"
#include "asteroid/game_manager.h"

namespace neko::pongsoso
{

PlayerCharacterManager::PlayerCharacterManager(EntityManager& entityManager, PhysicsManager& physicsManager, GameManager& gameManager) :
    ComponentManager(entityManager),
    physicsManager_(physicsManager),
    gameManager_(gameManager)
    
{

}

void PlayerCharacterManager::FixedUpdate(seconds dt)
{
    for(Entity playerEntity = 0; playerEntity < entityManager_.get().GetEntitiesSize(); playerEntity++)
    {
        if(!entityManager_.get().HasComponent(playerEntity, 
            EntityMask(ComponentType::PLAYER_CHARACTER)))
            continue;
        auto playerBody = physicsManager_.get().GetBody(playerEntity);
        auto playerCharacter = GetComponent(playerEntity);
        const auto input = playerCharacter.input;

        const bool up = input & PlayerInput::UP;
        const bool down = input & PlayerInput::DOWN;

        auto dir = Vec2f::up;

        const auto acceleration = (((down ? -1.0f : 0.0f) + (up ? 1.0f : 0.0f)) * dir) * 5; // *5 = multiplication de l'accélération du player

        playerBody.velocity = acceleration;
        if ((playerBody.position.y > playerMaxHeight && playerBody.velocity.y > 0)|| (playerBody.position.y < playerMinHeight && playerBody.velocity.y < 0))
        {
        	// permet que les joueurs ne sortent pas de l'écran par le haut ou le bas
            playerBody.velocity = Vec2f::zero;
        }

        physicsManager_.get().SetBody(playerEntity, playerBody);
    }
}

PlayerCharacterManager& PlayerCharacterManager::operator=(const PlayerCharacterManager& playerCharacterManager)
{
    components_ = playerCharacterManager.components_;
    //We do NOT copy the physics manager
    return *this;
}
}
#include "StateMachine.h"

StateMachine::StateMachine() :
    currentState(State::IDLE),
    currentFrame(0),
    timeAccumulator(0.0f)
{
}

void StateMachine::setup()
{
    // config parameters: State, row, totalFrames, frameDuration (seconds)
    addConfig(State::IDLE, 0, 6, 0.15f);
    addConfig(State::WALK, 1, 6, 0.10f);
    addConfig(State::JUMP, 2, 6, 0.15f);
    addConfig(State::ATTACK, 3, 6, 0.10f);
    addConfig(State::WALK_UP, 4, 6, 0.12f);
    addConfig(State::WALK_DOWN, 5, 6, 0.12f);
    addConfig(State::CROUCH, 6, 6, 0.15f);
    addConfig(State::ULTIMATE, 7, 6, 0.10f);
    addConfig(State::EMOTE, 8, 6, 0.15f);
}

void StateMachine::addConfig(State state, int row, int frames, float duration)
{
    configs[state] = { row, frames, duration };
}

void StateMachine::updateStateFromInput(bool up, bool down, bool left, bool right, bool attack, bool jump, bool crouch, bool ultimate, bool emote)
{
    State nextState = State::IDLE;

    if (ultimate)       nextState = State::ULTIMATE;
    else if (emote)     nextState = State::EMOTE;
    else if (attack)    nextState = State::ATTACK;
    else if (jump)      nextState = State::JUMP;
    else if (crouch)    nextState = State::CROUCH;
    else if (left)      nextState = State::WALK;
    else if (right)     nextState = State::WALK;
    else if (up)        nextState = State::WALK_UP;
    else if (down)      nextState = State::WALK_DOWN;

    // Reinicia contadores si cambiamos de animación
    if (nextState != currentState)
    {
        currentState = nextState;
        currentFrame = 0;
        timeAccumulator = 0.0f;
    }
}

void StateMachine::update(float deltaTime)
{
    timeAccumulator += deltaTime;
    float duration = configs[currentState].frameDuration;
    int frames = configs[currentState].totalFrames;

    if (timeAccumulator >= duration)
    {
        timeAccumulator -= duration;
        currentFrame++;

        if (currentFrame >= frames)
        {
            currentFrame = 0;
        }
    }
}

int StateMachine::getCurrentRow() const {
    return configs.at(currentState).row;
}

int StateMachine::getCurrentCol() const {
    return currentFrame;
}
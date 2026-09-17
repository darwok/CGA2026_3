#include "StateMachine.h"

StateMachine::StateMachine() :
    currentState(State::IDLE),
    currentFrame(0),
<<<<<<< Updated upstream
    timeAccumulator(0.0f)
=======
    timeAccumulator(0.0f),
    isFacingLeft(false),
    locked(false)
>>>>>>> Stashed changes
{
}

void StateMachine::setup()
{
<<<<<<< Updated upstream
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
=======
    // config parameters: State, row (0-indexed), totalFrames, frameDuration (seconds), isOneShot
    // Aquí puedes ajustar fácilmente si una animación tiene menos cuadros. Por ejemplo, si la fila 4 
    // solo tiene 5 columnas útiles, cambias el '6' por un '5' en ATTACK.
    addConfig(State::IDLE, 0, 6, 0.15f, false);
    addConfig(State::WALK, 1, 6, 0.10f, false);
    addConfig(State::JUMP, 2, 6, 0.15f, true);
    addConfig(State::ATTACK, 3, 6, 0.10f, true);
    addConfig(State::WALK_UP, 4, 6, 0.12f, false);
    addConfig(State::WALK_DOWN, 5, 6, 0.12f, false);
    addConfig(State::CROUCH, 6, 6, 0.15f, false);
    addConfig(State::ULTIMATE, 7, 6, 0.10f, true);
    addConfig(State::EMOTE, 8, 6, 0.15f, true);
}

void StateMachine::addConfig(State state, int row, int frames, float duration, bool oneShot)
{
    configs[state] = { row, frames, duration, oneShot };
>>>>>>> Stashed changes
}

void StateMachine::updateStateFromInput(bool up, bool down, bool left, bool right, bool attack, bool jump, bool crouch, bool ultimate, bool emote)
{
<<<<<<< Updated upstream
    State nextState = State::IDLE;

=======
    // Si estamos en una animación que no debe interrumpirse (ej. Ataque), ignoramos el input nuevo
    if (locked) return;

    State nextState = State::IDLE;

    // Jerarquía de prioridades para las transiciones
>>>>>>> Stashed changes
    if (ultimate)       nextState = State::ULTIMATE;
    else if (emote)     nextState = State::EMOTE;
    else if (attack)    nextState = State::ATTACK;
    else if (jump)      nextState = State::JUMP;
    else if (crouch)    nextState = State::CROUCH;
<<<<<<< Updated upstream
    else if (left)      nextState = State::WALK;
    else if (right)     nextState = State::WALK;
    else if (up)        nextState = State::WALK_UP;
    else if (down)      nextState = State::WALK_DOWN;

    // Reinicia contadores si cambiamos de animación
=======
    else if (left) { nextState = State::WALK; isFacingLeft = true; }
    else if (right) { nextState = State::WALK; isFacingLeft = false; }
    else if (up)        nextState = State::WALK_UP;
    else if (down)      nextState = State::WALK_DOWN;

    // Si el estado cambia, reseteamos los contadores
>>>>>>> Stashed changes
    if (nextState != currentState)
    {
        currentState = nextState;
        currentFrame = 0;
        timeAccumulator = 0.0f;
<<<<<<< Updated upstream
=======

        if (configs[currentState].isOneShot) {
            locked = true;
        }
>>>>>>> Stashed changes
    }
}

void StateMachine::update(float deltaTime)
{
    timeAccumulator += deltaTime;
    float duration = configs[currentState].frameDuration;
    int frames = configs[currentState].totalFrames;

<<<<<<< Updated upstream
=======
    // Verificamos si ya pasó el tiempo necesario para cambiar al siguiente cuadro
>>>>>>> Stashed changes
    if (timeAccumulator >= duration)
    {
        timeAccumulator -= duration;
        currentFrame++;

<<<<<<< Updated upstream
        if (currentFrame >= frames)
        {
            currentFrame = 0;
=======
        // Si la animación llega al último cuadro
        if (currentFrame >= frames)
        {
            if (configs[currentState].isOneShot) {
                locked = false;
                currentState = State::IDLE;
                currentFrame = 0;
            }
            else {
                // Si es un ciclo continuo (ej. Caminar), reinicia al cuadro 0
                currentFrame = 0;
            }
>>>>>>> Stashed changes
        }
    }
}

int StateMachine::getCurrentRow() const {
<<<<<<< Updated upstream
=======
    // Retorna la fila correspondiente a la configuración actual
>>>>>>> Stashed changes
    return configs.at(currentState).row;
}

int StateMachine::getCurrentCol() const {
    return currentFrame;
<<<<<<< Updated upstream
=======
}

bool StateMachine::isFlipped() const {
    return isFacingLeft;
>>>>>>> Stashed changes
}
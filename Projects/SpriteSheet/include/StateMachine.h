#pragma once
#include <map>

enum class State {
    IDLE,
    WALK,
    JUMP,
    ATTACK,
    WALK_UP,
    WALK_DOWN,
    CROUCH,
    ULTIMATE,
    EMOTE
};

struct AnimConfig {
    int row;            // A qué fila del spritesheet pertenece (0-indexed)
    int totalFrames;    // Cuántos cuadros útiles tiene 
    float frameDuration;// Duración de cada cuadro en segundos
};

class StateMachine
{
public:
    StateMachine();

    void setup();
    void updateStateFromInput(bool up, bool down, bool left, bool right, bool attack, bool jump, bool crouch, bool ultimate, bool emote);
    void update(float deltaTime);

    int getCurrentRow() const;
    int getCurrentCol() const;

    // Set number of cols and rows of my spritesheet
    float totalCols{ 6.0f };
    float totalRows{ 9.0f };

private:
    void addConfig(State state, int row, int frames, float duration);

    State currentState;
    std::map<State, AnimConfig> configs;

    int currentFrame;
    float timeAccumulator;
};
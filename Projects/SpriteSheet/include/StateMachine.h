#pragma once
#include <map>

// Definición de todos los estados posibles para el personaje
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
    int row;
    int totalFrames;    // Cuántos cuadros útiles tiene (por si una fila no usa todas las columnas)
    float frameDuration;// Duración de cada cuadro en segundos
    bool isOneShot;     // Si es true, la animación no se puede interrumpir hasta terminar
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
    bool isFlipped() const;

    // Set number of cols and rows of my spritesheet
    float totalCols{ 6.0f };
    float totalRows{ 9.0f };

private:
    void addConfig(State state, int row, int frames, float duration, bool oneShot);

    State currentState;
    std::map<State, AnimConfig> configs;

    int currentFrame;
    float timeAccumulator;

    bool isFacingLeft;
    bool locked; // Bloquea el cambio de estado si se está reproduciendo una animación One-Shot
};
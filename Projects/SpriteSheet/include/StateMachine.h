#pragma once
#include <map>

<<<<<<< Updated upstream
=======
// Definición de todos los estados posibles para el personaje
>>>>>>> Stashed changes
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

<<<<<<< Updated upstream
struct AnimConfig {
    int row;            // A qué fila del spritesheet pertenece (0-indexed)
    int totalFrames;    // Cuántos cuadros útiles tiene 
    float frameDuration;// Duración de cada cuadro en segundos
=======
// Estructura modular para configurar cada animación de forma independiente
struct AnimConfig {
    int row;            // A qué fila del spritesheet pertenece (0-indexed)
    int totalFrames;    // Cuántos cuadros útiles tiene (por si una fila no usa todas las columnas)
    float frameDuration;// Duración de cada cuadro en segundos
    bool isOneShot;     // Si es true, la animación no se puede interrumpir hasta terminar
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
=======
    bool isFlipped() const;
>>>>>>> Stashed changes

    // Set number of cols and rows of my spritesheet
    float totalCols{ 6.0f };
    float totalRows{ 9.0f };

private:
<<<<<<< Updated upstream
    void addConfig(State state, int row, int frames, float duration);
=======
    void addConfig(State state, int row, int frames, float duration, bool oneShot);
>>>>>>> Stashed changes

    State currentState;
    std::map<State, AnimConfig> configs;

    int currentFrame;
    float timeAccumulator;
<<<<<<< Updated upstream
=======

    bool isFacingLeft;
    bool locked; // Bloquea el cambio de estado si se está reproduciendo una animación One-Shot
>>>>>>> Stashed changes
};
# 🛡️ Bellicus  —  Top-Down Tank Survival Game

**Tank game based on clean architecture and classic wave survival
gameplay.**\
Bellicus is a top-down action game where the player controls a tank and
survives against increasingly difficult enemy waves.

------------------------------------------------------------------------

## 🎮 Overview

![Menu](./img/bellicus.png)

Bellicus is an experimental indie game project focused on:

-   Top-down 2D gameplay with shooting and movement mechanics\
-   Wave-based enemy spawning\
-   Clean and modular C architecture\
-   Separation of concerns using game states and systems

The main goal is to survive as long as possible while managing enemies,
positioning, and resources.

------------------------------------------------------------------------

## 🚀 Features

-   ✔️ Top-down 2D tank gameplay\
-   ✔️ Modular state machine (menu, gameplay, pause)\
-   ✔️ Clean and scalable C codebase\
-   ✔️ Integrated sprites, sound effects, and music
-   ✔️ Tank gears\
-   ✔️ Linear spawn of enemies\
-   ✔️ Random generation of battlefield\
-   ✔️ Original start and end game screens\
-   ✔️ Collisions with Ray Casting

------------------------------------------------------------------------

## 🧠 Architecture

Bellicus is built using a modular and procedural architecture in C, inspired by clean architecture principles and classic game engine design.

The project is structured around a **central game loop**, responsible for:

- Processing player input
- Updating game logic
- Handling physics and collisions
- Rendering graphics and playing audio

### Core Concepts

- **State-based system**  
  The game uses a state machine approach to manage different stages such as menu, gameplay, and end screens. Each state encapsulates its own logic, update cycle, and rendering behavior.

- **Modular subsystems**  
  Game logic is divided into independent modules, such as:
  - Math and physics calculations (`calc/`)
  - Rendering and assets
  - Audio system
  - Gameplay rules and enemy spawning

- **Collision system with Ray Casting**  
  Ray casting is used for collision detection, allowing accurate interaction between tanks, projectiles, enemies, and the environment.

- **Procedural battlefield generation**  
  The battlefield layout is generated dynamically, increasing replayability and reducing static level dependency.

This architecture prioritizes **clarity, performance, and extensibility**, making it suitable for experimentation with new mechanics and future refactoring into a more engine-oriented design.


------------------------------------------------------------------------

## 📦 Requirements

-   GCC or compatible C compiler\
-   Make (optional but recommended)\
-   Standard C libraries
-   SDL2 (graphics, input, audio)
-   SDL2_image
-   SDL2_mixer
-   SDL2_gfx


------------------------------------------------------------------------

## 🛠️ Build

Bellicus does not enforce a specific build system.

To compile and run the game, you must have **SDL2 and its extensions properly installed and linked** in your development environment.

### Compilation

The project can be compiled using any build system or IDE of your choice  
(e.g., Makefiles, CMake, IDE projects).

Below is a **simplified example** using GCC.  
Additional linker flags (such as `SDL2main` or platform-specific libraries)
may be required depending on your environment.

```bash
gcc *.c calc/*.c -o bellicus \
    -lSDL2main -lSDL2 -lSDL2_gfx -lSDL2_image -lSDL2_mixer
```


------------------------------------------------------------------------

## ▶️ How to Play

1.  Run the executable:

        ./bellicus

2.  Survive enemy waves and improve your score

------------------------------------------------------------------------

## 📂 Repository Structure

    img/        # Visual assets
    sprites/    # Original sprites
    calc/       # Math and physics logic
    music/      # Background music
    sound/      # Sound effects
    fontes/     # Fonts        
    README.md   # Documentation
    LICENSE     # Proprietary license
    espera.c    # Function for events
    estados.c   # Game state definitions and transitions
    global.c    # global variables
    info.c      # Formatting functions
    main.c      # Main loop
    menu.c      # Start menu
    save.c      # Save system **(under development)**

------------------------------------------------------------------------

## 📜 License

This project is licensed under proprietary license.

------------------------------------------------------------------------

## 🤝 Contributions

Contributions are welcome via issues and pull requests.

------------------------------------------------------------------------

## Future Concepts

![Conceito](./img/conceito.png)

------------------------------------------------------------------------

## 📌 Authors

**Marcos Vitor Mendonça**\
Computer Science Undergraduate -- UERJ\
GitHub: https://github.com/m-m-legend

**Eraldo Botelho**\
Computer Science Undergraduate -- UERJ\
GitHub: https://github.com/SlliperySwan

**Bruno Domenico**\
Computer Science Undergraduate -- UERJ\
GitHub: https://github.com/Brunodpp
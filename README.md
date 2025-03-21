# Dino_Odyssey

This is my first game project, developed using SDL2 and C++. It is an endless runner game based on the Google Chrome Dinosaur game, enhanced with additional features and mechanics to expand the gameplay experience.

## Overview
The game involves a player character running continuously across platforms, avoiding obstacles, and collecting power-ups. The objective is to achieve the highest score possible while the difficulty increases over time (currently the difficulty is fixed). It incorporates elements such as multiple ground levels, shooting mechanics, and customizable skins.

## Authors
- **Fully developed by** [me](https://github.com/maaL6)
- **Visual assets and concept designed by** [@ic.bq_002](https://www.instagram.com/ic.bq_002/)

### Key Features
- **Endless Gameplay**: Continuous running with procedurally generated platforms and obstacles.
- **Power-Ups**: Collectible "Buffs" enable shooting to destroy obstacles.
- **Dual Ground Levels**: Platforms switch between two heights for varied navigation.
- **Progressive Difficulty**: Obstacle speed increases as the game progresses.
- **Parallax Scrolling**: Multi-layered backgrounds for a depth effect.
- **Character Skins**: Multiple selectable skins available via a skin selector.

## Gameplay Mechanics
- **Controls**:
  - `Space`: Jump to avoid obstacles or gaps.
  - `P`: Shoot bullets when power-ups are active.
  - `Pause/Continue`: Toggle between paused and active states.
- **Obstacles**:
  - Spikes and robots reduce player health on collision.
  - Falling off platforms results in game over.
- **Scoring**: Points accumulate over time; high scores are saved to `high_score.txt`.
- **Health**: Player starts with 3 hearts; depletion ends the game.
- **Power-Ups**: Collecting buffs grants bullets to clear obstacles.

## Code Structure
- **Main Components**:
  - `main()`: Manages game loops for menu, skin selection, instructions, and gameplay.
  - `resetGame()`: Resets player position, obstacles, and game state.
  - `Init()`: Initializes SDL2, window, renderer, and required subsystems.
  - `LoadMedia()`: Loads textures, sounds, and fonts.
  - `Close()`: Frees resources and shuts down SDL.

- **Game Loops**:
  - **Menu Loop**: Handles main menu options (play, help, settings, exit).
  - **Skin Selector**: Allows selection of character skins.
  - **Instruction Loop**: Displays multi-page instructions.
  - **Game Loop**: Controls player movement, obstacle generation, collision detection, and rendering.

- **Assets**:
  - **Images**: Backgrounds, buttons, skins, obstacles, power-ups (`images/` directory).
  - **Sounds**: Music and sound effects (jump, shoot, lose) in `sound/`.
  - **Fonts**: Pixel font for score display (`font/pixel_font.ttf`).

## References
This project was built with guidance and inspiration from the following resources:
- **LazyFoo**: Tutorials on SDL2 programming (http://lazyfoo.net/tutorials/SDL/).
- **LTNC VNU**: Programming resources and examples (https://sites.google.com/site/ltncvnu/).
- **Dino in Zungle**: Open-source project by thai-nm (https://github.com/thai-nm/dino-in-zungle).


---
#### Explore the code and enjoy the game.

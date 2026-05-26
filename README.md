# Turn-Based RPG Prototype (SFML C++)

![Gameplay](./previews/Gameplay.gif)

## Description
Turn-based RPG combat prototype inspired by Honkai: Star Rail, built using SFML in C++.

This project was developed as my first major programming project in university, where it received the highest grade among BS Computer Science and Information Technology students for that semester.


## Features
- Turn-based combat system (speed-based turn order)
- Basic attack, skill, and ultimate mechanics
- Enemy targeting system
- Configurable character and enemy stats via JSON
- Custom animations and models (created in Blender)


## Built With
- C++
- SFML 2.6.1 (Visual C++ 17 - 64-bit)
- nlohmann/json (single-header library)
- Blender (for models and animations)


## Controls
- **Q** — Basic Attack  
- **E** — Skill  
- **Space** — Ultimate  
- **← / →** — Select target enemy  


## Configuration
Game settings can be modified in `config.json`.

### Editable Parameters
The following values are safe to modify for gameplay experimentation:

```json
{
  "initialActionGauge": 10000,
  "skillPoints": 3,
  "characters": [
    {
      "speed": 100,
      "health": 163,
      "attack": 84,
      "errate": 0.194,
      "critRate": 0.5,
      "critDamage": 0.5
    }
  ]
}
```

### Notes
- Only the parameters listed above are recommended to change  
- Other fields in `config.json` are used internally and may break the game if modified
- Due to file size limitations, sprite sheets are hosted externally:
    https://drive.google.com/drive/folders/1S-VO5RaTo5IRSYlhghHBH-ZmVT1AdA3v
    
  Installation:
  - Download and extract the files
  - Place them in the project's root directory
 

## Screenshots
![TextureLoading](./previews/Texture%20Loading.gif)
![Gameplay2](./previews/Gameplay2.gif)
![SkillAttack](./previews/Skill%20Attack.gif)
![UltimateAttack](./previews/Ultimate%20Attack.gif)





# Arcade Games (Working Title)

A collection of arcade games such as snake and tetris written in c++ using glfw, opengl and glm. I want to explore creating some more simple games and see how I naturally structure an "engine" around the problems I encounter. I intend to use my findings to later create 2D games and potentially 3D games.

## Architecture
- There is an "Engine" which is a static library that provides helper functions on top of glfw, opengl/glad and glm.
- There is a "Launcher" which provides a menu to choose which game to play and serves as the main application.
- The games are dynamically linked libraries that use functions from the Engine which are passed to the dll when it is first loaded.

## Current State
- ~~No playable games, No engine, initial readme, project and dependancies set up.~~
- ~~No playable games, some basic engine code, basic "Dancefloor" example.~~
- Basic 'engine', Dancefloor example and playable Sokoban (Although it is janky and has memory issues due to being thrown together).
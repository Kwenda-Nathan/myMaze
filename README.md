# Mystery Maze

Welcome to the Mystery Maze Game! This document provides detailed instructions on how to play, explains the maze generation process, and highlights additional features implemented in the game.

# How to Play

## Objective

Navigate through the maze to reach the exit while avoiding enemies, collecting food, and solving puzzles to progress.

## Controls

1 Movement:
    -W: Move up
    -A: Move left
    -S: Move down
    -D: Move right

2 Volume Adjustment:
    - Use the slider to adjust volume with the mouse.

3 Pause/Resume:
    - press the pause button 

## Gameplay Mechanics

1 Maze Navigation:
    - Use the arrow W, S, A, D to navigate through the maze.
    - collect food for score 100 for level1, level++ +150

2 Enemies:
    -Enemies patrol predefined paths in the maze. Avoid colliding with them to prevent losing score points

3 Food Collection:
    - collect food items that appear randomly in the maze

4 Puzzle:
    - solve math puzzle to progress to next level

5 Exit:
    - Locate the exit, marked with a green square, to complete the level.

6 Saving and Loading:
    - press u to save your game progress
    - Load previous progress from menu

# Maze Generation

## Algorithm

1 Initialization:

    - Start with a grid where all cells are walls.
    - Mark a starting cell as a path (open space).
    - Use a stack to remember the current path for          backtracking.

2 Carving Paths:

    - While there are still cells to process:

        - Shuffle Directions: Randomize the order of traversal directions (up, right, down, left).

        - Choose a Neighbor: Check adjacent cells that are two steps away:
            (1)The cell is within the maze bounds.
            (2)The cell is still a wall (unvisited).

        - Carve Path to Neighbor:

            -Remove the wall between the current cell and the chosen neighbor.
            - Mark the neighbor as part of the path.
            -Push the current cell onto the stack and move to the neighbor.

3 Backtracking:

    - If no unvisited neighbors are found, backtrack by popping a cell from the stack.
    - Repeat until the stack is empty.

4 Result:

    - The grid now represents a maze with open paths (0) and walls (1).

# Additional Features

## Audio system

Background music plays continuously.

Sound effects:

Collecting food

Losing game


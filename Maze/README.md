## Maze Generator/Solver

Simple maze generator/solver based on recursive backtracking. There are a number of defines that can change the maze.

| Define  | Description |
| ------------- | ------------- | 
| ```MAZE_WIDTH``` | Maze Width |
| ```MAZE_HEIGHT``` | Maze Height |
| ```DISP_COL``` | Display Colour in format r;g;b |
| ```SOLVE_COL```| Solved Path Colour in format r;g;b |
| ```THEME```| One of the 3 options below |

Themes change the maze characters displayed. Current themes are:

| Name  | Wall | Floor | Empty |
| --- | --- | --- | --- | 
| ```DEFAULT```|```\|```|```_```|``` ```|
| ```FUTURE```|```/```|```_```|``` ```|
| ```SURREAL```|```@```|```?```|```%```|


## Potential Additions
- [ ] Add more maze algorithms
- [ ] More display customization

## Example
![image](https://i.imgur.com/pwzRTHX.png)

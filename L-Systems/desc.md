## L-Systems

Implementation of L-Systems. Symbols supported are

| Symbol  | Description |
| ------------- | ------------- | 
| ```A - Z``` | move in current direction, except for constants |
| ```+``` | turn right |
| ```-``` | turn left |
| ```{```| change colour to blue |
| ```}```| change colour to purple |
| ```~```| pseudo-random colour |
| ```\|```| reverse direction (180 degree turn) |

There are 3 arrays needed to make your own L-System.

| Variable | Type | Description |
| ------------- | ------------- | ------------- | 
| ```rootString``` | ``` char const ``` | starting string before iterations |
| ```rootConst``` | ``` char const ``` | string of constants used |
| ```ruleArray``` | ```struct Rule ``` | null-terminated array of rules, in the form 'find', "replace" |

The L-Systems are created through ```genLSystem```. You can customize ```iter``` for number of iterations. They are printed through ```printLSystem``` where ```x``` and ```y``` are the coordinates of the visual. A number of curve presets are availible, taken from [here](http://paulbourke.net/fractals/lsys/), which you can use by simply pasting the macro.

## Potential Additions
- [ ] More symbols
- [ ] More options for display

## Example
![image](https://i.imgur.com/oBEAuEm.png)

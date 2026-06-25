
# **C_grafeq**
use interval arithmetic to plot implict function

reference: [Reliable Two-Dimensional Graphing Methods for Mathematical Formulae with Two Free Variables](https://www.dgp.toronto.edu/~mooncake/papers/SIGGRAPH2001_Tupper.pdf)

### usage:
1. ##### create a file of the expression
  * note: `xy`should be `x*y`
  * note: no space allowed, eg.`x + y`should be `x+y`
2. ##### compile
  * `./easm < expression.file > ouput.asm
3. ##### run
  * 1. run `C_grafeq`
  * 2. input `9`for 512x512 screen
  * 3.
      |command|example|note|
      |-|-|-|
      |`plot <filename.asm>`|`plot ./example/c.expr.asm`|plot the equation|
      |`range`|`range -10 10 -10 10`|initially,`xmin=ymin=-10,xmax=ymax=10`|
      |`l`|`l`|list all equation filename and i|
      |`del`|`del 0`|delete the specific|
      |`showpx`|`showpx 255 255`|show the horizontal and vertical line|
  * 4. ##### exit
  press `Esc` in gui window and press some char in console ended with enter

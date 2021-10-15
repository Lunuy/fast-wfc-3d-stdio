# fast-wfc-3d-stdio
Standard input/output implementation of fast-wfc-3d for external call.
You can run WFC with options that entered from stdin. This program will print progress while WFC.

# Used libraries
[fast-wfc-3d-undoable](https://github.com/Lunuy/fast-wfc-3d-undoable)

You should copy header files(.hpp). You should copy fastwfc.dll and fastwfc_static.lib to lib folder.

# Use of this program
[minecraft WFC plugin](https://github.com/Lunuy/wfc-minecraft-plugin)

# Not Implemented features yet
- symmetry
- tiling_wfc

# Stdin
```
input_size_z input_size_y input_size_x
periodic_input_z periodic_input_y periodic_input_x
periodic_output_z periodic_output_y periodic_output_x
output_size_z output_size_y output_size_x
N
fixed_blocks_numbers
// Already fixed blocks
// fixed_blocks_numbers times {
z y x blockId
// }

// Input sample
// for z, for y, for x {
blockId
// }
```

## Example stdin
```
1 2 1
1 1 1
1 1 1
2 2 2
1
0
1 0
```
X-Y-Z Periodic input/output, N=1, No fixed blocks.


# Stdout
## ING SET
```
ING n
// n times {
SET z y x blockId
// }
```
Doing WFC. Changed blocks are printed using "SET".

## UNDO
```
UNDO
```
Failed WFC. But will retry using undo.

## FINISHSET / FINISH
```
// output x*y*z times {
FINISHSET z y x blockId
// }
FINISH
```
Finished WFC. Results are printed using "FINISHSET".
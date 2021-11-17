# About

External for Windows version of Max 8 (Max/MSP). Messages to set function's breakpoints from function object's dump list.

- Input: list from [function] object after "listdump" message
- Messages: "mode" 0 is lineal, 1 is curve
- Output:
	- "clear"
	- "setdomain" f
	- "setrange" f f
	- f f 
	- ...
	- f f
	- "setcurve" i //if mode=1
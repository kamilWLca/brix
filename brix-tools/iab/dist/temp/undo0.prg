CanvasSizeXY
2000	2000

IconData
BEGIN_BLOCK

ArrayBool1
_
1	0	0	0
_
_
Arrays of boolean variables can be stored in the component. The index must fall between 0 and the maximum size -1. No Error is generated for out of range indexes.
ArrayBool1
_
30	45	130	142
1	0
parameter
size	1	2	1024	1024
EndOfValues
EndOfLabels
Memory allocated for array

parameter
default value	0	_	_	0
EndOfValues
EndOfLabels
default value for uninitialised values

inputport

0	1	30	65	0	0	1	mandatory= 0	0	1
index

Run_ArrayBoolWrite	2	EndOfFunc
BEGIN_LINE
0	-1	0	0	-1	0
END_LINE
inputport

0	1	30	100	1	0	1	mandatory= 0	0	1
index

Run_ArrayBoolRead	1	EndOfFunc
BEGIN_LINE
0	-1	0	0	-1	0
END_LINE
inputport

0	0	30	75	2	0	1	mandatory= 0	0	1
data

Run_ArrayBoolWrite	1	EndOfFunc
BEGIN_LINE
1	1	0	0	-1	0
160	110
160	15
5	15
5	75
END_LINE
outputport

1	0	130	110	0	0	1	mandatory= 0	0	1
data

Run_ArrayBoolRead	2	EndOfFunc
BEGIN_LINE
0	1	2	1	-1	0
END_LINE
startport

2	-1	30	125	0	0	1	mandatory= 0	0	1
clear

Run_ArrayBoolClear	0	EndOfFunc
BEGIN_LINE
0	-1	0	2	-1	0
END_LINE
startport

2	-1	30	55	1	0	1	mandatory= 0	0	1
write

Run_ArrayBoolWrite	0	EndOfFunc
BEGIN_LINE
0	-1	0	2	-1	0
END_LINE
startport

2	-1	30	90	2	0	1	mandatory= 0	0	1
read

Run_ArrayBoolRead	0	EndOfFunc
BEGIN_LINE
0	-1	0	2	-1	0
END_LINE
finishport

3	0	130	125	0	0	1	mandatory= 0	0	1
clear

Run_ArrayBoolClear	1	EndOfFunc
BEGIN_LINE
0	3	1	3	-1	0
END_LINE
finishport

3	0	130	55	1	0	1	mandatory= 0	0	1
write

Run_ArrayBoolWrite	3	EndOfFunc
BEGIN_LINE
0	3	2	3	-1	0
END_LINE
finishport

3	0	130	90	2	0	1	mandatory= 0	0	1
read

Run_ArrayBoolRead	3	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
finishport

3	0	130	65	3	0	1	mandatory= 0	0	1
err

Run_ArrayBoolWrite	4	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
finishport

3	0	130	100	4	0	1	mandatory= 0	0	1
err

Run_ArrayBoolRead	4	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
END_BLOCK
BEGIN_BLOCK

ArrayReal1
_
2	0	0	0
_
_
Arrays of real value (floating point) variables can be stored in the component\nThe index must fall between 0 and the maximum size -1. No Error is generated for out of range indexes.
ArrayReal1
_
65	155	165	252
1	0
parameter
size	1	2	1024	1024
EndOfValues
EndOfLabels
Memory allocated for array

parameter
default value	2	_	_	NaN
EndOfValues
EndOfLabels
default value for uninitialised values

inputport

0	1	65	175	0	0	1	mandatory= 0	0	1
index

Run_ArrayRealWrite	2	EndOfFunc
BEGIN_LINE
0	-1	0	0	-1	0
END_LINE
inputport

0	1	65	210	1	0	1	mandatory= 0	0	1
index

Run_ArrayRealRead	1	EndOfFunc
BEGIN_LINE
0	-1	0	0	-1	0
END_LINE
inputport

0	2	65	185	2	0	1	mandatory= 0	0	1
data

Run_ArrayRealWrite	1	EndOfFunc
BEGIN_LINE
1	2	0	0	-1	0
200	220
200	260
40	260
40	185
END_LINE
outputport

1	2	165	220	0	0	1	mandatory= 0	0	1
data

Run_ArrayRealRead	2	EndOfFunc
BEGIN_LINE
0	2	2	1	-1	0
END_LINE
startport

2	-1	65	235	0	0	1	mandatory= 0	0	1
clear

Run_ArrayRealClear	0	EndOfFunc
BEGIN_LINE
0	-1	0	2	-1	0
END_LINE
startport

2	-1	65	165	1	0	1	mandatory= 0	0	1
write

Run_ArrayRealWrite	0	EndOfFunc
BEGIN_LINE
0	-1	0	2	-1	0
END_LINE
startport

2	-1	65	200	2	0	1	mandatory= 0	0	1
read

Run_ArrayRealRead	0	EndOfFunc
BEGIN_LINE
0	-1	0	2	-1	0
END_LINE
finishport

3	2	165	235	0	0	1	mandatory= 0	0	1
clear

Run_ArrayRealClear	1	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
finishport

3	2	165	165	1	0	1	mandatory= 0	0	1
write

Run_ArrayRealWrite	3	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
finishport

3	2	165	200	2	0	1	mandatory= 0	0	1
read

Run_ArrayRealRead	3	EndOfFunc
BEGIN_LINE
0	3	0	3	-1	0
END_LINE
finishport

3	2	165	175	3	0	1	mandatory= 0	0	1
err

Run_ArrayRealWrite	4	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
finishport

3	2	165	210	4	0	1	mandatory= 0	0	1
err

Run_ArrayRealRead	4	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
END_BLOCK
BEGIN_BLOCK

ArrayString1
_
3	0	0	0
_
_
Arrays of string variables can be stored in the component \nThe index must fall between 0 and the maximum size -1. \nNo Error is generated for out of range indexes.\nThe string length matches that if the connected ports.
ArrayString1
_
190	70	290	167
1	0
parameter
size	1	2	1024	1024
EndOfValues
EndOfLabels
Memory allocated for array

parameter
default value	3	_	_	_
EndOfValues
EndOfLabels
default value for uninitialised values

inputport

0	1	190	90	0	0	1	mandatory= 0	0	1
index

Run_ArrayStringWrite	2	EndOfFunc
BEGIN_LINE
0	-1	0	0	-1	0
END_LINE
inputport

0	1	190	125	1	0	1	mandatory= 0	0	1
index

Run_ArrayStringRead	1	EndOfFunc
BEGIN_LINE
0	-1	0	0	-1	0
END_LINE
inputport

0	3	190	100	2	0	1	mandatory= 0	0	1
data

Run_ArrayStringWrite	1	EndOfFunc
BEGIN_LINE
1	3	0	0	-1	0
320	135
320	25
170	25
170	100
END_LINE
outputport

1	3	290	135	0	0	1	mandatory= 0	0	1
data

Run_ArrayStringRead	2	EndOfFunc
BEGIN_LINE
0	3	2	1	-1	0
END_LINE
startport

2	-1	190	150	0	0	1	mandatory= 0	0	1
clear

Run_ArrayStringClear	0	EndOfFunc
BEGIN_LINE
1	2	2	2	-1	0
175	200
175	150
END_LINE
startport

2	-1	190	80	1	0	1	mandatory= 0	0	1
write

Run_ArrayStringWrite	0	EndOfFunc
BEGIN_LINE
1	1	0	2	-1	0
175	125
175	80
END_LINE
startport

2	-1	190	115	2	0	1	mandatory= 0	0	1
read

Run_ArrayStringRead	0	EndOfFunc
BEGIN_LINE
1	1	1	2	-1	0
165	55
165	115
END_LINE
finishport

3	3	290	150	0	0	1	mandatory= 0	0	1
clear

Run_ArrayStringClear	1	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
finishport

3	3	290	80	1	0	1	mandatory= 0	0	1
write

Run_ArrayStringWrite	3	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
finishport

3	3	290	115	2	0	1	mandatory= 0	0	1
read

Run_ArrayStringRead	3	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
finishport

3	3	290	90	3	0	1	mandatory= 0	0	1
err

Run_ArrayStringWrite	4	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
finishport

3	3	290	125	4	0	1	mandatory= 0	0	1
err

Run_ArrayStringRead	4	EndOfFunc
BEGIN_LINE
0	-1	0	3	-1	0
END_LINE
END_BLOCK
END_OF_BLOCKS

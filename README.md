# tessellator


Usage:

tessellator.exe -i <input file name> -ld <linear deflection> -la <angular deflection>> -ir <is relative>

-i <input file name> is mandatory
-ld <linear deflection> is optional     [double]
-la <angular deflection> is optional    [double]
-ir <is relative> is optional           [char 'Y' or 'N']

note: when not specified the followind default values are used:
linear deflection = 0.1
angular deflection = 0.1
is relative = true

Please, consider using opencascade automatically computed default values
for visualization

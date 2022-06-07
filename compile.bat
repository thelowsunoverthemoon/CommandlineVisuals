@ECHO OFF

SET "path[cl]="

IF not exist Programs (
    MD Programs
)
%comspec% /K ""%path[cl]%" & FOR /D %%Q in (*) DO (CL /FePrograms\ "%%Q\*.c")"

PAUSE
EXIT /B

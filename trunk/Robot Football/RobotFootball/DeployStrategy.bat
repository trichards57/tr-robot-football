if not exist c:\strategy mkdir c:\strategy
if not exist c:\strategy\blue mkdir c:\strategy\blue
if not exist c:\strategy\yellow mkdir c:\strategy\yellow

copy /y ..\Debug\*Strategy.dll c:\strategy\blue
copy /y ..\Debug\*Strategy.dll c:\strategy\yellow

copy /y ..\Debug\*Strategy.pdb c:\strategy\blue
copy /y ..\Debug\*Strategy.pdb c:\strategy\yellow

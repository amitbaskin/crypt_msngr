open in visual studio 2019 and set the following:
project->properties->c/c++->general->additional include directories: <C:\cryptopp-CRYPTOPP_8_5_0;C:\boost_1_77_0_rc1\boost_1_77_0>
project->properties->c/c++->c++ language stadard-></std:c++ 17>
project->properties->c/c++->preprocessor: <_WIN32_WINNT=0x0601>
project->properties->c/c++->code generation: </mtd>
project->properties->linker->system->subsystem: <console>
project->properties->linker-input>additional dependencies: <C:\cryptopp-CRYPTOPP_8_5_0\Win32\Output\Debug\cryptlib.lib;>

@echo off
cls

stamp_timer.exe start

set libs=-luser32 -lopengl32 -lgdi32 -lShell32 -l"..\..\enet\enet64" -lWs2_32 -lWinmm
set comp=-std=c11 -Dm_internal -DDEBUG -Dm_app

set warn=-Wextra -Wpedantic -Wall
set warn=%warn% -Wno-switch
set warn=%warn% -Wno-c99-designator
set warn=%warn% -Wno-writable-strings
set warn=%warn% -Wno-gnu-anonymous-struct
set warn=%warn% -Wno-nested-anon-types
set warn=%warn% -Wno-language-extension-token
set warn=%warn% -Wno-sign-compare
set warn=%warn% -Wno-missing-braces
set warn=%warn% -Wno-logical-op-parentheses
set warn=%warn% -Wno-char-subscripts
set warn=%warn% -Wno-c++98-compat-pedantic
set warn=%warn% -Wno-non-virtual-dtor
set warn=%warn% -Wno-suggest-override
set warn=%warn% -Wno-extra-semi-stmt
set warn=%warn% -Wno-old-style-cast
set warn=%warn% -Wno-double-promotion
set warn=%warn% -Wno-reserved-macro-identifier
set warn=%warn% -Wno-sign-conversion
set warn=%warn% -Wno-implicit-int-float-conversion
set warn=%warn% -Wno-missing-noreturn
set warn=%warn% -Wno-string-conversion
set warn=%warn% -Wno-format-nonliteral
set warn=%warn% -Wno-covered-switch-default
set warn=%warn% -Wno-switch-enum
set warn=%warn% -Wno-newline-eof
set warn=%warn% -Wno-strict-prototypes
set warn=%warn% -Wno-incompatible-pointer-types-discards-qualifiers

@REM maybe we want these
@REM set warn=%warn% -Wno-cast-align
@REM set warn=%warn% -Wno-unreachable-code-return
@REM set warn=%warn% -Wno-cast-function-type
@REM set warn=%warn% -Wno-missing-prototypes
set warn=%warn% -Wno-unused-parameter
set warn=%warn% -Wno-unused-function
set warn=%warn% -Wno-enum-constexpr-conversion
@REM set warn=%warn% -Wno-unreachable-code-break
@REM set warn=%warn% -Wno-unused-template

pushd build
	clang ..\src\client.c -shared -g -o game.dll %libs% %comp% %warn%
	clang ..\src\server.c -shared -g -o game.dll %libs% %comp% %warn%
popd

stamp_timer.exe end
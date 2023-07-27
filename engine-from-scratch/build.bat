set render=src\engine\render\render.c src\engine\render\render_init.c src\engine\render\render_util.c src\engine\animation\animation.c
set input=src\engine\input\input.c
set physics=src\engine\physics\physics.c
set io=src\engine\io\io.c
set array_list=src\engine\array_list\array_list.c
set config=src\engine\config\config.c
set entity=src\engine\entity\entity.c
set audio=src\engine\audio\audio.c
set files=src\glad.c src\main.c src\engine\global.c src\engine\time.c %io% %render% %config% %input% %physics% %array_list% %entity% %audio%
set libs=C:\Users\zachm\OneDrive\Desktop\C-Game-Test\lib\SDL2main.lib C:\Users\zachm\OneDrive\Desktop\C-Game-Test\lib\SDL2.lib C:\Users\zachm\OneDrive\Desktop\C-Game-Test\lib\SDL2_mixer.lib

CL /Zi /I C:\Users\zachm\OneDrive\Desktop\C-Game-Test\include %files% /link %libs% /OUT:mygame.exe
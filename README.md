# Premake Example

Este repo esta pensado para mostrar como configurar proyectos de C usando Premake. El tutorial esta orientado para los alumnos que cursen la asignatura **Sistemas Operativos** en la **UTN FRBA**. Aunque puede ser útil para cualquier persona que quiera configurar sus proyectos de C/C++ con Premake.

## ¿Que es Premake?
Premake es un generador de proyectos. Esto quiere decir que cuando creamos un proyecto de C/C++, configuramos nuestro workspace usando scripts de Premake, y luego generamos el proyecto para el entorno o IDE que usemos. Actualmente, Premake soporta los siguientes entornos:

* Makefiles
* Visual Studio (en casi todas sus versiones).
* codelite.
* xcode4.

Es decir, Premake nos permite desligarnos del entorno de trabajo que usemos (lo cual es muy útil para proyectos multiplataforma).

Sin embargo, por mas que no estemos realizando un proyecto multiplataforma, podemos seguir obteniendo beneficios al usar Premake. Sobre todo a la hora de comparar escribir scripts de Makefiles vs escribir scrips de Premake. Los script de Premake son bastante simples, pero muy configurables.

## Configurando el workspace
Para ver como sería una configuración estandar de un workspace usando Premake, vamos a realizar un mini-proyecto en linux. El linux que se va a estar usando es el mismo que otorga la catedra de Sistemas Operativos de la UTN. La podes encontrar en [esta pagina](https://www.utnso.com.ar/recursos/máquinas-virtuales/).

El código de ejemplo que vamos a estar usando esta disponible en este mismo repositorio. Exite un branch llamado _tutorial_ en el que esta el proyecto sin ningún script de Premake para el que quiera descargar el proyecto limpio y crear sus propios script de Premake

Antes de arrancar describiendo el workspace, cabe recalcar que en este tutorial no se muestran todas las características ni reglas disponibles en Premake. En caso de que quieran interiorizar mas sobre Premake, el repo del mismo tiene una excelente [wiki](https://github.com/premake/premake-core/wiki) en la que se detallan todas las reglas y sus características.

También decir que en la página de Premake no hay binarios del programa para máquinas de 32 bits (que es la arquitectura de la máquina virtual de la catedra). Pero en este repo, dentro de la carpeta `premake/x86` esta el binario compilado para 32 bits. Por lo que simplemente se pueden bajar el repo y copiarse el binario para empezar a usar Premake.

Dicho todo esto, vamos a empezar describiendo un poco el workspace.

La estructura del workspace es la siguiente:

```
    workspace
        |
        |__ client
        |      |__src
        |          |__ client.c
        |
        |__ server
        |      |__src
        |          |__ server.c
        |
        |__ utils
        |      |__include
        |      |    |__ conn.h
        |      |    |__ utils.h
        |      |
        |      |__src
        |          |__ conn.c
        |
        |__ premake
               |__ x86
                    |_ premake5

```

El dentro tenemos tres proyectos distintos. Cada uno representa un ejecutable o librería distinta. En nuestro caso, `client` y `server` son dos ejecutables y `utils` es una librería (en nuestro caso una librería estática).

El proyecto `server` es un servidor super simple que lo único que hace es recibir mensajes de algun cliente, envia una respuesta `"Hello From Server"` , imprime por pantalla el mensaje que recibió del cliente, cierra la conexión con ese cliente y queda a la espera de otro cliente.

El proyecto `client` es un programa que solamente envía un mensaje `"Hello From Client"` al servidor, recibe la respuesta del servidor, imprime por pantalla el mensaje del servidor, y finaliza cerrando la conexión.

Por último, `utils` posee algunas estructuras y funciones axuliares para poder simplificar la conexión y envio de mensajes de los clientes.

Obviamente cada uno es libre de expandir mas este proyecto. Pero para el propósito de este tutorial, es mas que suficiente.

Ahora que ya tenemos el proyecto, empecemos a escribir los scripts necesarios.

### Scripts
En el directorio principal del workspace, vamos a crear un script llamado `premake5.lua` y abrirlo con cualquier editor de texto:

`touch premake5.lua`

Los scripts de Premake se pueden dividir en 2 partes. Una en la que configuramos las opciónes del workspace y otra en la que configuramos cada uno de los proyectos. Vamos a empezar configurando el workspace:

```lua
workspace "premake-example"
    configurations { "Debug", "Release" }
    architecture "x86"
```

Vamos ver que hace cada una de estas `reglas`:
* `workspace`: indica el nombre del workspace. En este caso, vamos a colocarle "premake-example" pero pueden colocar el nombre que mas se adecúe a su proyecto.
* `configurations`: indicamos que configuraciónes tendrán cada uno de los proyectos dentro del workspace. Dependiendo bajo que configuración estemos compilando, podemos setear o no distintos flags de compilación. Por ejemplo, algo muy comun es que para compilaciones en **Debug** tengamos activados varios flags de compilación que nos permiten debugear nuestros programas (como por ejemplo, la opción -g3 de gcc), mientras que en compilaciones en **Release** se suelen activar todos los flags para optimizar código. Pero cada uno es libre de colocar y nombrar las configuraciones que desee.
* `architecture`: indica la arquitectura de la máquina en la que se va a compilar. Como en nuestro caso vamos a estar usando la máquina virtual de la catedra de **Sistemas Operativos** (las cuales tienen linux de 32 bits), seteamos la arquitectura a x86. En caso de que se quiera compilar en un sistema de 64 bits, se deberá cambiar a x86_64. Mas adelante, vamos a ver como forma para soportar cualquiera de estas dos arquitecturas sin tener que reescribir el script.

Estas son las reglas básicas que se necesitan para configurar nuestro workspace. Obviamente, se pueden especificar mas reglas, pero con las mencionadas basta y sobra para la mayoría de los casos.

Ahora que tenemos seteado el workspace, vamos a empezar a configurar los proyectos. Empecemos por el proyecto `utils`. Vamos a listar las primeras reglas:

```lua
project "utils"
    kind "StaticLib"
    language "C"
    cdialect "GNU11"
```

Como hicimos con el workspace, vamos a ver que hacen estas reglas:
* `project`: indica el nombre del proyecto.
* `kind`: el tipo de proyecto. En nuestro caso, estamos creando una librería estática. Las opciones disponibles para esta regla son **StaticLib**, **SharedLib** y **ConsoleApp**.
* `language`: el lenguaje que se va a usar. En nuestro caso, C.
* `cdialect`: el estandar que vamos a estar usando. En nuestro caso vamos a usar el estandar 11 con las extensiones de gnu.

Luego de estas reglas, vamos a setear los directorios donde vamos a colocar los binarios:

```lua
    targetdir "%{wks.location}/output/bin/%{prj.name}%{cfg.architecture}/%{cfg.buildcfg}"
    objdir "%{wks.location}/output/obj/%{prj.name}%{cfg.architecture}/%{cfg.buildcfg}"
```

* `targetdir`: setea el directorio donde se va a colocar el binario o la librería resultante del proceso de compilación.
* `objdir`: setea el directorio donde se van a colocar los archivos intermedios, como archivos .o.

En estas reglas, estamos usando una serie de tokens que nos permiten acceder a algunas datos del proyecto. Hay una lista completa del significado de estos tokens en [esta página](https://github.com/premake/premake-core/wiki/Tokens). Por el momento, vamos a ver qué es lo que hacen los tokens que estamos utilizando:

* `%{wks.location}`: contiene el directorio del workspace.
* `%{prj.name}`: contiene el nombre del proyecto.
* `%{cfg.architecture}`: contiene la arquitectura que se seteo para el workspace actual.
* `%{cfg.buildcfg}`: se reemplaza por la configuración activa del proyecto. Es decir que, si compilamos en **Debug**, este token se reemplaza por **Debug**.

Las siguientes reglas setean tanto los directorios de inclusión como los archivos que se van a usar para compilar el proyecto:

```lua
    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/utils/include/utils.h",
        "%{wks.location}/utils/include/conn.h",
        "%{wks.location}/utils/src/conn.c",
    }
```

* `includedirs`: lista de directorios donde se buscaran los archivos .h
* `files`: lista de archivos que se van a utilizar para compilar.

Si bien este proyecto tiene pocos archivos, sería poco practico estar enlistando cada uno de los archivos que se quiere compilar. Por suerte, se pueden utilizar wildcards para especificar cualquier archivo. Por ejemplo:

```lua
    files {
        "%{wks.location}/utils/include/*.h",
        "%{wks.location}/utils/src/*.c",
    }
```

Esto incluirá automaticamente cualquier archivo terminado con .h en el directorio include y .c en el directorio src. Existe tambien otra forma de agregar cualquier archivo dentro del directorio especificado y en los subdirectorios:

```lua
    files {
        "%{wks.location}/utils/include/**.h",
        "%{wks.location}/utils/src/**.c",
    }
```

Con esto, agregamos cualquier archivo .h que se encuentre en el directorio include y cualquier otro subdirectorio dentro de include. Y de igual forma, para src.

Finalmente, especificaremos distintas opciones dependiendo de la configuración en la que se este compilando:

```lua
    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"
```

* `filter`: regla que nos permite aplicar determinadas reglas dependiendo del filtro especificado. En nuestro caso, vamos a agregar reglas dependiendo de la configuración que se este usando.

* `symbol`: especifica si se incluiran los simbolos de depuración en el binario.
* `warnings`: especifica el nivel de warnings. "extra" setea el nivel de warnings en el maximo nivel.
* `buildoptions`: permite especificar flags de compilación para el compilador. Vamos a agergar el flag -g3 para poder depurar macros.
* `optimize`: setea el nivel de optimización.
* `fatalwarnings`: especifica que warnings se tratan como errores. En este caso, todos los warnings serán tratados como errores.

Estos serían todas las reglas necesarias para el proyecto `utils`. La configuración completa quedaría de la siguiente forma:

```lua
project "utils"
    kind "StaticLib"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/utils/include/**.h",
        "%{wks.location}/utils/src/**.c",
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"
```

La ventaja que tenemos ahora es que, salvo algunas reglas, podemos copiar y pegar la misma configuración que usamos para `utils` en `server` y `client`:

```lua
project "server"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/server/src/**.c"
    }

    links {
        "utils"
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"

project "client"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/client/src/**.c"
    }

    links {
        "utils"
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"
```

Los cambios que hicimos fueron:
* En `project` colocamos el nombre del proyecto correspondiente.
* `kind` es seteado a ConsoleApp tanto en `server` como en `client`.
* En files, especificamos la ruta correspondiente donde buscamos los archivos a compilar para cada proyecto.

Ademas de agregar una regla mas:
* `link`: especificamos que librerías vincular al proyecto. En caso de que la librería sea libxxx.a o libxxx.so se pueden obviar tanto los prefijos lib como la extensión. Y en caso de que se este linkeando con algun proyecto dentro del mismo workspace, bastara con poner solamente el nombre del proyecto. En nuestor caso, `utils`

El script completo debería quedar de la siguiente manera:

```lua
workspace "premake-example"
    configurations { "Debug", "Release" }
    architecture "x86"

project "utils"
    kind "StaticLib"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/utils/include/**.h",
        "%{wks.location}/utils/src/**.c",
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"

project "server"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/server/src/**.c"
    }

    links {
        "utils"
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"

project "client"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/client/src/**.c"
    }

    links {
        "utils"
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"
```

## Compilación

Ahora que tenemos listo nuestro script de premake, vamos a ejecutar premake. Parado desde el directorio principal del workspace hacemos:

`premake/x86/premake5 gmake2`

Esto va a generar los makefiles necesarios para compilar los distintos proyectos. El directorio se debería ver de la siguiente manera:

```
    workspace
        |
        |__ client
        |      |__src
        |          |__ client.c
        |
        |__ server
        |      |__src
        |          |__ server.c
        |
        |__ utils
        |      |__include
        |      |    |__ conn.h
        |      |    |__ utils.h
        |      |
        |      |__src
        |          |__ conn.c
        |
        |__ premake
        |      |__ x86
        |           |_ premake5
        |
        |__ Makefile
        |
        |__ client.make
        |
        |__ server.make
        |
        |__ utils.make
```

Ahora simplemente ejecutamos make para compilar los proyectos:

`make`

Por defecto, el proyecto se compila en **Debug**. Si quisieramos ejecutar en **Release**:

`make config=release`

Al compilar, se van a generar los binarios en los directorios que especificamos en las reglas `targetdir` y `objdir`. El directorio (dependiendo de la configuración en la que compilemos) podría quedar de la siguiente manera:


```
    workspace
        |
        |__ outputs
        |      |__ bin
        |      |    |__ client
        |      |    |      |__ x86
        |      |    |           |__ Debug
        |      |    |           |     |__ client
        |      |    |           |
        |      |    |           |__ Release
        |      |    |                 |__ client
        |      |    |
        |      |    |__ server
        |      |    |      |__ x86
        |      |    |           |__ Debug
        |      |    |           |     |__ server
        |      |    |           |
        |      |    |           |__ Release
        |      |    |                 |__ server
        |      |    |
        |      |    |__ utils
        |      |           |__ x86
        |      |                |__ Debug
        |      |                |     |__ libutils.a
        |      |                |
        |      |                |__ Release
        |      |                      |__ libutils.a
        |      |    
        |      |
        |      |__ obj
        |           |__ client
        |           |      |__ x86
        |           |           |__ Debug
        |           |           |     |__ main.d
        |           |           |     |
        |           |           |     |__ main.o
        |           |           |
        |           |           |__ Release
        |           |                 |__ main.d
        |           |                 |
        |           |                 |__ main.o
        |           |
        |           |__ server
        |           |      |__ x86
        |           |           |__ Debug
        |           |           |     |__ main.d
        |           |           |     |
        |           |           |     |__ main.o
        |           |           |
        |           |           |__ Release
        |           |                 |__ main.d
        |           |                 |
        |           |                 |__ main.o
        |           |
        |           |__ utils
        |                  |__ x86
        |                       |__ Debug
        |                       |     |__ conn.d
        |                       |     |
        |                       |     |__ conn.o
        |                       |
        |                       |__ Release
        |                             |__ conn.d
        |                             |
        |                             |__ conn.o
        |
        |__ client
        |      |__src
        |          |__ client.c
        |
        |__ server
        |      |__src
        |          |__ server.c
        |
        |__ utils
        |      |__include
        |      |    |__ conn.h
        |      |    |__ utils.h
        |      |
        |      |__src
        |          |__ conn.c
        |
        |__ premake
        |      |__ x86
        |           |_ premake5
        |
        |__ Makefile
        |
        |__ client.make
        |
        |__ server.make
        |
        |__ utils.make
```

Y eso sería todo. Para probar que todo funcione simplemente ejecutamos los programas que se encuentran en los directorios de salida.

Aclaración: dado que Premake genera los Makefiles, cada vez que se agregue un archivo nuevo dentro de algun proyecto, se debe ejecutar Premake nuevamente para que el nuevo archivo sea incluido dentro del Makefile.

## Usando Eclipse
Si bien esto no tiene mucho que ver especificamente con Premake, vamos a ver como hacer para usar Eclipse junto con Premake. Básicamente importamos los proyectos como **proyectos de Makefile** de la siguiente manera:

* Dentro del IDE, hacemos click derecho sobre el workspace > Importar:
![ImportImg](img/1.jpg)

* Luego seleccionamos en **C/C++ > Existing Code as Makefile Project**:
![MakefileImg](img/2.jpg)

* Buscamos el directorio dentro del workspace de uno de nuestros proyectos. En mi caso, voy a seleccionar el de utils. En lenguaje seleccionamos C y en los ToolChain seleccionamos Linux GCC:
![UtilsImg](img/3.jpg)

* Una vez hecho esto, ya deberíamos ver nuestro proyecto en el workspace. Probamos que todo haya salido correctamente. Hacemos click derecho sobre el proyecto y luego en **Build Project**:
![BuildImg](img/4.jpg)

* Esto nos deberia mostrar los pasos de compilación como se muestra en la siguiente imagen. Si no se llegaran a ver, pueden intentar hacer click derecho sobre el proyecto > **Clear Project** y luego buildear nuevo siguiendo el paso anterior:
![CompileImg](img/5.jpg)

Y listo. Debemos repetir estos pasos de la misma manera para los otros tres proyectos.
Aclaración: de la misma manera que hay que correr Premake cuando agregamos nuevos archivos al proyecto con los Makefiles, tambien debemos hacerlo cuando agregamos archivos desde Eclipse (ya que Eclipse lo único que hace, en esencia, es correr los Makefiles).

## TL;DR

Con todo lo anterior ya estamos en condiciones de setear cualquier workspace o proyecto de C usando Premake. Lo que sigue a continuación son algunas topicos extra que nos pueden ser de utilidad para organizar mejor nuestros scripts.

## Uso de variables

Como Premake esta escrito en lua, nada nos impide poder usar todas las características del lenguaje. No me voy a expandir demasiado en temas muy especificos del lenguaje, pero una de las cosas de las que si podemos sacar probecho son las variables.

Anteriormente, seteamos los directorios de salida como:

`<workspace>/outputs/bin/<project-name>/<architecture>/<configuration>`

Ahora, imaginemos que nosotros quisieramos cambiar la ruta donde colocar nuestros binario mas al estilo Eclipse:

`<workspace>/<project-name>/<configuration>`

Deberíamos ir recorriendo cada proyecto e ir cambiando cada uno de los directorios. Una alternativa seria setear alguna variable en la que podamos colocar los directorios comunes a todos los proyectos. Por ejemplo:

```lua
workspace "premake-example"
    configurations { "Debug", "Release" }
    architecture "x86"

    --Common Locations--
    --------------------

    CommonLocations = { }
    CommonLocations["TargetDir"] = "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/%{cfg.buildcfg}"
    CommonLocations["ObjDir"]    = "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/%{cfg.buildcfg}"

    .
    .
    .

```

Luego, podriamos acceder a esta variable de manera similar a como lo hacemos con los tokens predefinidos de Premake:

```lua
project "utils"
    kind "StaticLib"
    language "C"
    cdialect "GNU11"

    targetdir "%{CommonLocations.TargetDir}"
    objdir    "%{CommonLocations.ObjDir}"
    .
    .
    .

project "server"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{CommonLocations.TargetDir}"
    objdir    "%{CommonLocations.ObjDir}"
    .
    .
    .

project "client"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{CommonLocations.TargetDir}"
    objdir    "%{CommonLocations.ObjDir}"
    .
    .
    .

```

Así, si en el futuro quisieramos cambiar el directorio de salida de nuestros binarios, simplemente modificamos la variable que contiene los directorios de salida:

```lua
workspace "premake-example"
    configurations { "Debug", "Release" }
    architecture "x86"

    --Common Locations--
    --------------------

    CommonLocations = { }
    CommonLocations["TargetDir"] = "%{wks.location}/%{prj.name}/%{cfg.buildcfg}"
    CommonLocations["ObjDir"]    = "%{wks.location}/%{prj.name}/%{cfg.buildcfg}"

    .
    .
    .
```

## Multiples scripts

Hasta ahora, solo estuvimos colocando todas las reglas para cada proyecto en un único script. Pero ¿No seria mas facil si pudieramos separar cada configuración en diferentes scripts?

Esto se puede lograr de la siguiente manera. Ademas del script principal, vamos a crear tres scripts mas. Uno en el directorio principal de cada proyecto:

```
    workspace
        |
        |__ client
        |      |__src
        |      |   |__ client.c
        |      |
        |      |__ client.lua
        |
        |__ server
        |      |__src
        |      |   |__ server.c
        |      |
        |      |__ server.lua
        |
        |__ utils
        |      |__include
        |      |    |__ conn.h
        |      |    |__ utils.h
        |      |
        |      |__src
        |      |   |__ conn.c
        |      |
        |      |__ utils.lua
        |
        |__ premake
               |__ x86
                    |_ premake5

```

En estos scripts vamos a copiar la configuración de cada uno de los proyectos:

utils.lua
```lua
project "utils"
    kind "StaticLib"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/utils/include/**.h",
        "%{wks.location}/utils/src/**.c",
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"
```

server.lua
```lua
project "server"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/server/src/**.c"
    }

    links {
        "utils"
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"
```

client.lua

```lua
project "client"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/client/src/**.c"
    }

    links {
        "utils"
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"
```

Y en nuestro script principal, incluimos estos scripts de la siguiente forma:

premake5.lua
```lua
workspace "premake-example"
    configurations { "Debug", "Release" }
    architecture "x86"

    --Common Locations--
    --------------------

    CommonLocations = { }
    CommonLocations["TargetDir"] = "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/%{cfg.buildcfg}"
    CommonLocations["ObjDir"]    = "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/%{cfg.buildcfg}"

    include("utils/utils.lua")
    include("server/server.lua")
    include("client/client.lua")
```

De esta manera no tenemos la configuración de todos los proyectos metidos en un solo script gigante. Y podemos ir a buscar las configuraciones de un proyecto en particular mucho mas rápido.

## Creando nuevas opciones

Premake tambien permite crear opciones personalizadas que podemos pasarle a la hora de ejecutar los scripts. En nuestro caso, lo que queremos hacer es decirle a premake bajo que arquitectura estamos compilando los proyectos. De esta manera, no tenemos que reescribir el script para compilar para 32 bits o para 64 bits. Lo hacemos de la siguiente manera:

premake5.lua
```lua
newoption {
    trigger = "arch",
    description = "Set the current target architecture",
    default = "x86",
    allowed = {
        { "x86", "32 bit target architecture" },
        { "x86_64", "64 bit target architecture" },
    }
}

workspace "premake-example"
    configurations { "Debug", "Release" }
    architecture (_OPTIONS["arch"])
    .
    .
    .

```

`newoption` nos permite crear opciones extra que podemos pasarle a Premake cuando lo invocamos:

* `trigger`: el nombre de la opción. Esta se debe especificar luego cuando se ejecuta premake como `--<option>`. En nuestro caso, `--arch`
* `description`: una descripción del flag. Esta puede aparecer luego si hacemos `premake5 --help`.
* `default`: valor por defecto en caso de que no hayamos seteado la opción.
* `allowed`: todas las opciones disponibles. Estas se pueden especificar a la hora de ejecutar Premake haciendo `premake5 --<option>=<value>`. Un ejemplo sería `premake5 --arch=x86_64`.

Una vez creada esta nueva opción, podemos setear la arquitectura a la que queremos 
compilar haciendo:

`premake/x86/premake5 --arch=x86 gmake2` | `premake/x86_64/premake5 --arch=x86_64 gmake2`
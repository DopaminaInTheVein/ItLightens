# Instrucciones GIT

## Pasos previos

### Programario

Git:
> https://git-scm.com/

Git ficheros grandes
> https://git-lfs.github.com/

### Descarga del repositorio

Primero toca descargar el contenido del repositorio. Pasos:

1. Instalar normalmente el git y el git-lfs.
2. Tener una carpeta nueva y vacia donde mejor vaya.
3. Abrir con el explorador, tal cual, botón derecho en pleno centro y darle a "Git Gui"
4. Clicar en "Clone Existing Repository"
5. En "Source Location" copiar "https://github.com/DopaminaInTheVein/ItLightens.git" o en su defecto, "https://github.com/DopaminaInTheVein/ItLightensArt.git"
6. En "Target Directory" buscar la carpeta nueva y añadir un "/itlightens" (o cualquier otro nombre que os guste más)
7. Clic en "Clone" y esperar un rato a que se descargue.
8. Cuando termine salir de la nueva ventana que aparece

### Configurar ficheros pesados
* Repetir paso 4 anterior con "Git Bash" dentro de la subcarpeta que se crea.
* En el terminal que se abre, escribir y ejecutar:

> git lfs install

* Para cada tipo de fichero que vaya a pesar bastante (por ejemplo el escenario.max):

> git lfs track "*.max"
	
### En caso de clonar el repositorio de programación

En el terminal ejecutar un:
> git checkout dev

## Organización

Ahora deberiais tener dentro todas las carpetas ("Engine", "bin", "max") en la subcarpeta itlightens.

Dentro de "max" están todos los ".max" y maxscripts para tools que se han montado. Ahí podeis tener todos los max juntitos y ordenados.
En la carpeta "bin" estará el ejecutable y dentro de "bin/data" estarán todas las texturas, etc.
Usando las tools de max proporcionadas deberían llenar las carpetas de "bin/data", menos las texturas.

Con esto debería poderse exportar todo y probarse con el último ejecutable disponible.

## Comandos terminal GIT

Antes de empezar seria bueno que por el grupo de whatsapp que sea conveniente os reserveis los ficheros ya existentes que se deban cambiar, para evitar modificar en paralelo el mismo.
Ahora se especifican comandos para el terminal:

### Comprobar estado repositorio
Este comando os dirà que cambios o ficheros nuevos teneis pendientes para subir al repositorio.
> git status

### Actualizar repositorio
Si no teneis cambios pendientes de subir, podreis actualizar sin problemas.
> git pull origin dev

### Subir cambios
* Si estais seguros de que todo lo que aparece en el "status" se debe subir (el punto final es importante):

> git add --all .

	* Si no se deben subir todos pongo una ruta de ejemplo para fichero concreto:

> git add bin/data/scenes/ms3.xml

	* Pongo una ruta de ejemplo para todos los ficheros nuevos y cambiados de una carpeta:

> git add bin/data/scenes

* Una vez añadidos, con un mensaje de ejemplo entre comillas simples (recomendado algo claro y descriptivo por si hay que buscarlo):

> git commit -m 'Descripción del contenido de la subida'

* Si os dejais de añadir algo y os dais cuenta o quereis separar las subidas por organización, repetir el paso 1-2 por cada parte.
* Mandar los cambios al repositorio (os pedirá usuario y contraseña de github "https://github.com/orgs/DopaminaInTheVein/people"):

> git push origin dev

* "Liberar" el/los fichero/s reservado/s. (Quizás un excell de google groups os puede ayudar ahí)


Error D3D11_CREATE_DEVICE_DEBUG:
--------------------
Ejecutar en cmd con derechos de admin:
Dism /online /add-capability /capabilityname:Tools.Graphics.DirectX~~~~0.0.1.0
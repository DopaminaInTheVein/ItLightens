--Directorio Scripts
scriptsDir = getDir #scripts + "\\"

--Personal config file (where is data path, etc.)
filein (scriptsDir+"config.ms") 

--Funciones MaxScript
mcvScriptsDir = scriptsDir + "mcv\\"
filein (mcvScriptsDir+"export_mesh.ms") 
filein (mcvScriptsDir+"object.ms") 
filein (mcvScriptsDir+"export_scene.ms")

--Macros MaxScript
mcvMacrosDir = mcvScriptsDir + "\\macros\\"

getMacros = getFiles (mcvMacrosDir + "*.mcr")
for f in getMacros do
(
	fileIn f
)
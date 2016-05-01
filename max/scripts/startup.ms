-- PATHS GENERALES (Configurar para cada usuario)
--------------------------------------------------------------------------------------------------------------------
data_dir = "E:GIT\\ItLightens\\bin\\data\\"
max_dir = "E:\\GIT\\ItLightens\\max\\"
--------------------------------------------------------------------------------------------------------------------

filein (max_dir + "scripts\\" + "project_config.ms")

fn startUp = (
	-- Folders
	scriptsDir = (max_dir + "scripts\\")
	mcvScriptsDir = scriptsDir + "mcv\\"
	mcvResources = mcvScriptsDir + "\\resources\\"
	mcvTools = mcvScriptsDir + "\\tools\\"
	mcvParsingDir = mcvScriptsDir + "\\parsing\\"
	mcvMacrosDir = mcvScriptsDir + "\\macros\\"
	
	--Utils file
	filein (mcvScriptsDir +"utils.ms") 		
	
	--Resources export (meshes, materials, skel, ...)
	getResources = getFiles (mcvResources + "*.ms")
	for f in getResources do
	(
		fileIn f
	)
	filein (mcvScriptsDir+"resources.ms") 
	
	--Funciones MaxScript
	filein (mcvScriptsDir+"object.ms") 
	--filein (mcvScriptsDir+"export_scene.ms")
	
	-- Rigid Anim
	fileIn (mcvScriptsDir + "RigidAnimExp.ms")

	
	--Tools MaxScript
	getTools = getFiles (mcvTools + "*.ms")
	for f in getTools do
	(
		fileIn f
	)

	-- Parsing
	fileIn (mcvParsingDir + "parsing.ms")
	
	--Macros MaxScript
	getMacros = getFiles (mcvMacrosDir + "*.mcr")
	for f in getMacros do
	(
		fileIn f
	)
)

startUp()
startUp()
clearListener()
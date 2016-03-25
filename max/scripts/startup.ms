fn startUp = (
	--Directorio Scripts
	scriptsDir = getDir #scripts + "\\"

	--Personal config file (where is data path, etc.)
	filein (scriptsDir+"config.ms") 
	
	--Utils file
	mcvScriptsDir = scriptsDir + "mcv\\"
	filein (mcvScriptsDir+"utils.ms") 		
	
	--Resources export (meshes, materials, skel, ...)
	mcvResources = mcvScriptsDir + "\\resources\\"
	getResources = getFiles (mcvResources + "*.ms")
	for f in getResources do
	(
		fileIn f
	)
	filein (mcvScriptsDir+"resources.ms") 
	
	
	
	--Funciones MaxScript
	filein (mcvScriptsDir+"object.ms") 
	--filein (mcvScriptsDir+"export_scene.ms")
	
	--Tools MaxScript
	mcvTools = mcvScriptsDir + "\\tools\\"
	getTools = getFiles (mcvTools + "*.ms")
	for f in getTools do
	(
		fileIn f
	)

	-- Parsing
	mcvParsingDir = mcvScriptsDir + "\\parsing\\"
	getParsing = getFiles (mcvParsingDir + "*.ms")
	for f in getParsing do
	(
		fileIn f
	)

	--Macros MaxScript
	mcvMacrosDir = mcvScriptsDir + "\\macros\\"

	getMacros = getFiles (mcvMacrosDir + "*.mcr")
	for f in getMacros do
	(
		fileIn f
	)
)

startUp()
clearListener()
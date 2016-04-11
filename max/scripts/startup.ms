fn startUp = (
	-- Folders
	scriptsDir = (getDir #scripts) + "\\"
	mcvScriptsDir = scriptsDir + "mcv\\"
	mcvResources = mcvScriptsDir + "\\resources\\"
	mcvTools = mcvScriptsDir + "\\tools\\"
	mcvParsingDir = mcvScriptsDir + "\\parsing\\"
	mcvMacrosDir = mcvScriptsDir + "\\macros\\"
	
	--Personal config file (where is data path, etc.)
	filein (scriptsDir+"config.ms") 
	
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
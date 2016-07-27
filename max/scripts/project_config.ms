-- PATHS RELATIVOS a los anteriores (por lo general no tocar)
--------------------------------------------------------------------------------------------------------------------
--ScriptsDir
if dev_scripts_dir == undefined then
	scripts_dir = max_dir + "scripts\\"
else
	scripts_dir = dev_scripts_dir
end
--Data
meshes_dir = data_dir + "meshes\\"
staticMeshes_dir = data_dir + "static_meshes\\"
scenes_dir = data_dir + "scenes\\"
materials_dir = data_dir + "materials\\"
objMax_dir = max_dir + "maxObjects\\"
helperMax_dir = max_dir + "maxHelpers\\"
lua_dir = data_dir + "lua_scripts\\"
--Scripts
scriptsDir = (getDir #scripts) + "\\"
mcvScriptsDir = scriptsDir + "mcv\\"
mcvResources = mcvScriptsDir + "\\resources\\"
mcvTools = mcvScriptsDir + "\\tools\\"
mcvParsingDir = mcvScriptsDir + "\\parsing\\"
mcvMacrosDir = mcvScriptsDir + "\\macros\\"
--------------------------------------------------------------------------------------------------------------------

-- EXTENSIONES
--------------------------------------------------------------------------------------------------------------------
mesh_ext = ".mesh"
staticMesh_ext = ".static_mesh"
skel_ext = ".skeleton"
--------------------------------------------------------------------------------------------------------------------

fn startUp = (
	-- Folders
	mcvScriptsDir = scripts_dir + "mcv\\"
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

	-- Material Manager
	fileIn (mcvScriptsDir + "materialManager.ms")
	
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
	
	--assign_default_room_to_all()
)

startUp()
startUp()
clearListener()
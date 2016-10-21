try(DestroyDialog RL_Meshes)catch()
rollout RL_Meshes "IT Lightens!" width:296 height:349
(
	local selectedObj = undefined
-- Before simplify
/* 	label lblTitle "MESHES ( Export and Set )" pos:[79,12] width:137 height:16
* 	GroupBox grpSelObj "Select Object" pos:[16,39] width:232 height:48
* 	label lblSelObj "Selected Object:" pos:[32,59] width:88 height:16
* 	pickbutton pckObj "-- None --" pos:[128,59] width:112 height:16
* 	button btnSetMesh "Export Mesh" pos:[78,191] width:135 height:16
* 	GroupBox grpSetMesh "Export Mesh" pos:[15,99] width:259 height:143
* 
* 	label lblMesh "Mesh:" pos:[63,145] width:34 height:16
* 	label lblStaticMesh "Static Mesh:" pos:[32,164] width:66 height:16
* 	edittext txtMesh "" pos:[101,143] width:157 height:16
* 	edittext txtStaticMesh "" pos:[100,164] width:157 height:16
* 	button btnHelp "Help" pos:[79,214] width:135 height:16
* 	
* 	
* 	
* 	GroupBox grpCopy "Copy" pos:[16,256] width:259 height:89
* 	button btnCopy "Set this mesh to selected objects" pos:[30,275] width:230 height:16
* 	label lblCopy "Para asignar la misma mesh a otros objetos (sólo actualiza la info para exportar nivel, no cambiará la apariencia en max)" pos:[27,296] width:237 height:46
* 	GroupBox grpAll "Export All Selected" pos:[15,359] width:259 height:269
* 	button btnExportSelectedNew "Only New Meshes" pos:[29,476] width:114 height:16
* 	button btnRexport "Only Re-export" pos:[150,476] width:112 height:16
* 	button btnExportSelected "Export New and Reexport Old" pos:[29,454] width:234 height:16
* 	button btnForceNew "Force all as new (caution!)" pos:[75,600] width:145 height:16
* 	label lbl11 "BASIC" pos:[133,431] width:29 height:17
* 	label lbl12 "ADVANCED" pos:[119,578] width:58 height:17
* 	label lbl20 "Las nuevas meshes (objetos sin mesh asignada), se guardarán con el mismo nombre que el objeto. Las reexportadas con el mismo nombre con el que ya se exportaron" pos:[26,500] width:237 height:61
* 	label lblFolder "Folder" pos:[34,381] width:42 height:20
* 	edittext txtFolder "" pos:[71,379] width:192 height:18
*/
	label lblTitle "MESHES ( Export and Set )" pos:[79,12] width:137 height:16

	
	
	
	GroupBox grpAll "Export All Selected" pos:[18,36] width:259 height:304
	button btnExportSelectedNew "Only New Meshes" pos:[29,190] width:114 height:16
	button btnRexport "Only Re-export" pos:[150,190] width:112 height:16
	button btnExportSelected "Export New and Reexport Old" pos:[29,168] width:234 height:16
	button btnForceNew "Force all as new (caution!)" pos:[75,314] width:145 height:16
	label lbl11 "BASIC" pos:[133,145] width:29 height:17
	label lbl12 "ADVANCED" pos:[119,292] width:58 height:17
	label lbl20 "Las nuevas meshes (objetos sin mesh asignada), se guardarán con el mismo nombre que el objeto. Las reexportadas con el mismo nombre con el que ya se exportaron" pos:[26,214] width:237 height:61
	label lblFolder "Folder" pos:[37,60] width:42 height:20
	edittext txtFolder "" pos:[74,58] width:192 height:18

	--Funcs
	fn checkFolder folder needFolder = (
		if needFolder then (
			if folder == "" then (
				--local yes = queryBox "No se ha seleccionado carpeta. ¿Exportar en raíz?"
				--if not yes then (return false)
				messagebox "¡Debes indicar una carpeta!"
				return false
			) else (
				local createMeshDir = false
				local createMatDir = false
				local createStMeshDir = false
			
				local meshPath = meshes_dir + folder + "\\"
				local dirMeshes = getDirectories (meshes_dir + folder)
				if dirMeshes.count == 0 then (
					local yes = queryBox ("Directorio nuevo. ¿Crear directorio [" + meshPath+ "]?")
					if yes then (
						createMeshDir = true
					) else (
						return false
					)
				)

				local matPath = materials_dir + folder + "\\"
				local dirMats = getDirectories (materials_dir + folder)
				if dirMats.count == 0 then (
					local yes = queryBox ("Directorio nuevo. ¿Crear directorio [" + matPath+ "]?")
					if yes then (
						createMatDir = true
					) else (
						return false
					)
				)
				
				
				local stMeshPath = staticMeshes_dir + folder + "\\"
				local dirStMeshes = getDirectories (staticMeshes_dir + folder)
				if dirStMeshes.count == 0 then (
					local yes = queryBox ("Directorio nuevo. ¿Crear directorio [" + stMeshPath + "]?")
					if yes then (
						createStMeshDir = true
					) else (
						return false
					)
				)
				
				if createMeshDir then (
					if not (makeDir meshPath) then (
						messagebox ("Cannot create [" + meshes_dir + folder + "\\]")
						return false
					)
				)

				if createMatDir then (
					if not (makeDir matPath) then (
						messagebox ("Cannot create [" + materials_dir + folder + "\\]")
						return false
					)
				)
				
				if createStMeshDir then (
					if not (makeDir stMeshPath) then (
						messagebox ("Cannot create [" + stMeshPath + "\\]")
						return false
					)
				)
			)
		) else (
			if folder != "" then (
				local yes = queryBox "En modo reexportar se ignora el directorio indicado. ¿Continuar?"
				if not yes then (return false)
			)
		)
		return true
	) 
	
	checkbox chkTextures "Export textures" pos:[154,80] width:108 height:15
	checkbox chkMoveResources "Delete materials on root" pos:[34,79] width:99 height:37
	checkbox chkMesh "Only mesh" pos:[32,114] width:108 height:15
	on RL_Meshes open do
	(
		local folder_init = (filterstring maxFileName ".")[1]
		if folder_init != undefined then (txtFolder.text = folder_init)
		if _is_ui_scene() then (
			txtFolder.text = "ui/" + txtFolder.text
		)
	)
	
	fn getResourcesConfig = (
		tr = TResources()
		tr.copy_textures = chkTextures.state
		tr.move_resources = chkMoveResources.state
		tr.only_mesh = chkMesh.state
		return tr
	)
	
	on btnExportSelectedNew pressed do
	(
		tr = getResourcesConfig()
		
		local folder =  trimRight (trimLeft txtFolder.text)
		tr.folder = folder
		
		if (checkFolder folder true) then (
			tr.exportSelectedNew()
			messageBox "Operación realizada OK"
		)
	)
	on btnRexport pressed do
	(
		tr = getResourcesConfig()
		
		local folder =  trimRight (trimLeft txtFolder.text)
		tr.folder = folder
		
		if (checkFolder folder false) then (
			tr.rexport()
			messageBox "Operación realizada OK"
		)
	)
	on btnExportSelected pressed do
	(
		tr = getResourcesConfig()
	
		local folder =  trimRight (trimLeft txtFolder.text)
		tr.folder = folder
	
		if (checkFolder folder true) then (		
			tr.exportSelected()
			messageBox "Operación realizada OK"
		)
	)
	on btnForceNew pressed do
	(
		tr = getResourcesConfig()
	
		local folder =  trimRight (trimLeft txtFolder.text)
		tr.folder =	folder
		
		if (checkFolder folder true) then (
			tr.exportSelectedForceNew()
			if tr.error == "" then ( 
				messageBox ("Operación realizada OK\n" + (_list_objs as string) )
			) else (
				local txt = (tr.error + "\n-------------\n" + (_list_objs as string))
				print txt
				messagebox txt
			)
		)
	)
)
CreateDialog RL_Meshes

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------

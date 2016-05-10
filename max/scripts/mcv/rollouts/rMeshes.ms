try(DestroyDialog RL_Meshes)catch()
rollout RL_Meshes "IT Lightens!" width:296 height:615
(
	local selectedObj = undefined

	label lblTitle "MESHES ( Export and Set )" pos:[79,12] width:137 height:16
	GroupBox grpSelObj "Select Object" pos:[16,39] width:232 height:48
	label lblSelObj "Selected Object:" pos:[32,59] width:88 height:16
	pickbutton pckObj "-- None --" pos:[128,59] width:112 height:16
	button btnSetMesh "Export Mesh" pos:[78,191] width:135 height:16
	GroupBox grpSetMesh "Export Mesh" pos:[15,99] width:259 height:143

	label lblMesh "Mesh:" pos:[63,145] width:34 height:16
	label lblStaticMesh "Static Mesh:" pos:[32,164] width:66 height:16
	edittext txtMesh "" pos:[101,143] width:157 height:16
	edittext txtStaticMesh "" pos:[100,164] width:157 height:16
	button btnHelp "Help" pos:[79,214] width:135 height:16
	
	
	
	GroupBox grpCopy "Copy" pos:[16,256] width:259 height:89
	button btnCopy "Set this mesh to selected objects" pos:[30,275] width:230 height:16
	label lblCopy "Para asignar la misma mesh a otros objetos (sólo actualiza la info para exportar nivel, no cambiará la apariencia en max)" pos:[27,296] width:237 height:46
	GroupBox grpAll "Export All Selected" pos:[15,359] width:259 height:245
	button btnExportSelectedNew "Only New Meshes" pos:[29,452] width:114 height:16
	button btnRexport "Only Re-export" pos:[150,452] width:112 height:16
	button btnExportSelected "Export New and Reexport Old" pos:[29,430] width:234 height:16
	button btnForceNew "Force all as new (caution!)" pos:[75,576] width:145 height:16
	label lbl11 "BASIC" pos:[133,407] width:29 height:17
	label lbl12 "ADVANCED" pos:[119,554] width:58 height:17
	label lbl20 "Las nuevas meshes (objetos sin mesh asignada), se guardarán con el mismo nombre que el objeto. Las reexportadas con el mismo nombre con el que ya se exportaron" pos:[26,476] width:237 height:61
	label lblFolder "Folder" pos:[34,381] width:42 height:20
	edittext txtFolder "" pos:[71,379] width:192 height:18
	
	--Funcs
	fn checkFolder folder needFolder = (
		if needFolder then (
			if folder == "" then (
				local yes = queryBox "No se ha seleccionado carpeta. ¿Exportar en raíz?"
				if not yes then (return false)
			) else (
				local createMeshDir = false
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
	
	label lblFolderUp "Folder:" pos:[63,120] width:34 height:20
	edittext txtFolderUp "" pos:[101,119] width:159 height:18
	on pckObj picked obj do
	(
		pckObj.text = obj.name
		selectedObj = obj
	)
	on btnSetMesh pressed do
	(
		local err = ""
		local stMesh = trimRight (trimLeft txtStaticMesh.text)
		local stMeshPath = staticMeshes_dir + stMesh + staticMesh_ext
		local _mesh = trimRight (trimLeft txtMesh.text)
		local meshPath = meshes_dir + _mesh + mesh_ext
		local newMeshCreated = false
		try (
	
			if selectedObj == undefined do (
				throw "Debes seleccionar un objeto!!" 
			)					
			
			if stMesh == "" do (
				throw "Debes indicar la mesh!" 
			)		
	
			if _mesh == "" do (
				throw "Debes indicar la staticMesh!" 
			)
			local folder = trimRight (trimLeft txtFolderUp.text)
			
			if not (checkFolder folder true) then (
				return #abort
			)
			if folder != "" then (
				_mesh = folder + "/" + _mesh
				stMesh = folder + "/" + stMesh
				stMeshPath = staticMeshes_dir + stMesh + staticMesh_ext
			)
			-- Static Mesh existe
			if (doesFileExist stMeshPath) then (
				local resp = (queryBox "Esta staticMesh ya existe y se sobreescribirá. ¿Continuar?")
				if (not resp) do (
					return #abort
				)			
			)
					
			-- Mesh existe
			if (doesFileExist meshPath) then (
				local resp = (queryBox "Esta mesh ya existe y se sobreescribirá. ¿Continuar?")
				if (not resp) do (
					return #abort
				)
			)
			
			-- Exportar Static Mesh
			try (
				setUserProp selectedObj "mesh" _mesh
				setUserProp selectedObj "staticMesh" stMesh
				
				tr = TResources();
				tr.folder = folder
				tr.exportStaticMesh stMeshPath #(selectedObj)
				messageBox "Exportado correctamente"
			) catch (
				err = "Error exportando static_mesh!"
				throw()
			)
			
		) catch (
			err += "\n***\n"+getCurrentException()+"\n***\n"
			try (
				if newMeshCreated do (
					messageBox "Borro Mesh"
					deleteFile meshPath
				)
			) catch ( )
			messageBox err
		)
	)
	on btnHelp pressed do
	(
		local help  = "Ayuda para artistas\n\n"
		help += "Mesh: nombre del fichero .mesh (guarda la geometría del objeto)\n"
		help += "Static Mesh: nombre del fichero .static_mesh (referencia a .mesh + info adicional de render)\n"
	
		help += "\n¿Ésto qué #!@?=%& hace? \n"
		help += "Al pulsar Export Mesh, se exportará los ficheros de las meshes, materiales, etc. del objeto en subcarpetas del directorio data. "
		help += "Además se guardará en las propiedades de este objeto (Object properties-> User defined), el vínculo a la mesh exportada. "
		help += "(Esta información la utiliza el exportador del nivel, ¡procura que todos los objetos la tengan!)"
		
		help += "\nRecomedaciones:\n"
		help += "1) Utilizar mismo nombre para mesh que para static_mesh (a excepción de reutilizar misma geometría con distintas texturas, etc.)\n" 
		help += "2) Exportar nuevas mallas sólo desde los .max de los objetos sueltos\n(o mallas únicas de un escenario)\n"
		help += "3) Preguntad lo que sea que no entendáis antes de liarla :P"
		messageBox help
	)
	on btnCopy pressed do
	(
		rMesh = getUserProp selectedObj "mesh"
		rStMesh = getUserProp selectedObj "staticMesh"
		if rMesh == undefined then return #abort
		if rStMesh == undefined then return #abort
		if $ == undefined then return #abort
			
		if (classOf $ == ObjectSet) then objs = $ as array
		else objs = #($)
		for o in objs do(
			setUserProp o "mesh" rMesh
			setUserProp o "staticMesh" rStMesh
		)
		
		messageBox "Meshes y StaticMeshes modificadas correctamente"
	)
	on btnExportSelectedNew pressed do
	(
		tr = TResources()
		
		local folder =  trimRight (trimLeft txtFolder.text)
		tr.folder = folder
		
		if (checkFolder folder true) then (
			tr.exportSelectedNew()
			messageBox "Operación realizada OK"
		)
	)
	on btnRexport pressed do
	(
		tr = TResources()
		
		local folder =  trimRight (trimLeft txtFolder.text)
		tr.folder = folder
		
		if (checkFolder folder false) then (
			tr.rexport()
			messageBox "Operación realizada OK"
		)
	)
	on btnExportSelected pressed do
	(
		tr = TResources()
	
		local folder =  trimRight (trimLeft txtFolder.text)
		tr.folder = folder
	
		if (checkFolder folder true) then (		
			tr.exportSelected()
			messageBox "Operación realizada OK"
		)
	)
	on btnForceNew pressed do
	(
		tr = TResources()
	
		local folder =  trimRight (trimLeft txtFolder.text)
		tr.folder =	folder
		
		if (checkFolder folder true) then (
			tr.exportSelectedForceNew()
			messageBox "Operación realizada OK"
		)
	)
)
CreateDialog RL_Meshes

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------------------
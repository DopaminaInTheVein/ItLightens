fn registerVertex ht all_vtxs new_vtx = (

	-- Key is a string representation of all floats in new_vtx
	local key = with printAllElements off new_vtx as string
	
	-- Find the string in the hash map
	local idx = ht.Item key
	if idx == undefined then (
		-- Register it
		idx = ht.count + 1
		ht.add key idx			-- Bind "vertex_as_string" to 1
		append all_vtxs new_vtx	
		return idx
	)
	-- Return the generated idx
	return idx
)

-- 
fn saveBinaryMesh obj vtxs idxs groups full_path = (
	
	-- Open file
	local fs = fopen full_path "wb"
	if fs == undefined then throw ("Can't create output file " + full_path )
	
	-- Save Header
	local num_bytes_of_header = 9 * 4
	local version = 1
	local primitive_type = 2000
	local vertex_type = 1002		-- Pos, N, UV
	local bytes_per_float = 4		-- Assuming all vtxs are float
	local bytes_per_vertex = vtxs[1].count * bytes_per_float
	local bytes_per_index = if idxs.count > 65535 then 4 else 2
	local num_groups = groups.count
	local bytes_per_group = 4 * 2    -- 4 bytes/int x 2 ints (one for first_index, one for num_indices)
	
	format "Exporting mesh with % vertexs and % idxs\n" vtxs.count idxs.count
	
	WriteLong fs 0x44221100  -- Mesh magic
	WriteLong fs num_bytes_of_header
	
	WriteLong fs version
	WriteLong fs vtxs.count
	WriteLong fs idxs.count
	WriteLong fs primitive_type
	
	WriteLong fs vertex_type
	WriteLong fs bytes_per_vertex
	WriteLong fs bytes_per_index			-- 2 or 4
	WriteLong fs num_groups
	WriteLong fs 0x44222200  -- terminator Mesh header
	
	-- Save vtxs
	WriteLong fs 0x44221101  -- vtxs magic
	WriteLong fs ( bytes_per_vertex * vtxs.count )
	for v in vtxs do (
		for c in v do (			-- Each member of the vtx
			WriteFloat fs c		-- Write a float
		)
	)

	-- Save idxs
	WriteLong fs 0x44221102  -- idx magic
	WriteLong fs ( bytes_per_index * idxs.count )
	for i in idxs do (
		if bytes_per_index == 2 then (
			WriteShort fs (i-1)			-- Base 0
		) else (
			WriteLong fs (i-1)
		)
	)
	
	-- Save group info
	WriteLong fs 0x44221103  -- groups magic
	WriteLong fs ( bytes_per_group * groups.count )
	for g in groups do (
		writeLong fs g[1]
		writeLong fs g[2]
	)
	
	WriteLong fs 0x44221144  -- magic mesh end
	WriteLong fs 0
	
	FClose fs
)

fn export_mesh given_obj meshName mat_infos = (

	local outfilename = meshes_dir + meshName + mesh_ext
	
	-- Is the object valid?...
	local obj = given_obj
	if classof given_obj != Editable_Mesh then (
		obj = copy given_obj
		ConvertToMesh obj
	)
	
	-- Basic info
	local channelA = 1
	
	local world_max_to_local_mcv = (inverse obj.transform) * ( rotateXMatrix -90)
	
	-- 
	local all_vtxs = #()
	local all_idxs = #()
	local all_groups = #()
	
	local HT = dotNetObject "System.Collections.Hashtable"
	
	if not meshop.getMapSupport obj channelA then (
		throw ("Object " + given_obj.name + " does not have tex coords for channel " + channelA )
	)
	
	-- Pasearse por todos los grupos de caras
	for mat_info in mat_infos do (
		
		-- Algunos puede que esten vacios...
		if mat_info == undefined then continue
		
		local first_index_of_mat = all_idxs.count
		
		-- Para todas las face_id's del material id...
		for face_id in mat_info do (
			local pos_idx = getFace obj face_id
			local uv_A = meshop.getMapFace obj channelA face_id
			
			-- Returns an array of 3 entries, one Normal per each vertex
			-- of the face
			local world_normals = meshop.getFaceRNormals obj face_id
			
			--format "Face % %\n" face_id pos_idx
			for vtx_id in #(1,3,2) do (
				local world_pos = getVert obj pos_idx[vtx_id]
				local world_normal = world_normals[ vtx_id ]
				local pos =  world_pos * world_max_to_local_mcv
				
				-- Multiply Just the the rotation, we don't want to translate
				-- the normal directions
				local normal = world_normal * world_max_to_local_mcv.rotationpart 
				
				local uvA = meshop.getMapVert obj channelA uv_A[vtx_id]

				-- 1-uvA.y because DX11 has the origin in the top, and gl&MAX in the bottom
				local vtx = #(pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uvA.x, 1-uvA.y)
				local idx = registerVertex HT all_vtxs vtx
				append all_idxs idx
				--format "  % % => %\n" pos uvA idx
			)
		)
		
		-- El num de indices que he generado en este tipo de material
		local num_indices_in_this_mat = all_idxs.count - first_index_of_mat 
		append all_groups #(first_index_of_mat, num_indices_in_this_mat)
		
	)
	
	-- If I did a copy, it's time to delete it
	if obj != given_obj then 
		delete obj

	---------------------------------------------
	-- Save everything
	---------------------------------------------
	saveBinaryMesh given_obj all_vtxs all_idxs all_groups outfilename
	
)

--export_mesh $
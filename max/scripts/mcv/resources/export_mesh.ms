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
	try (
		-- Save Header
		local num_bytes_of_header = 9 * 4
		local version = 1
		local primitive_type = 2000
		-- Modificado 22/04/2016
		--local vertex_type = 1002		-- Pos, N, UV
		local vertex_type = 1003		-- Pos, N, UV, Tangent
		-- Fin modif --
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
				WriteShort fs (i-1)			-- save in base 0, were stored in base 1
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
	) catch (
		FClose fs
		deleteFile full_path
		throw()
	)
	FClose fs
)

--Modificado 22/04/2016 --
---------------------------------------------
-- Translated to maxscript from
-- http://www.terathon.com/code/tangent.html
---------------------------------------------
fn computeTangentSpace vtxs idxs = (
	
	local vertexCount = vtxs.count
	local triangleCount = idxs.count / 3
		
    local tan1 = #()
    local tan2 = #()
	local i =0
	for i = 1 to vertexCount do (
		tan1[i] = [0,0,0]
		tan2[i] = [0,0,0]
	)
	
	local a = 0
	for a = 0 to triangleCount-1 do (
		local i1 = idxs[ a*3 + 1]
		local i2 = idxs[ a*3 + 2]
		local i3 = idxs[ a*3 + 3]
        
		-- Each vertexs is: x,y,z,  nx,ny,nz,  u,v ,...
		local full_v1 = vtxs[ i1 ]
		local full_v2 = vtxs[ i2 ]
		local full_v3 = vtxs[ i3 ]
		
		-- Access to the x,y,z of each vertex
		local v1 = [ full_v1[1], full_v1[2], full_v1[3] ]
		local v2 = [ full_v2[1], full_v2[2], full_v2[3] ]
		local v3 = [ full_v3[1], full_v3[2], full_v3[3] ]
		
		-- Access to the u,v of each vertex
		local w1 = [ full_v1[7], full_v1[8] ]
		local w2 = [ full_v2[7], full_v2[8] ]
		local w3 = [ full_v3[7], full_v3[8] ]
		
        local x1 = v2.x - v1.x;
        local x2 = v3.x - v1.x;
        local y1 = v2.y - v1.y;
        local y2 = v3.y - v1.y;
        local z1 = v2.z - v1.z;
        local z2 = v3.z - v1.z;
        
        local s1 = w2.x - w1.x;
        local s2 = w3.x - w1.x;
        local t1 = w2.y - w1.y;
        local t2 = w3.y - w1.y;
        
        local r = 1.0 / (s1 * t2 - s2 * t1);
        local sdir = [(t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r]
        local tdir = [(s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r]
        
        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;
        
        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
    )
    
	for a = 1 to vertexCount do (
		local v = vtxs[a]
		
        local n = [ v[4], v[5], v[6] ];
        local t = tan1[a];
        
        -- Gram-Schmidt orthogonalize
		local dot_n_t = dot n t
        local tangent = normalize( t - n * dot_n_t )
        
        -- Calculate handedness
		local cross_n_t = cross n t
		local dot_c_t2 = dot cross_n_t tan2[a]
        local w = 1.0
		if dot_c_t2 < 0.0 then w = -1.0
			
		join vtxs[a] #(tangent.x, tangent.y, tangent.z, w )
	)
    
)
-- Fin modif --

fn export_mesh given_obj meshName mat_infos = (
	--messagebox "Entro en export mesh"
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
		throw ("Object " + given_obj.name + " does not have tex coords for channel " + (channelA as string) )
	)
	
	-- Modif 22/04/2016 --
	-- If not mat info is given, generate a single one
	-- which represents 'all the vertexs in a single draw call'
	if mat_infos == undefined then (
		local num_faces = getNumFaces obj
		local all_faces = #()
		for i = 1 to num_faces do all_faces[i] = i
		-- A single group with all the faces
		mat_infos = #( all_faces )
	)
	-- Fin Modif --
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
				
				--Modified 11/05/2016, Cristian --
				--DELETED:
				--local vtx = #(pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uvA.x, 1-uvA.y)
				--ADDED:
				local uTiling = obj.mat.diffusemap.coords.U_tiling
				local vTiling = obj.mat.diffusemap.coords.V_tiling
				local uvx = uvA.x * uTiling
				local uvy = (1-uvA.y) * vTiling
				local vtx = #(pos.x, pos.y, pos.z, normal.x, normal.y, normal.z, uvx, uvy)
				-- End Modif--
				
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
	-- Modif 22/04/2016 --
	computeTangentSpace all_vtxs all_idxs
	--Fin Modif --

	---------------------------------------------
	-- Save everything
	---------------------------------------------
	saveBinaryMesh given_obj all_vtxs all_idxs all_groups outfilename
	--messagebox "Salgo de export_mesh"
)

--export_mesh $

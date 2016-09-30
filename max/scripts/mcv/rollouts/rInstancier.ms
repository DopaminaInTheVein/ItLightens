try(DestroyDialog RL_Instancier)catch()
rollout RL_Instancier "IT Lightens!" width:280 height:101
(
	label lblTitle "INSTANCIER" pos:[7,5] width:102 height:18
	button btnSrc "<Select and click>" pos:[52,23] width:213 height:15
	label lbl3 "Source" pos:[7,24] width:38 height:12

	button btnConvert "Convert selected to instances" pos:[8,43] width:258 height:15
	
--
	local selected
--	


	button btnReplace "Replace selected objects" pos:[9,79] width:258 height:15

	fn replaceObject dest source = (
		local pos = dest.pos
		local new_obj = copy source
		new_obj.pos = pos
		--delete dest
	)

	button btnCopy "Copy selected objs over" pos:[9,61] width:258 height:15
	on btnSrc pressed do
	(
		if $ == undefined then (messagebox "Select an object!")
		if classof $ == ObjectSet then (messagebox "Select only once!")
		selected = $
		btnSrc.text = ("<" + $.name + ">")
	)
	on btnConvert pressed do
	(
		if $ == undefined then (messagebox "Select an object to convert!")
		if classof $ == ObjectSet then (
			for obj in $ do(
				instanceReplace obj selected
			)
		) else (
			instanceReplace $ selected
		)
	)
	on btnReplace pressed do
	(
		if $ == undefined then (messagebox "Select an object to replace!")
		if classof $ == ObjectSet then (
			for obj in $ do(
				replaceObject obj selected
				delete obj
			)
		) else (
			replaceObject $ selected
			delete $
		)
	)
	on btnCopy pressed do
	(
		if $ == undefined then (messagebox "Select an object to copy!")
		if classof $ == ObjectSet then (
			for obj in $ do(
				replaceObject obj selected
			)
		) else (
			replaceObject $ selected
		)
	)
)
CreateDialog RL_Instancier

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------------
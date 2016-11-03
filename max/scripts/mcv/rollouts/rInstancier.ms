try(DestroyDialog RL_Instancier)catch()
rollout RL_Instancier "IT Lightens!" width:280 height:148
(
	label lblTitle "INSTANCIER" pos:[7,5] width:102 height:18
	button btnSrc "<Select and click>" pos:[52,23] width:213 height:15

	button btnConvert "Convert selected to instances" pos:[8,58] width:258 height:15
	
--
	local selected
--	


	button btnReplace "Replace selected objects" pos:[9,94] width:258 height:15
	button btnCopy "Copy selected objs over" pos:[9,76] width:258 height:15
	label lbl3 "Source" pos:[10,24] width:38 height:12
	label lbl4 "Sufix" pos:[10,40] width:38 height:12
	editText edt1 "" pos:[49,40] width:216 height:15

	fn replaceObject dest source = (
		local pos = dest.pos
		local new_obj = copy source
		new_obj.pos = pos
		new_obj.name = dest.name + edt1.text
		--delete dest
	)

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
---------------------------------------------------------------------------------------------------------------------------------
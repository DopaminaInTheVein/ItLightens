try(DestroyDialog RL_Test)catch()
rollout RL_Test "IT Lightens!" width:296 height:4992
(
	label 'lbl4' "OBJECT" pos:[124,8] width:48 height:16 align:#left
	GroupBox 'grp5' "Pick Object" pos:[24,32] width:232 height:48 align:#left
	label 'lbl6' "Selected Object:" pos:[40,53] width:88 height:16 align:#left
	pickbutton 'btn7' "-- None --" pos:[136,52] width:112 height:16 align:#left
	label 'lbl7' "Current Value:" pos:[38,169] width:93 height:16 align:#left
	label 'lbl9' "Set Value" pos:[41,300] width:60 height:16 align:#left
	edittext 'edt4' "" pos:[91,298] width:164 height:16 align:#left
	button 'btn8' "Export From This Object" pos:[36,453] width:220 height:16 align:#left
	button 'btn9' "Modify" pos:[36,432] width:78 height:16 align:#left
	combobox 'cbx2' "ComboBox" pos:[229,293] width:0 height:1 align:#left
	label 'lbl29' "Label" pos:[32,4956] width:160 height:32 align:#left
	button 'btn31' "Modify All ($)" pos:[120,431] width:135 height:16 align:#left
	label 'lbl39' "SCENE" pos:[124,540] width:48 height:16 align:#left
	button 'btn32' "Export Scene" pos:[13,563] width:269 height:24 align:#left
	GroupBox 'grp16' "Edit Components" pos:[15,97] width:259 height:381 align:#left
	dropdownList 'ddl3' "Select a component" pos:[40,118] width:213 height:40 align:#left
	label 'lbl40' "Prefab Value:" pos:[38,277] width:117 height:16 align:#left
	label 'lbl41' "-- None --" pos:[120,277] width:129 height:16 align:#left
	edittext 'curVal' "" pos:[37,183] width:221 height:66 enabled:true align:#left
	
	curVal.readOnly=true
)
CreateDialog RL_Test
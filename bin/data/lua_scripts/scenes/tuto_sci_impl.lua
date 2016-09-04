p = Public()

--==================
-- Level DATA
--==================
TASK_TUTOS_POSS = 0
TASK_TUTOS_BOMB = 1
TASK_TUTOS_STUN = 2
TASK_TUTOS_DRONE = 3
TASK_TUTOS_GENERATOR = 4
-------------------------
poss_done = false
bomb_done = false
stunned_1 = false
stunned_2 = false
repaired_done = false
unposs_done = false
reach_done = false
gen_done = false
--==================

--===========================================
-- Events
--===========================================
function OnStart_tuto_sci( )
	poss_done = false
	bomb_done = false
	stunned_1 = false
	stunned_2 = false
	repaired_done = false
	unposs_done = false
	reach_done = false
	gen_done = false
	p:exec_command("tutosci_help_possess();", 2)
end

-- function opendoortutosci( )
	-- p:print( "opendoortutosci\n")
	-- loadlevel("level_1") -- defined in functions.lua
-- end

function OnUseGenerator_gen_tuto_sci( )
	if not gen_done then
		gen_done = true
		openDoorTutoSci()
		tutosci_help_exit()
		p:complete_tasklist(TASK_TUTOS_GENERATOR)
	end
end

function OnPossess_tuto_sci( )
	if not poss_done then
		poss_done = true
		tutosci_help_wb()
		p:complete_tasklist(TASK_TUTOS_POSS)
	end
end

function OnUnpossess_tuto_sci( )
	if repaired_done then
		if not unposs_done then
			unposs_done = true
			tutosci_help_polarity()
			p:complete_tasklist(TASK_TUTOS_DRONE)
		end
	end
end

function OnCreateBomb_tuto_sci( )
	if not bomb_done then
		bomb_done = true
		tutosci_help_bomb()
		p:complete_tasklist(TASK_TUTOS_BOMB)
	end
end

function OnStunned_guard_tuto_sci_1( )
	if not stunned_1 then
		stunned_1 = true
		if stunned_2 then
			tutosci_stunned_guards()
		else
			tutosci_stunned_guard_remain()
		end
	end
end

function OnStunned_guard_tuto_sci_2( )
	if not stunned_2 then
		stunned_2 = true
		if stunned_1 then
			tutosci_stunned_guards()
		else
			tutosci_stunned_guard_remain()
		end
	end
end

function OnRepairDrone_tuto_sci( )
	if not repaired_done then
		repaired_done = true
		tutosci_help_unpossess()
	end
end

function tutosci_reach( )
	if not reach_done then
		reach_done = true
		tutosci_help_generator()
	end
end

function tutosci_end( )
	sci_done = true
	LoadLevel("level_1")
end
--=============================================

--==============================================================
-- Auxiliars
--==============================================================
function tutosci_help_possess( )
	p:player_talks("Acercate al cientifico y pulsa SHIFT IZQUIERDO para poseer su cuerpo.", "scientific.dds", "SCI")
end
function tutosci_help_unpossess( )
	p:player_talks("Pulsa SHIFT IZQUIERDO para abandonar el cuerpo del científico.", "scientific.dds", "SCI")
end
function tutosci_help_wb( )
	p:player_talks("Acercate al banco de trabajo con el cientifico y pulsa CLIC IZQUIERDO para crear bombas.", "scientific.dds", "SCI")
end
function tutosci_help_bomb( )
	p:player_talks("Lanza bombas con CLIC DERECHO para aturdir a los guardias.\n(No te acerques demasiado o te detectaran.)", "scientific.dds", "SCI")
end
function tutosci_help_exit( )
	p:player_talks("Has recuperado toda tu energia. Cruza la puerta abierta para abandonar este tutorial.", "scientific.dds", "SCI")
end
function tutosci_stunned_guard_remain()
	p:player_talks("Podria ser peor... Aturde al guardia restante.", "scientific.dds", "SCI")
end
function tutosci_stunned_guards( )
	tutosci_help_repair()
	p:complete_tasklist(TASK_TUTOS_STUN)
end
function tutosci_help_repair( )
	p:player_talks("Bien hecho. Al ser un sueño se quedaran aturdidos para siempre.\nAhora acercate al dron de enfrente y pulsa CLIC IZQUIERDO para repararlo.", "scientific.dds", "SCI")
end
function tutosci_help_polarity( )
	p:player_talks("Pulsa Q y E para alternar mi polaridad.\n Usa esta habilidad para atraerte hacia el dron y repelerte despues hacia arriba", "scientific.dds", "SCI")
end
function tutosci_help_generator( )
	p:player_talks("Pulsa CLIC IZQUIERDO cerca de un generador para recuperar energia", "scientific.dds", "SCI")
end

-- Door
idDoorTutoSci = 50
hDoorTutoSci = Handle()
function openDoorTutoSci( )
	hDoorTutoSci:get_handle_by_id(idDoorTutoSci)
	hDoorTutoSci:setLocked(0)
end
--==============================================================

-- Last id 305, 306
-- Next 307, 308

print('This is lua')

SLB.using( SLB )

p = Public( )
h = Handle( )
hg = HandleGroup()
cam = Camera()
player = Player()

function OnEnter_tElevator( )
  --Nothing to do
  --p:player_talks("I'm in an elevator! \nI'm so cool!!! \nI wanna die my hair red!","scientific.dds","SCI")
end

function OnLeave_tElevator( )
  --Nothing to do
  --p:player_talks_color("I'm out of an elevator! \nI'm not so cool!!! \nI wanna die my hair black!","scientific.dds","SCI", "#FF2222FF","#22FF22FF")
end

-- Pila --
--------------------------------------------------------------------------------------------
pila = Handle()
function OnAction___pila( )
  pila:getHandleCaller()
  
  if pila:is_charged() then
	p:player_talks("Where can I use this cell?", "scientific.dds", "SCI")
  else
    p:player_talks("This object contained energy in some past...", "scientific.dds", "SCI")
  end
  p:exec_command("pila:setActionable(1);", 4.5)
end

function OnActionSci___pila( )
  p:player_talks("This cell is exhausted. And too heavy for me...", "scientific.dds", "SCI")
end

-- Cargador --
---------------------------------------------------------------------------------------------
idCargador = 303
triggerCargador = Handle()
hCargador = Handle()
hCargadorTarget = Handle()
function activateCargadorPlayer()
  hCargador:get_handle_by_id(idCargador)

  if hCargador:has_pila() then
    if hCargador:has_pila_charged() then
		p:player_talks("The cell is full of energy now", "scientific.dds", "SCI")
	else
		p:player_talks("The cell fits perfectly.\nI don't know how this machine works though...", "scientific.dds", "SCI")
		
	end
  else
	p:player_talks("I can feel a lot of energy inside this.", "scientific.dds", "SCI")
  end
  
  triggerCargador:getHandleCaller()
  p:exec_command("triggerCargador:setActionable(1);", 4.5)
 
end

function activateCargadorSci()
  hCargador:get_handle_by_id(idCargador)
  if hCargador:has_pila() then
    if hCargador:has_pila_charged() then
		p:player_talks("The cell is full thanks to my hard work", "scientific.dds", "SCI")
	else
		hCargadorTarget:get_handle_by_name_tag("cargador_bateria_pAction", "target")
		p:player_talks("Here we go!", "scientific.dds", "SCI")
	    sci = Handle()
		sci:get_player()
		sci:go_and_look_as(hCargadorTarget, "rechargeCell();")
	end
  else
	p:player_talks("There is nothing to charge", "scientific.dds", "SCI")
  end

  triggerCargador:getHandleCaller()
  p:exec_command("triggerCargador:setActionable(1);", 4.5)
end

function rechargeCell()
  p:player_talks("*CLAC*", "scientific.dds", "SCI")
  hCargador:set_charged(1)
end

-- Enchufe --
--------------------------------------------------------------------------------------------
idEnchufe = 304
triggerEnchufe = Handle()
hEnchufe = Handle()
hEnchufeTarget = Handle()
function activateEnchufePlayer()
  triggerEnchufe:getHandleCaller()
  hEnchufe:get_handle_by_id(idCargador)

  if hEnchufe:has_pila() then
    if hEnchufe:has_pila_charged() then
		p:player_talks("I did it! The door is open now!", "scientific.dds", "SCI")
	else
		p:player_talks("The object on it has no energy...", "scientific.dds", "SCI")
	end
  else
	p:player_talks("This needs energy to work.", "scientific.dds", "SCI")
  end
  
  triggerCargador:getHandleCaller()
  p:exec_command("triggerEnchufe:setActionable(1);", 4.5)
end

function activateEnchufeSci()
  triggerEnchufe:getHandleCaller()
  hEnchufe:get_handle_by_id(idCargador)

  if hEnchufe:has_pila() then
    if hEnchufe:has_pila_charged() then
		p:player_talks("I don't like to brag, but THIS is a good work", "scientific.dds", "SCI")
	else
		p:player_talks("The cell is empty, and too heavy for me...", "scientific.dds", "SCI")
	end
  else
	p:player_talks("This needs a cell to work.", "scientific.dds", "SCI")
  end
  
  triggerCargador:getHandleCaller()
  p:exec_command("triggerEnchufe:setActionable(1);", 4.5)
end

idDoor = 300
function OnPutPila_enchufe()
  pila:getHandleCaller()
  if pila:is_charged() then
	-- Evento abrir puerta
	-- Faltaria cinematica, etc.
	
	--Abril
	h:get_handle_by_id(idDoor)
	h:setLocked(0)
  end
end

function OnRemovePila_enchufe()
  pila:getHandleCaller()
  if pila:is_charged() then
	-- Evento cerrar puerta
	-- Faltaria cinematica, etc.
	
	--Cerral (:P)
	h:get_handle_by_id(idDoor)
	h:setLocked(1)
  end
end
-- Elevator --
-------------------------------------------------
idElevator = 301
triggerElevator = Handle()
function activateElevator( )  
  triggerElevator:getHandleCaller()
  p:setControlEnabled(0)
  h:get_handle_by_id(idElevator)
  elevatorState = h:activate()
  
  p:exec_command( "triggerElevator:setActionable(1);", 2 )
  p:exec_command( "p:setControlEnabled(0);", 5 )
  --cambio sala cientifico
  --if elevatorState == 0 then
  --  p:playerRoom("sala3")
  --else if elevatorState == 2
  --  p:playerRoom("sala2")
  --end
  p:exec_command( "cam:fade_out(1);", 0.5)
  p:exec_command( "cam:fade_in(1);", 5.0)
  
end

function activateElevatorPlayer( )
  triggerElevator:getHandleCaller()
  p:player_talks("I don't know how this mechanism works\n\n","scientific.dds","SCI")
  p:exec_command( "triggerElevator:setActionable(1);", 2 )
end
--------------------------------------------------

-- Electric Lock --
--------------------------------------
-- idDoor = 300
-- triggerLock = Handle()
-- timesActivatedLock = 0
-- function activateLock( )
  -- p:print("Activate Lock\n")
  
  -- --Accionable a los 2 segundos
  -- triggerLock:getHandleCaller()
  -- p:exec_command( "triggerLock:setActionable(1);", 2 )
  
  -- --Modificamos polaridad durante 10 segundos
  -- h:get_handle_by_id(idDoor)
  -- --h:setPolarity(-1)  
  -- h:setLocked(0)  
  -- p:exec_command( "deactivateLock();", 6 )
  
  -- --Variable control para activaciones acumuladas
  -- timesActivatedLock = timesActivatedLock + 1
-- end

-- function putBattery( )
  -- p:print("Put Battery\n")
-- end

-- function deactivateLock( )
  -- p:print("Deactivate Lock\n")
  -- timesActivatedLock = timesActivatedLock - 1
  
  -- --Ha pasado el timer de la última activacion
  -- if timesActivatedLock < 1 then
	  -- --Puerta vuelve a neutral
      -- h:get_handle_by_id(idDoor)
      -- --h:setPolarity(0)
	  -- h:setLocked(1)
  -- end
-- end
--------------------------------------

-- Wall --
--------------------------------------
idWall = 302
tagWallFragment1 = "wall1_fragment"
triggerWall_1 = Handle()
all_fragments1 = HandleGroup()
actionWallTarget = Handle()
function lookWall( )
  p:print("Look Wall\n")
  triggerWall_1:getHandleCaller()
  p:player_talks("I feel some energy behind this wall...","scientific.dds","SCI")
  p:exec_command( "triggerWall_1:setActionable(1);", 2 )
end

function destroyWall( )
  p:print("Destroy Wall\n")
  triggerWall_1:getHandleCaller()
  
  --Llevamos el player al punto de accion
  actionWallTarget:get_handle_by_name_tag("tWall_pAction", "target")
  player = Handle()
  player:get_player()
  player:go_and_look_as(actionWallTarget, "destroyWallEffect();")
end

function destroyWallEffect()
  p:print("Destroy Wall Effect\n")
  --Destruimos pared
  h:get_handle_by_id(idWall)
  h:destroy()
  
  --Activamos fragmentos pared
  all_fragments1:get_handles_by_tag(tagWallFragment1)
  all_fragments1:awake()
  p:exec_command( "all_fragments1:remove_physics();", 5 )
  activeWire1();
  --p:exec_command( "activeWire1();", 0.5 )
end
--------------------------------------


-- Wires --
--------------------------------------
--Cable abajo info
triggerWire_1 = Handle()
tagWireParticle = "wire_particle"
wireName_1 = "tWireDown"
wirePart_1 = wireName_1.."_part"
wirePart_1_h = Handle()
wire_pos_up = "WirePosUp"
posParticleBefore = Pos()

--Cable arriba info
triggerWire_2 = Handle()
wire_pos_down = "WirePosDown"
----------------------------------------

--wireName_2 = "tWireUp" -- No lo necesitamos activar
--posParticleBefore = Pos() -- No hay "particulas" arriba ahora
--tagWireParticle = "wire_particle"
--wirePart_1 = wireName_1.."_part"
--wirePart_1_h = Handle()
function test()
  --Es porque testeando no activo el cable, y no tengo su handle
  triggerWire_1:get_handle_by_name_tag(wireName_1, "trigger")
  p:exec_command( "wireParticlesUp();", 2 )
end

function activeWire1( )
  wirePart_1_h:get_handle_by_name_tag(wirePart_1, tagWireParticle)
  posParticleBefore = wirePart_1_h:get_pos()
  triggerWire_1:get_handle_by_name_tag(wireName_1, "trigger")
  triggerWire_1:setActionable(1);
end

function wireGoUp( )
--  wirePart_1_h:follow_tracker(triggerWire_1, 15*toogle_test)
--  cam:run_cinematic("CineWireGoUp", 10*toogle_test)
--  toogle_test = -toogle_test
  p:print("Wire Go Up")
  factorWireGoUp = 2
  player = Player()
  player:get_player()
  cmd_teleport = "player:teleport(\""..wire_pos_up.."\")"
  cam:run_cinematic("CineWireGoUp", factorWireGoUp * 5)

  p:exec_command( "player:set_position(0,-1000,0);", 2 / factorWireGoUp )
  p:exec_command( "wireParticlesUp();", 2.5 / factorWireGoUp )
  p:exec_command( "cam:fade_out(1);", 2.5)
  p:exec_command( "cam:fade_in(1);", 4.5)
  p:exec_command( "triggerWire_1:setActionable(1);", 3)
  p:exec_command( cmd_teleport, 10 / factorWireGoUp )
end

function wireGoDown( )
  p:print("Wire Go Down")
  factorWireGoDown = 2
  player = Player()
  player:get_player()
  triggerWire_2:getHandleCaller()
  cmd_teleport = "player:teleport(\""..wire_pos_down.."\")"
  cam:run_cinematic("CineWireGoUp", factorWireGoDown * -5)

  p:exec_command( "player:set_position(0,-1000,0);", 2 / factorWireGoDown )
  --p:exec_command( "wireParticlesUp();", 2.5 / factorWireGoDown )
  p:exec_command( "cam:fade_out(1);", 2.5)
  p:exec_command( "cam:fade_in(1);", 4.5)
  p:exec_command( "triggerWire_2:setActionable(1);", 3)
  p:exec_command( cmd_teleport, 10 / factorWireGoDown )
end

function wireParticlesUp( )
  --wirePart_1_h:set_pos(posParticleBefore)
  wirePart_1_h:follow_tracker(triggerWire_1, 15)
end
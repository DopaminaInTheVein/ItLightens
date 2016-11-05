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
  p:player_talks("::look_wall")
  p:exec_command( "triggerWall_1:setActionable(1);", 2 )
end

function destroyWall( )
  p:print("Destroy Wall\n")
  triggerWall_1:getHandleCaller()
  
  --Llevamos el player al punto de accion
  actionWallTarget:get_handle_by_name_tag("tWall_pAction", "target")
  player = Handle()
  player:get_player()
  player:go_and_look_as(actionWallTarget, "destroyWallAnim();")
  
end

function destroyWallAnim( )
  p:print("Destroy Wall Animation")
  --Animacion Player
  player = Handle()
  player:get_player()
  player:set_anim("attack")
  p:exec_command("destroyWallEffect();", 0.5)
end

function destroyWallEffect()
  p:print("Destroy Wall Effect\n")
  
  --Destruimos pared
  h:get_handle_by_id(idWall)
  h:destroy()
  
  --Reproducimos sonido y vibracion
  p:play_sound("event:/OnBreakWall", 1.0, false)
  cam:start_vibration(0.0, 0.75, 10)
  p:exec_command("cam:stop_vibration(8)", 1.0)
  
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

function activeWire1( )
  wirePart_1_h:get_handle_by_name_tag(wirePart_1, tagWireParticle)
  posParticleBefore = wirePart_1_h:get_pos()
  triggerWire_1:get_handle_by_name_tag(wireName_1, "trigger")
  triggerWire_1:setActionable(1);
end

function wireGoUp( )
  p:print("Wire Go Up")
  factorWireGoUp = 2
  player = Player()
  player:get_player()
  cmd_teleport = "player:teleport(\""..wire_pos_up.."\")"
  cam:run_cinematic("CineWireGoUp", factorWireGoUp * 5)
  p:complete_tasklist(7)
  p:exec_command( "player:set_position(0,-1000,0);", 2 )
  p:exec_command( "wireParticlesUp();", 1.5 / factorWireGoUp )
  p:exec_command( "ui_cam:fade_out(0.5);", 7.5)
  p:exec_command( "ui_cam:fade_in(0.5);", 9.5)
  p:exec_command( "triggerWire_1:setActionable(1);", 3)
  p:exec_command( cmd_teleport, 7.5 )
end

function wireGoDown( )
  p:print("Wire Go Down")
  factorWireGoDown = 2
  player = Player()
  player:get_player()
  triggerWire_2:getHandleCaller()
  cmd_teleport = "player:teleport(\""..wire_pos_down.."\")"
  cam:run_cinematic("CineWireGoUp", factorWireGoDown * -5)

  p:exec_command( cmd_teleport, 1.9 )
  p:exec_command( "player:set_position(0,-1000,0);", 5.0 )
  --p:exec_command( "wireParticlesUp();", 2.5 / factorWireGoDown )
  ui_cam:fade_out(0.2)
  p:exec_command( "ui_cam:fade_in(1);", 3.0)
  p:exec_command( "triggerWire_2:setActionable(1);", 3)
  p:exec_command( cmd_teleport, 9.5 )
end

function wireParticlesUp( )
  wirePart_1_h:follow_tracker(triggerWire_1, 15)
end
SLB.using( SLB )

all_co={}

function startCoroutine( f, name, ... )
    print( "COROUTINES: starting "..name )
    local co = coroutine.create( f )
    coroutine.resume( co, ... )
    all_co[ name ] = co
end

function updateCoroutines( elapsed )
    --print( "COROUTINES: update "..tostring(elapsed) )

    local name, co = next( all_co )
    while name ~= nil do
        --print( "COROUTINES: updating "..name )
        local ok = coroutine.resume( co, elapsed )
        local status = coroutine.status( co )

        if not ok or status == "dead" then
            all_co[ name ] = nil
        end

        name, co = next( all_co, name )
    end
end

function clearCoroutines( )
    print( "COROUTINES: clear" )
    local running_co = coroutine.running( )

    local name, co = next( all_co )
    while name ~= nil do
        if co == running_co then
            print( "COROUTINES: Coroutine "..name.." RUNNING!!" )
        else
            all_co[ name ] = nil
        end
        name, co = next( all_co )
    end
end

function dumpCoroutines( )
    print( "COROUTINES: dump" )

    local name, co = next( all_co )
    while name ~= nil do
        print( "COROUTINE:" .. name ..": " .. coroutine.status( co ) )
        name, co = next( all_co )
    end
end

function waitTime( t )
    local curr = 0
    --print( "WAITING_TIME: " .. tostring(t) )
    while true do
        curr = curr + coroutine.yield( )
        --print( "WAITING_TIME: CURR: " .. tostring(curr) )
        if curr > t then
            return
        end
    end
end

function waitKey( key )
    while true do
        local pressed = InputManager:isPressed( key )
        if pressed then 
            return
        else
            coroutine.yield( )
        end
    end
end

function waitCondition( f, ... )
    while true do
        local ok = f( ... )
        if ok then 
            return
        else
            coroutine.yield( )
        end
    end
end

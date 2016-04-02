print('This is lua')

SLB.using( SLB )
clearCoroutines( )

function myCoroutine( name, other_name )
    print( "MY_COROUTINE: Hello, "..name )
    waitTime( 1.0 )
    print( "MY_COROUTINE: Oh, hello, ".. other_name )
    waitKey( "confirm" )
    print( "MY_COROUTINE: Bye, ".. name )
end

function numberGenerator( start )
    local value = start
    while true do
        coroutine.yield( value )
        value = value + 1
        print( coroutine.running( ) )
    end
end

function isEnemyDead( name )
    local is_dead = EnemyManager:isDead( name )
    return is_dead
end

waitCondition( isEnemyDead, name )

startCoroutine( myCoroutine, "co_1", "juan", "pepe" )
--startCoroutine( myCoroutine, "co_2", "maria", "pilar" )

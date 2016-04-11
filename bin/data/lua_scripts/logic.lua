print('This is lua')

SLB.using( SLB )

public = Public( )

function execCommandTest( code, timer )
    public:exec_command( code, timer )
end

function dbg( name )
    public:print( "DBG: "..name )
end




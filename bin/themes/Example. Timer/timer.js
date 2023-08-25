let whoosh = new Audio( 'whoosh.mp3' );

let g_timerTotal = 0;

function div( val, by )
{
    return (val - val % by) / by;
}

// function pad( value )
// {
//	 return ("0" + value).slice(-2);
// }

function Offset( id, y )
{
	let element = document.getElementById(id);

	if( element )
	{
		const posY = y * -77;
		element.style.backgroundPosition = `0px ${posY}px`;
	}
}

function OnTimerTotal( timerTotal )
{
	g_timerTotal = timerTotal;
}

function OnChangeTimer( timeInSecond )
{	
	const total = parseInt( timeInSecond );

	const hh = div( total, 3600 );
	const mm = div( total, 60 ) % 60;
	const ss = total % 60;

	Offset( 'hh_2', Math.floor( hh / 10) );
	Offset( 'hh_1', hh % 10 );

	Offset( 'mm_2', Math.floor( mm / 10 ) );
	Offset( 'mm_1', mm % 10 );
	
	Offset( 'ss_2', Math.floor( ss / 10 ) );
	Offset( 'ss_1', ss % 10 );
}

function OnTimerEvent( timeEvent )
{
	// timeEvent == "1" - Start
	// timeEvent == "2" - Pause
	// timeEvent == "3" - Resume
	// timeEvent == "4" - Stop
	// timeEvent == "5" - time expired

	switch( timeEvent )
	{
		case "1":
		case "4":
		case "5":
		{
			whoosh.pause();
			whoosh.currentTime = 0;
			whoosh.play();
		}
		
		default:
			break;
	}

}

let g_timerTotal = 0;

function div( val, by )
{
    return (val - val % by) / by;
}

function pad( value )
{
	return ("0" + value).slice(-2);
}

function SetValue( id, value )
{
	let element = document.getElementById(id);

	if( element )
	{
		element.textContent = pad(value);
	}
}

function OnTimerTotal( timerTotal )
{
	// timer range [0 - timerTotal]
	g_timerTotal = timerTotal;
}

function OnChangeTimer( timeInSecond )
{	
	const total = parseInt( timeInSecond );

	const hh = div( total, 3600 );
	const mm = div( total, 60 ) % 60;
	const ss = total % 60;

	SetValue( 'hh', hh );
	SetValue( 'mm', mm );
	SetValue( 'ss', ss );
}

function OnTimerEvent( timeEvent )
{
	// switch( timeEvent )
	// timeEvent == "1" - Start
	// timeEvent == "2" - Pause
	// timeEvent == "3" - Resume
	// timeEvent == "4" - Stop
	// timeEvent == "5" - time expired
}

let g_timerTotal = 0;

function OnTimerTotal( timerTotal )
{
	g_timerTotal = timerTotal;
}

function OnChangeTimer( timeInSecond )
{	
	const total = parseInt( timeInSecond );

	let value = 0;
	if( g_timerTotal > 0 )
		value = Math.floor(total / g_timerTotal * 100000 + 0.5) / 1000;

	let element = document.getElementById("progress_bar");
	if( element )
	{
		element.style.width = value + "%";
	}
	
	let elementValue = document.getElementById("progres_value");
	if( elementValue )
	{
		elementValue.textContent = value + "%";
	}
}


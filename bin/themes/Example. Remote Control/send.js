function OnSendScore( playerName, incStep )
{
	const json = `{ "${playerName}": "${incStep}" }`;

	ws.send( json );
}

function OnResetScores( )
{
	const json = `{ "reset": "0" }`;

	ws.send( json );
}

function OnSortAsc( )
{
	const json = `{ "sortasc": "0" }`;

	ws.send( json );
}

function OnSortDesc( )
{
	const json = `{ "sortdesc": "0" }`;

	ws.send( json );
}


function OnTimerStart( )
{
	const json = `{ "timerStart": "0" }`;

	ws.send( json );
}

function OnTimerStop( )
{
	const json = `{ "timerStop": "0" }`;

	ws.send( json );
}

function OnTimerPause( )
{
	const json = `{ "timerPause": "0" }`;

	ws.send( json );
}
function OnSendScore( playerName, incStep )
{
	const json = `{ "${playerName}": "${incStep}" }`;

	ws.send( json );
}

function OnSendCommand( command )
{
	const json = `{ "${command}": "0" }`;

	ws.send( json );
}

let deckSocket = null;

let piAction = null;
let piContext = null;

function SendNewData()
{
	if( !piAction || !piContext )
		return;
	
	let pNumber = 1;
	
	let indexElement = document.getElementsByName("playerNumber");
	if( indexElement.length > 0 )
	{
		pNumber = parseInt(indexElement[0].value);
	}	
	
	const json = 
	{
		action: piAction,
		event: 'sendToPlugin',
		context: piContext,
		payload:
		{
			playerNumber: pNumber,
			incStep: 0.0			
		}
	};
	
	deckSocket.send(JSON.stringify(json));
}

function connectElgatoStreamDeckSocket(inPort, inPropertyInspectorUUID, inRegisterEvent, inInfo, inActionInfo)
{
	piContext = inPropertyInspectorUUID;
	piAction = 'com.maagames.versus.flag';

    deckSocket = null;
    deckSocket = new WebSocket("ws://127.0.0.1:" + inPort);

    deckSocket.onopen = function()
	{
        const json =
		{
            event: inRegisterEvent,
            uuid: inPropertyInspectorUUID,
        };

        deckSocket.send(JSON.stringify(json));		
    };

    deckSocket.onmessage = function(evt)
	{
	    const jsonObj = JSON.parse(evt.data);
		
		switch( jsonObj['event'] )
		{
			case 'sendToPropertyInspector':
			{
				const payload = jsonObj['payload'];
								
				let indexElement = document.getElementsByName("playerNumber");
				for( let i=0; i < indexElement.length; ++i )
				{
					indexElement[i].value = payload['playerNumber'];
				}	

				break;
			}
			default:
				break;			
		}
	};

    deckSocket.onclose = function()
	{
        deckSocket = null;
        versusSocket = null;
    };
}

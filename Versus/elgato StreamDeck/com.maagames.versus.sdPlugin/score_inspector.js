let deckSocket = null;

let piAction = null;
let piContext = null;

function SendNewData()
{
	if( !piAction || !piContext )
		return;
	
	let pNumber = 1;
	let iStep = 1.0;
	
	let indexElement = document.getElementsByName("playerNumber");
	if( indexElement.length > 0 )
	{
		pNumber = parseInt(indexElement[0].value);
	}	

	let stepElement = document.getElementsByName("incStep");
	if( stepElement.length > 0 )
	{
		iStep = parseFloat(stepElement[0].value);
	}
	
	const json = 
	{
		action: piAction,
		event: 'sendToPlugin',
		context: piContext,
		payload:
		{
			playerNumber: pNumber,
			incStep: iStep			
		}
	};
	
	deckSocket.send(JSON.stringify(json));
}

function SendVersusData()
{
	if( !piAction || !piContext )
		return;
	
	let httpPort = 0;
	let socketPort = 0;
	
	let httpElement = document.getElementsByName("httpPort");
	if( httpElement.length > 0 )
	{
		httpPort = parseInt(httpElement[0].value);
	}	

	let socketElement = document.getElementsByName("socketPort");
	if( socketElement.length > 0 )
	{
		socketPort = parseFloat(socketElement[0].value);
	}
	
	const json = 
	{
		action: piAction,
		event: 'sendToPlugin',
		context: piContext,
		payload:
		{
			"httpPort": httpPort,
			"socketPort": socketPort			
		}
	};
	
	deckSocket.send(JSON.stringify(json));
}

function connectElgatoStreamDeckSocket(inPort, inPropertyInspectorUUID, inRegisterEvent, inInfo, inActionInfo)
{
	piContext = inPropertyInspectorUUID;
	piAction = 'com.maagames.versus.showscore';

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

				let stepElement = document.getElementsByName("incStep");
				for (let i=0; i < stepElement.length; ++i )
				{
					stepElement[i].value = payload['incStep'];
				}
				
				let sourceElement = document.getElementsByName("httpPort");
				for( let i=0; i < sourceElement.length; ++i )
				{
					sourceElement[i].value = payload['httpPort'];
				}	

				let socketElement = document.getElementsByName("socketPort");
				for (let i=0; i < socketElement.length; ++i )
				{
					socketElement[i].value = payload['socketPort'];
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

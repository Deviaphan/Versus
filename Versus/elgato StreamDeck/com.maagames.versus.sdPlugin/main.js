let deckSocket = null;
let versusSocket = null;
let pluginUuid = null;

let globalHttpPort = '-1';
let globalSocketPort = '-1';


const ItemEnum = Object.freeze(
	{
		PLAYER_SCORE: 0,
		PLAYER_PHOTO: 1,
		TEAM_FLAG: 2,
		TIMER: 3,
		TIMER_START: 4,
		TIMER_PAUSE: 5,
		TIMER_STOP: 6,
		PLAYER_SORT_UP: 7,
		PLAYER_SORT_DOWN: 8,
		PLAYER_SWAP: 9
	}
);

/*
"itemType" - ItemEnum
"itemContext" - context
"playerIndex" - int
"incStep" - float
"lastData" - string
*/
let allItems = new Array();


function ProcessItems( itemType, playerIndex, itemData, callback )
{
	if( deckSocket == null )
		return;
		
	for( let i=0; i < allItems.length; ++i )
	{
		if( (allItems[i].playerIndex === playerIndex) && (allItems[i].itemType === itemType) )
		{
			callback( allItems[i], itemData );
		}
	}
}

function RemoveItemByContext( context )
{
	for( let i=0; i < allItems.length; ++i )
	{
		if( allItems[i].itemContext === context )
		{
			allItems.splice( i, 1 );
			return;
		}
	}
}

function HasItem( itemType, playerIndex )
{
	for( let i=0; i < allItems.length; ++i )
	{
		const item = allItems[i];
		if( (item.playerIndex === playerIndex) && (item.itemType === itemType) )
		{
			return true;
		}
	}

	return false;
}

function FindNextIndex( itemType )
{
    for (let i = 1; i <= 100; ++i)
	{
        if( !HasItem(itemType, i) )
		{
            return i;
		}
    }

	return 0;
}

function GetPlayerID(payload)
{
	const settings = payload["settings"];
	if( settings && settings.hasOwnProperty("index") )
	{
		return settings["index"];
	}

    return 0;
}

function GetIncStep(payload)
{
    const settings = payload["settings"];
    if( settings && settings.hasOwnProperty("incStep") )
	{
        return settings["incStep"];
    }

    return 1.0;
}

function UpdateScore( item, itemData )
{
	const json =
	{
		event: "setTitle",
		context: item.itemContext,
		payload:
		{
			title: itemData
		}
	};

	deckSocket.send(JSON.stringify(json));
}

function UpdateButtonImage( item, itemData )
{
	if( item.lastData && (item.lastData === itemData) )
		return;
	
	item.lastData = ""+itemData;
	
	let image = new Image();
	image.crossOrigin = 'Anonymous';
	image.onload = function()
	{
		let canvas = document.createElement('canvas');
		let context = canvas.getContext('2d');
		canvas.height = 144;
		canvas.width = 144;
		context.drawImage(this, 0, 0, 144, 144);
		const dataURL = canvas.toDataURL('image/png');

		const json =
		{
            event: "setImage",
            context: item.itemContext,
            payload:
			{
				image: dataURL
			}
		};

		deckSocket.send(JSON.stringify(json));
	};
	
	image.src = "http://127.0.0.1:" + globalHttpPort +"/" + itemData;
	
}

function UpdateButtonColor( item, itemData )
{
	if( item.lastData && (item.lastData === itemData) )
		return;
	
	item.lastData = ""+itemData;

	let canvas = document.createElement('canvas');
	let context2D = canvas.getContext('2d');
	canvas.height = 144;
	canvas.width = 144;

	let grd = context2D.createRadialGradient(72, 72, 16, 72, 72, 100);
	grd.addColorStop(0, "black");
	grd.addColorStop(1, itemData);
	context2D.fillStyle = grd;
	context2D.fillRect(0, 0, 144, 144);

	const dataURL = canvas.toDataURL('image/png');

	const json =
	{
		event: "setImage",
		context: item.itemContext,
		payload:
		{
			image: dataURL
		}
	};

	deckSocket.send(JSON.stringify(json));
	
}

function div( val, by )
{
    return (val - val % by) / by;
}

function pad( value )
{
	return ("0" + value).slice(-2);
}

function UpdateTime( item, itemData )
{
	const total = parseInt( itemData );

	const hh = div( total, 3600 );
	const mm = div( total, 60 ) % 60;
	const ss = total % 60;

	const json =
	{
		event: "setTitle",
		context: item.itemContext,
		payload:
		{
			title: (hh > 0 ) ? (pad(hh) + ":" + pad(mm) + "\n'" + pad(ss)) : (pad(mm) + ":" + pad(ss))
		}
	};

	deckSocket.send(JSON.stringify(json));
}

function CloseVersus()
{
	if( deckSocket )
	{
		for( let i=0; i < allItems.length; ++i )
		{
			const item = allItems[i];

			switch( item.itemType )
			{
				case ItemEnum.PLAYER_SCORE:
				{
					const jsonImage =
					{
						event: "setImage",
						context: item.itemContext
					};
					deckSocket.send(JSON.stringify(jsonImage));

					const jsonTitle =
					{
						event: "setTitle",
						context: item.itemContext,
						payload:
						{
							title: "P." + item.playerIndex
						}
					};
					deckSocket.send(JSON.stringify(jsonTitle));
					break;
				}
				case ItemEnum.PLAYER_PHOTO:
				{
					const jsonImage =
					{
						event: "setImage",
						context: item.itemContext
					};
					deckSocket.send(JSON.stringify(jsonImage));

					break;
				}
				case ItemEnum.TEAM_FLAG:
				{
					const jsonImage =
					{
						event: "setImage",
						context: item.itemContext
					};
					deckSocket.send(JSON.stringify(jsonImage));
					break;
				}
				case ItemEnum.TIMER:
				{
					const jsonTitle =
					{
						event: "setTitle",
						context: item.itemContext,
						payload:
						{
							title: "00:00"
						}
					};
					deckSocket.send(JSON.stringify(jsonTitle));
					break;
				}
				default:
					break;
			}
		}
	}

	versusSocket = null;
}

function GetPlayerIndex( idString )
{
	const pid = parseInt( idString.match(/\d+/) );
	return pid;
}

function ParseVersusData( evt )
{
    if( !deckSocket )
        return;

    const jsonData = JSON.parse(evt.data);

    for (let i = 0; i < jsonData.data.length; ++i)
	{
        const data = jsonData.data[i];

        if( (data.id.charAt(0) == 'p') && data.id.includes('score') )
		{
			const pid = GetPlayerIndex( data.id );
			ProcessItems( ItemEnum.PLAYER_SCORE, pid, data.v, UpdateScore );
        }
		else if( (data.id.charAt(0) == 'p') && data.id.includes('color') )
		{
			const pid = GetPlayerIndex( data.id );
			ProcessItems( ItemEnum.PLAYER_SCORE, pid, data.v, UpdateButtonColor );
        }
		else if ((data.id.charAt(0) == 'p') && data.id.includes('logo'))
		{
			const pid = GetPlayerIndex( data.id );
			ProcessItems( ItemEnum.PLAYER_PHOTO, pid, data.v, UpdateButtonImage );
        }
		else if ((data.id.charAt(0) == 't') && data.id.includes('logo'))
		{
			const pid = GetPlayerIndex( data.id );
			ProcessItems( ItemEnum.TEAM_FLAG, pid, data.v, UpdateButtonImage );
		}
		else if( data.id === 'timer_hms' )
		{
			ProcessItems( ItemEnum.TIMER, 0, data.v, UpdateTime );
		}
    }
}

function InitVersusSocket()
 {
    if( deckSocket == null )
        return;

    if( versusSocket != null )
	{
        versusSocket.send('{"updateMe":"1"}');
        return;
    }

    versusSocket = new WebSocket( "ws://localhost:" + globalSocketPort );
    versusSocket.onopen = function ()
	{
        versusSocket.send('{"updateMe":"1"}');
    };

    versusSocket.onerror = function (err)
	{
        versusSocket.close();
        versusSocket = null;
    }

    versusSocket.onclose = CloseVersus;

    versusSocket.onmessage = ParseVersusData;
}


function StreamDeckWillAppear( action, context, payload )
{	
    switch( action )
	{
		case 'com.maagames.versus.showscore':
		{
			let playerId = GetPlayerID(payload);
			let iStep = GetIncStep(payload);

			if( playerId == 0 )
			{
				playerId = FindNextIndex( ItemEnum.PLAYER_SCORE );
				
				const newSettings = 
				{
					event: "setSettings",
					context: context,
					payload:
					{
						index: playerId,
						incStep: iStep
					}
				};

				deckSocket.send(JSON.stringify(newSettings));
			}

			const newItem =
			{
				itemType: ItemEnum.PLAYER_SCORE,
				itemContext: context,
				playerIndex: playerId,
				incStep: iStep,
				lastData: ""
			};
			
			allItems.push( newItem );			
			
			break;
		}
		case 'com.maagames.versus.photo':
		{
			let playerId = GetPlayerID(payload);

			if( playerId == 0 )
			{
				playerId = FindNextIndex( ItemEnum.PLAYER_PHOTO );
				
				const newSettings = 
				{
					event: "setSettings",
					context: context,
					payload:
					{
						index: playerId
					}
				};

				deckSocket.send(JSON.stringify(newSettings));
			}

			const newItem =
			{
				itemType: ItemEnum.PLAYER_PHOTO,
				itemContext: context,
				playerIndex: playerId,
				lastData: ""
			};
			
			allItems.push( newItem );

			break;
		}
		case 'com.maagames.versus.flag':
		{
			let playerId = GetPlayerID(payload);

			if( playerId == 0 )
			{
				playerId = FindNextIndex( ItemEnum.TEAM_FLAG );
				
				const newSettings = 
				{
					event: "setSettings",
					context: context,
					payload:
					{
						index: playerId
					}
				};

				deckSocket.send(JSON.stringify(newSettings));
			}
			
			const newItem =
			{
				itemType: ItemEnum.TEAM_FLAG,
				itemContext: context,
				playerIndex: playerId,
				lastData: ""
			};
			
			allItems.push( newItem );
			
			break;
		}
		case 'com.maagames.versus.timer':
		{
			const newItem =
			{
				itemType: ItemEnum.TIMER,
				itemContext: context,
				playerIndex: 0
			};
			
			allItems.push( newItem );

			break;
		}
		default:
			break;
	}
	
	InitVersusSocket();
}

function StreamDeckWillDisappear( context )
{
	RemoveItemByContext( context );
}

function StreamDeckOnMessage( evt )
{
    const jsonObj = JSON.parse(evt.data);

    const event = jsonObj['event'];

    const action = jsonObj['action'];
    const context = jsonObj["context"];

    switch( event )
	{
		case 'keyUp':
		{
			if( versusSocket != null )
			{
				if( action === 'com.maagames.versus.showscore' )
				{
					const payload = jsonObj["payload"];

					const playerId = GetPlayerID(payload);
					if( playerId > 0 )
					{
						const incStep = GetIncStep(payload);
						const json = `{ "p${playerId}score": "${incStep}" }`;
						versusSocket.send(json);
					}
				}
				
				else if( action === 'com.maagames.versus.starttimer' )
				{
					const json = `{ "timerStart": "0" }`;
					versusSocket.send(json);
				}
				else if( action === 'com.maagames.versus.stoptimer' )
				{
					const json = `{ "timerStop": "0" }`;
					versusSocket.send(json);
				}
				else if( action === 'com.maagames.versus.pausetimer' )
				{
					const json = `{ "timerPause": "0" }`;
					versusSocket.send(json);
				}
				
				else if( action === 'com.maagames.versus.reset' )
				{
					const json = `{ "reset": "0" }`;
					versusSocket.send(json);
				}
				else if( action === 'com.maagames.versus.swap' )
				{
					const json = `{ "swap": "0" }`;
					versusSocket.send(json);
				}
				else if( action === 'com.maagames.versus.sortasc' )
				{
					const json = `{ "sortasc": "0" }`;
					versusSocket.send(json);
				}
				else if( action === 'com.maagames.versus.sortdesc' )
				{
					const json = `{ "sortdesc": "0" }`;
					versusSocket.send(json);
				}
			}
			else
			{
				InitVersusSocket();
			}
			break;
		}
		case 'willAppear':
		{
			const payload = jsonObj["payload"];
			StreamDeckWillAppear( action, context, payload );
			break;
		}
		case 'willDisappear':
		{
			StreamDeckWillDisappear( context );
			break;
		}
		case 'propertyInspectorDidAppear':
		{
			for( let i=0; i < allItems.length; ++i )
			{
				const item = allItems[i];
				if( item.itemContext === context )
				{
					const json =
					{
						action: action,
						event: "sendToPropertyInspector",
						context: context,
						payload:
						{
							playerNumber: item.playerIndex,
							incStep: item.incStep,
							httpPort: globalHttpPort,
							socketPort: globalSocketPort
						}
					};

					deckSocket.send(JSON.stringify(json));

					return;
				}
			}
			break;
		}
		case 'sendToPlugin':
		{
			for( let i=0; i < allItems.length; ++i )
			{				
				let item = allItems[i];
				if( item.itemContext === context )
				{
					const payload = jsonObj["payload"];

					if( payload.hasOwnProperty("playerNumber") )
					{
						item.playerIndex = payload["playerNumber"];
						item.incStep = payload["incStep"];					
							
						const newSettings = 
						{
							event: "setSettings",
							context: item.itemContext,
							payload:
							{
								index: item.playerIndex,
								incStep: item.incStep
							}
						};

						deckSocket.send(JSON.stringify(newSettings));
						
				        versusSocket.send('{"updateMe":"1"}');
					}
					else if( payload.hasOwnProperty("httpPort") )
					{
						let newHttp;
						let newSocket;
						
						if( payload.hasOwnProperty("httpPort") )
						{
							newHttp = payload["httpPort"];
						}
						
						if( payload.hasOwnProperty("socketPort") )
						{
							newSocket = payload["socketPort"];
						}

						if( (globalHttpPort != newHttp) || (globalSocketPort != newSocket) )
						{
							globalHttpPort = newHttp;
							globalSocketPort = newSocket;
							
							const jsonGlobalSetting =
							{
								"event": "setGlobalSettings",
								"context": pluginUuid,
								"payload":
								{
									httpPort: globalHttpPort,
									socketPort: globalSocketPort
								}
							}
							deckSocket.send( JSON.stringify(jsonGlobalSetting) );
						}
					}

					return;
				}
			}

			break;
		}
		case 'didReceiveGlobalSettings':
		{
			console.log('didReceiveGlobalSettings' + evt.data);
			
			const payload = jsonObj["payload"];
			const settings = payload["settings"];

			if( settings.hasOwnProperty("httpPort") )
			{
				console.log('Saved http port' + evt.data);

				globalHttpPort = settings["httpPort"];
			}
			else
			{
				globalHttpPort='9090';
			}
			
			if( settings.hasOwnProperty("socketPort") )
			{
				console.log('Saved socket port' + evt.data);

				globalSocketPort = settings["socketPort"];
			}
			else
			{
				globalSocketPort='9091';
			}
			
			break;
		}
		case 'applicationDidLaunch':
		{
			break;
		}
		case 'applicationDidTerminate':
		{
			break;
		}
	}
}


function connectElgatoStreamDeckSocket(
    inPort,
    inPluginUUID,
    inRegisterEvent,
    inInfo)
{
    deckSocket = null;
    deckSocket = new WebSocket("ws://127.0.0.1:" + inPort);

    deckSocket.onopen = function()
	{
		pluginUuid = inPluginUUID;
		
        const json =
		{
            event: inRegisterEvent,
            uuid: inPluginUUID,
        };

        deckSocket.send(JSON.stringify(json));
		
		const jsonGetGlobal =
		{
			"event": "getGlobalSettings",
			"context": inPluginUUID
		}
		deckSocket.send(JSON.stringify(jsonGetGlobal));
    };

    deckSocket.onmessage = StreamDeckOnMessage;

    deckSocket.onclose = function()
	{
        deckSocket = null;
        versusSocket = null;
    };
}

// update content by element ID

let ws = null;

function ParseData(event)
{
	let jsonData = JSON.parse(event.data);
	
	if( jsonData.updateView == 1 )
	{
		document.location.reload();
		return;
	}
	
	for( let i = 0; i < jsonData.data.length; ++i )
	{
		let data = jsonData.data[i];
		
		if( data.id === 'themeStyleIndex' )
		{
			if( typeof OnChangeThemeStyle === 'function' )
			{
				OnChangeThemeStyle( data.v );
			}
			continue;
		}
		
		if( data.id === 'timer_hms' )
		{
			if( typeof OnChangeTimer === 'function' )
			{
				OnChangeTimer( data.v );
			}
			continue;
		}
		
		if( data.id === 'timer_total' )
		{
			if( typeof OnTimerTotal === 'function' )
			{
				OnTimerTotal( data.v );
			}
			continue;
		}

		if( data.id === 'timer_event' )
		{
			if( typeof OnTimerEvent === 'function' )
			{
				OnTimerEvent( data.v );
			}
			continue;
		}

		let element = document.getElementById(data.id);
		if( element != null )
		{
			if( data.id.search('logo')>= 0 )
			{
				if( element.nodeName.toLowerCase() === 'img')
				{
					element.src = data.v;
				}
				else
				{
					element.style.backgroundImage = `url('${data.v}')`;
				}
			}
			else
			{
				if( element.textContent != data.v )
				{
					if( typeof OnChangeDataValue === 'function' )
					{
						OnChangeDataValue( data.id, data.v );
					}
					else
					{
						element.textContent = data.v;
					}
				}
			}
		}
	}
}

function UpdateData()
{
	ws = new WebSocket( "ws://%LOCALHOST%:%PORT%" );
	
	ws.addEventListener("error", (event) => {
	  console.log("WebSocket error: ", event);
	});
	
	
	ws.onopen = function(){ ws.send('{"updateMe":"1"}'); };

	ws.onmessage = ParseData;
	
	ws.onerror = function(err)
	{
		ws.close();
		setTimeout(function(){UpdateData()}, 5000);
	}
	
	ws.onclose = function()
	{
		ws = null;		
		setTimeout(function(){UpdateData()}, 5000);
	}
}


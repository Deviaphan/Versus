let lastIndex = 0;

function OnChangeThemeStyle( styleIndex )
{
	let panelElement = document.getElementById("panel");
	if( panelElement )
	{
		const number = parseInt( styleIndex );
		if( lastIndex != number )
		{
			lastIndex = number;
			const posY = -number * panelElement.clientHeight;
			panelElement.style.backgroundPosition = `0px ${posY}px`;
		}
	}
}

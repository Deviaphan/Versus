function OnSelectScene()
{
	const queryString = window.location.search;
	
	if( queryString === "?RemoteControl" )
	{
		let element = document.getElementById("RemoteControl");
		if( element )
		{
			element.style.display = "block";
		}
	}
	else// if( queryString === "?Scene_1" )
	{
		let element = document.getElementById("Scene_1");
		if( element )
		{
			element.style.display = "block";
		}
	}
}

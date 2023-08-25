function OnChangeThemeStyle( hideClock )
{
	let clockElement = document.getElementById("Clock_Face");
	if( clockElement )
	{
		if( hideClock == "1" )
		{
			clockElement.style.visibility  = "hidden";
		}
		else
		{
			clockElement.style.visibility = "visible";
		}
	}
}

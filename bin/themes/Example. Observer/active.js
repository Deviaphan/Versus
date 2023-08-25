let timeout1 = 0;
let ring = null;

function ChangeBack_1()
{
	let element = document.getElementById("p1score");
	if( element != null )
		element.classList.remove("pBig");
}

function Change_1()
{
	clearTimeout( timeout1 );
	let element = document.getElementById("p1score");
	if( element != null )
	{
		if( ring )
		{
			ring.pause();
			ring.currentTime = 0;
			ring.play();
		}
		
		element.classList.add("pBig");
		timeout1 = setTimeout( ChangeBack_1, 1000 );
	}
}


function InitListener()
{
	ring = new Audio( 'ring.mp3' );
	
 	let MutationObserver = window.MutationObserver || window.WebKitMutationObserver || window.MozMutationObserver;
	
    let container1 = document.getElementById ("p1score");	
	let observer1 = new MutationObserver(Change_1);
	observer1.observe(container1, { childList: true  });
}

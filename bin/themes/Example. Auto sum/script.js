let p1 = [1, 3, 5, 7, 9, 11, 13];
let p2 = [2, 4, 6, 8, 10, 12, 14];

function OnChangeDataValue( element, id, value )
{
	if( element == null )
		return;

	// default processing
	element.textContent = value;

	let sumP1 = 0;
	let sumP2 = 0;
	let wonP1 = 0;
	let wonP2 = 0;
	
	for( let i=0; i < 7; ++i)
	{
		let e1 = document.querySelector( ".p" + p1[i] + "score" );
		let e2 = document.querySelector( ".p" + p2[i] + "score" );

		let v1 = parseInt( e1.textContent );
		let v2 = parseInt( e2.textContent );
		
		sumP1 += v1;
		sumP2 += v2;
		
		if( v1 > v2 )
		{
			wonP1 += 1;
		}
		else if( v1 < v2 )
		{
			wonP2 +=1;
		}
	}
	
	let w1 = document.querySelector( ".P1Won" );
	w1.textContent = wonP1;
	let w2 = document.querySelector( ".P2Won" );
	w2.textContent = wonP2;
	
	
	let s1 = document.querySelector( ".P1Sum" );
	s1.textContent = sumP1;
	let s2 = document.querySelector( ".P2Sum" );
	s2.textContent = sumP2;
}

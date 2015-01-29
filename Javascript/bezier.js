function bezier(pts) 
{
  return function (t) 
         {
    	    for (var a = pts; a.length > 1; a = b) // do..while loop in disguise
    		for (var i = 0, b = [], j; i < a.length - 1; i++) // cycle over control points
    		    for (b[i] = [], j = 0; j < a[i].length; j++) // cycle over dimensions
    			b[i][j] = a[i][j] * (1 - t) + a[i+1][j] * t; // interpolation
    	    return a[0];
  	}	
}

/* example usage:
var b = bezier([[0, 0, 0], [1, 1, 1], [2, -3, 6]]);
for (var t = 0; t <= 10; t++) console.log(b(t/10));
*/

function b(t,p)
{
  var result=[2];
  for(var i=0;i<2;i++)
  {
    result[i]=Math.pow((1-t),3)*p[0][i]+3*(1-t)*(1-t)*t*p[1][i]+3*(1-t)*t*t*p[2][i]+Math.pow(t,3)*p[3][i];
  }
  return result;
}

function b_prime(t,p)
{
  var result=[2];
  for(var i=0;i<2;i++)
  {
    result[i]=3*Math.pow((1-t),2)*(p[1][i]-p[0][i])+6*(1-t)*t*(p[2][i]-p[1][i])+3*t*t*(p[3][i]-p[2][i]);
  }
  return result;
}

function parametric(u, s, e)
{
    var result=[];
    result[0]=(1-u)*s[0]+u*e[0];
    result[1]=(1-u)*s[1]+u*e[1];
    return result;
}

function calculate()
{
    var DEBUG=0;
    var P=[];
    var N=4;
    
    var Segs=document.getElementById("Segs").value;
    var u=1/Segs;
    for(var i=0;i<N;i++)
    {
    	P[i]=document.getElementById("P"+i).value.split(",");
    }
    var attribute="M0,0 L"+P[0][0]+","+(-P[0][1])+" C";
    for(var i=1;i<N;i++)
    {
	attribute=attribute+P[i][0]+","+(-P[i][1])+" ";
    }
    document.getElementById("output").setAttribute("d",attribute); 

    var svg = document.getElementsByTagName('svg')[0]; //Get svg element
    var control=[4];
    var tmp23;

    for(var i=0;i<2;i++)
    {
	if(i==0)
	{
		control[0]=P[0];//1
 		control[1]=parametric(0.5,P[0],P[1]);//12
		tmp23=parametric(0.5,P[1],P[2]);//23
		control[2]=parametric(0.5,control[1],tmp23);//123
		var tmp234;
		tmp234=parametric(0.5,parametric(0.5,P[3],P[2]),parametric(0.5,P[2],P[1]));
		control[3]=parametric(0.5,control[2],tmp234);
        }
	else
	{
		
		control[0]=control[3];//1234
		control[3]=P[3];//4
                control[2]=parametric(0.5,P[2],P[3]);//34
                tmp23=parametric(0.5,P[1],P[2]);//23
                control[1]=parametric(0.5,control[2],tmp23);//234

	}
	//if(DEBUG) alert(control);
	var newElement = document.createElementNS("http://www.w3.org/2000/svg", 'path'); //Create a path in SVG's namespace
	if(i==0)
	{
		attribute="M0,0 L"+control[0][0]+","+(-control[0][1])+" C";
	}
	else
	{
		attribute="M"+control[0][0]+","+(-control[0][1])+"L"+control[0][0]+","+(-control[0][1])+" C";
	}
    	for(var j=1;j<N;j++)
        {
            attribute=attribute+control[j][0]+","+(-control[j][1])+" ";
        }
	newElement.setAttribute("d",attribute); //Set path's data
	newElement.style.fill="none";
	if(i==0)
	{
		newElement.style.stroke = "#F00"; //Set stroke colour
	}
	else
	{
		newElement.style.stroke = "#0F0";
	}
	newElement.style.strokeWidth = "4px"; //Set stroke width
	svg.appendChild(newElement);
    }
    
	for (var t = 0; t < 0.999; t=t+u) 
	{
	    var newElement = document.createElementNS("http://www.w3.org/2000/svg", 'rect');
	    var tmp=b(t,P);
	    var tilt=b_prime(t,P);
	    var degree=(Math.atan(-tilt[1]/tilt[0]))/Math.PI*180;
	    if(DEBUG) alert(degree);
	    newElement.setAttribute("x",-8); 
	    newElement.setAttribute("y",-8);
	    newElement.setAttribute("height",16);
	    newElement.setAttribute("width",16);
            newElement.style.fill="#00F";
	    newElement.setAttribute("opacity",0.5);
	    newElement.style.strokeWidth = "1px";
	    newElement.setAttribute("transform","translate("+tmp[0]+","+(-tmp[1])+") "+"rotate("+degree+")");
	    svg.appendChild(newElement);
	}
}


﻿//NAME              ID
//Jake Terpening    21945519
//Whitney Tran      77187709
//Daniel Williams   77402284
//Tristan Bock      50665011


var Server;

var SPACESIZE = 20;
var col = 12, row = 12; // store size of board
var state_str = "";

var IncMessage = "";

var p1Name = "", p2Name = "";
var clientSnake; // client's snake
var score1 = 0, score2 = 0;

var KEY_LEFT = 37, KEY_UP = 38, KEY_RIGHT = 39, KEY_DOWN = 40; //keyboard [left,up,right,down]
var A_KEY = 65, W_KEY = 87, D_KEY = 68, S_KEY = 83;  //keyboard [a,w,d,s]


//distinguishes which client ID is which player
function whichSnake(snakeID)
{
	if (snakeID == p1)
		return p1;
	else if (snakeID == p2)
        	return p2;
}

//sends message from client to update server
function send(text)
{
	Server.send('message', text);           
}

function connect()
{
    	Server = new FancyWebSocket('ws://' + document.getElementById('ip').value + ':' + document.getElementById('port').value);
    	Server.connect();

    	Server.bind('open', function ()
	{
        	send("START:" + document.getElementById('playerid').value + ":DEFAULT");
        	document.getElementById("cntBtn").disabled = true;
    	});

    	Server.bind('close', function (data)
	{
        	document.getElementById("cntBtn").disabled = false;
    	});
}

//Get which key was pressed
//var A_KEY = 65, W_KEY = 87, D_KEY = 68, S_KEY = 83;  //keyboard [a,w,d,s]
function getKeypress()
{
	//W/UP, A/LEFT, S/DOWN, D/RIGHT

    if (e.keyCode == A_KEY || e.keyCode == KEY_LEFT)
        send("MOVE:a");

    else if (e.keyCode == W_KEY || e.keyCode == KEY_UP)
        send("MOVE:w");

    else if (e.keyCode == D_KEY || e.keyCode == KEY_RIGHT)
        send("MOVE:d");

    else if (e.keyCode == S_KEY || e.keyCode == KEY_DOWN)
        send("MOVE:s");
}

//updates server on current client state
function update()
{
	getKeypress();
	//get message from server
}

function handleMessage() // Not necessary
{
        main();
}

//yup
function draw()
{
        //visual goodness
	ctx.fillStyle = "#000000";
        ctx.fillText(p1Name + score1, 5, 20);
	ctx.fillText(p2name + score2, 5, 40);
}

function loop()
{
        update();
        draw_by_str(state_str);
        window.requestAnimationFrame(loop);
}




function draw_by_str(state_str)
{
    var c_x=0;        //current x-pos
    var c_y = 0;        //current y-pos
    var c_w = canvas.width / col; //pixel width
    var c_h = canvas.height / row; //pixel height

    console.log(state_str); // Shows string on js console (ctrl + shift + j on Chrome)

    for (var i = 0; i < state_str.length; i++)      //iterates through the state string (created in gamestate by state_str())
    {
        var c = state_str.charAt(i);
        console.log(c);
        context.beginPath();
        context.rect(c_x, c_y, c_w, c_h) // (top-right x coor of rect, top-right y, pixel width, pixel height)

        //border
        if(c == "X")
        {
            context.fillStyle = "black";        
            context.fill();
            c_x += 20;                          //go to next tile
        }

        //new row
        if(c == "-")                            
        {
            c_x = 0; 
            c_y += 20;
        }

        //empty space
        if(c == "0")
        {
            context.fillStyle = "green";
            context.fill();
            c_x += 20;                          //go to next tile
        }

        //contains player 1
        if (c == "1") {
            context.fillStyle = "red";
            context.fill();
            c_x += 20;                          //go to next tile
        }

        //contains player 2
        if (c == "2") {
            context.fillStyle = "blue";
            context.fill();
            c_x += 20;                          //go to next tile
        }

        //contains food
        if (c == "3") {
            context.fillStyle = "yellow";
            context.fill();
            c_x += 20;                          //go to next tile
        }
            
    }
}




function main()
{
        canvas = document.createElement("canvas");
        canvas.width = 12 * 20; // add 2 extra for border space
        canvas.height = 12 * 20; 
        context = canvas.getContext("2d");
        document.body.appendChild(canvas);
        context.font = "16px Arial";

        document.addEventListener("keydown", getKeypress, false);
        draw_by_str(state_str);
        //init();
	    //loop();
}

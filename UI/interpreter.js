
var con = new SimpleConsole({
	handleCommand: handle_command,
	placeholder: "",
	storageID: "simple-console demo"
});
document.body.appendChild(con.element);

con.logHTML(
	"<p>Python3 Console</p>"
);

function handle_command(command){
	var err;
	try {
		var result = eval(command);
	}
	catch(error) {
		err = error;
	}

	if(err) {
		con.error(err);
	}
	else {
		con.log(result).classList.add("result");
	}
};

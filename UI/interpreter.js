
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
	try {
		window.webkit.messageHandlers.command.postMessage(command);
	}
	catch(error) {
		con.error(error)
	}
};

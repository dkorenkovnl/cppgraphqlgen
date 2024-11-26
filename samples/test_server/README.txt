//************************
to generate cpp files

./schemagen -s ./schema.test_server.graphql -p TestServer -n test_server

//************************
To test subscription you could use Simple WebSocket Client chrome addon

url for connection:
ws://127.0.0.1:8080/graphql
http://127.0.0.1:8080/graphql

Request:

{
	"query": "subscription {\nsubscriptionTelemetry {\n id: id \n when \n name \n sensorId \n sensorValue \n } \n}",
	"operationName": "",
	"variables": ""
}
//************************


//************************


//************************


//************************
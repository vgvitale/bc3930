function doGet(e) {
  var ss = SpreadsheetApp.getActiveSpreadsheet();
  var sheet = ss.getSheets()[0];

  // Obtain parameters from the HTTP GET request
  var temperature = e.parameter.temperature;
  var humidity = e.parameter.humidity;

  // Append a new row with the current date and time, temperature, and humidity
  sheet.appendRow([new Date(), temperature, humidity]);

  // Return a simple message to indicate success
  return ContentService.createTextOutput("Temperature and Humidity data added.");
}

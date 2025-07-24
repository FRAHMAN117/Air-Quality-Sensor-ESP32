// Credits to Sujay S. Phadke, 2017
// Github: https://github.com/electronicsguy
// Read/Write to Google Sheets using REST API.
// Can be used with ESP8266 & other embedded IoT devices.
 
// Use this file with the ESP8266 library HTTPSRedirect

// let           - https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/let
// const         - https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/const
// UrlFetchApp   - https://developers.google.com/apps-script/reference/url-fetch/url-fetch-app
// SpreadSheet App - https://developers.google.com/apps-script/reference/spreadsheet/spreadsheet-app
// For testing end points - https://reqbin.com/curl 
// BetterLog     - https://github.com/peterherrmann/BetterLog 

// Payload example:
// sprintf(outstr, "%02u/%02u/%02u %02u:%02u:%02u, %.2d, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %x",year,month,day,hour,minute,second,CO2,Tco2,RHco2,Tbme,Pbme,RHbme,measuredvbat,stat);
// String payload = "{\"command\":\"appendRow\",\"sheet_name\":\"Sheet1\",\"values\":" + "\"" + outstr + "\"}"

/********************************************'
 * Better Logger to Google Sheets 'BetterLogger - dont delete'
 *******************************************/
// Add one line to use BetterLog
// Logger = BetterLog.useSpreadsheet('1B2AWsd1ERaQHfxh8OtDRngoFK0kGmNL4023jO8gaUHU'); // this id is for GS 'BetterLog - dont delete'
// Now you can log and it will also log to the spreadsheet
// Logger.log("That's all you need to do");


/********************************************************************************
 * Function to send data to Node Red endpoint(/...) 
 ********************************************************************************/
function sendToNodeRedPost(data) {
  let url = "https://ngens.environmentalcrossroads.net/nodered/o3ngensrooftopobs"; 
  let encodedCredentials = Utilities.base64Encode("n******in:N*****1n");

  let options = {
    method: "POST",
    contentType: "application/json",
    payload:JSON.stringify(data),
    headers: {
      "Authorization": "Basic " + encodedCredentials
    },
  };
  try {
    //Logger.log("Calling UrlFetchApp - In ESP8266");
    UrlFetchApp.fetch(url, options);
  } catch (e){
    //Logger.log("CSLP8266 Error occurred while doing Post to Node Red");
  }  
}

/**********************************************************************************
 *  Set sheet headers 
 **********************************************************************************/
const SS = SpreadsheetApp.openById('PLACE GSID FROM GOOGLE SHEET');
const sheet = SS.getSheetByName('Sheet1'); // don't change this!

/*************************************************************************************
  * The doPost(e) gets triggered when a client calls a post request on the AppScript 
  * using the Web App Url
 *************************************************************************************/
function doPost(e) {
  
  let parsedData;

  try {
    parsedData = JSON.parse(e.postData.contents);
  }
  catch (error) {
    // Logger.log("Error occured while trying to parse data")
    return ContentService.createTextOutput("Error in parsing request body: " + error.message);
  }

  let str = "Failed"
  if (parsedData !== undefined) {

    let sensorSheet = SS.getSheetByName(parsedData.sheet_name); 
    let dataArr = parsedData.values.split(",");

    switch (parsedData.command) {
      case "addHeader":
        // Logger.log(dataArr);
        dataArr.unshift('google_datetime');
        // Logger.log(dataArr);
        // Logger.log(dataArr.length);
        sensorSheet.getRange(1,2,1,3).setValues([['sample interval', 60, 'seconds']]); 
        sensorSheet.getRange(2,1,1,dataArr.length).setValues([dataArr]).setFontWeight("bold"); // put header in row 2
        str = "Success addHeader";
        SpreadsheetApp.flush();      
        break;

      case "appendRow":
        // Logger.log(dataArr);
        let datetime = new Date();
        // yyyy/mm/dd hh:mm:ss
        dformat = [datetime.getFullYear(), datetime.getMonth() + 1, datetime.getDate()].join('/') + ' ' + [datetime.getHours(), datetime.getMinutes(), datetime.getSeconds()].join(':');
        dataArr.unshift(dformat); 
        sensorSheet.appendRow(dataArr);
        str = "Success appendRow";
        SpreadsheetApp.flush();
        
        // Logger.log(SS.getId());
        // Logger.log(dataArr);
        //sendToNodeRedPost(dataArr.slice(0,5));
        break;

        default:
        //Logger.log("CSLP8266: Failed to proccess data.");    
    }
    
    return ContentService.createTextOutput(str);
  } // endif (parsedData !== undefined)
  else {
    //Logger.log("CSLP8266: Empty or incorrect fomat");
    return ContentService.createTextOutput("Error! Request body empty or in incorrect format.");
  }
}

/*************************************************************************************
  * The doGet(e) gets triggered when a client calls a get request on the AppScript 
  * using the Web App Url
 *************************************************************************************/
function doGet(e) {

  let val = e.parameter.value;
  let cal = e.parameter.cal;
  let read = e.parameter.read;

  let range = sheet.getRange('A1');
  let retval = range.getValue();
  // let retval = range.setValue(val).getValue();
  let now = Utilities.formatDate(new Date(), "EST", "yyyy-MM-dd'T'hh:mm a'Z'").slice(11, 19);
  sheet.getRange('B1').setValue(now);
  sheet.getRange('C1').setValue('123');
  Logger.log(e.parameter.value);
  Logger.log(retval);

  if (retval == e.parameter.value)
    return ContentService.createTextOutput("Successfully wrote " + e.parameter.value + " into spreadsheet.");
  else
    return ContentService.createTextOutput("Unable to write into spreadsheet.\nCheck authentication and make sure the cursor is not on cell 'A1'." + retval + ' ' + e.parameter.value);
}


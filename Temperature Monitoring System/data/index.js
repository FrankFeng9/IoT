
//js file for index.html
//load document
$(document).ready(() => {

    //set default tempurature format
    let tempFormat = "C";
    let currentTemp;

    //get the current tempature and the sensor id from the server
    $.get(`/sensor`, (data) => {   
            currentTemp = data.temperature;
            setTemperature(currentTemp, tempFormat);  
            $("#sensorId").text(data.address);   //display the sensor id   
    });
    

   // refresh every 5 seconds, get the temperature and update t
    setInterval(() => {
        $.get(`/sensor`, (data) => {

            if(tempFormat == "F"){ //if the current format is F, convert the temp to F
                currentTemp = data.temperature * 9/5 + 32;
            }
            else{
                currentTemp = data.temperature;
            }
            
            setTemperature(currentTemp.toFixed(2), tempFormat);
                 
        });
    }, 5000);

    
    /**
     * display the current time on the page
     */
    (function currentTime () {
        function checkTime(i) {
            return (i < 10) ? "0" + i : i;
        }
    
        function startTime() { 
            var today = new Date(),
                h = checkTime(today.getHours()),
                m = checkTime(today.getMinutes()),
                s = checkTime(today.getSeconds());
            document.getElementById('currentTime').innerHTML = h + ":" + m + ":" + s;
            t = setTimeout(function () {
                startTime()
            }, 500);
        }
        startTime();
    })();

    /**
     * change the tempature  between C and F when the button is clicked
     */
    function updateTemperature() {
        //if the current format is C, change to F
        if (tempFormat == "C") {
            tempFormat = "F";         
            currentTemp = ((currentTemp * 9 / 5) + 32).toFixed(2);
            setTemperature(currentTemp, tempFormat);
        } else {
            tempFormat = "C";
            currentTemp = ((currentTemp - 32) * 5 / 9).toFixed(2);
            setTemperature(currentTemp, tempFormat);
        }     
    }

    //display different color for different tempature
    function setTemperature(temp, tempFormat) {
        
        //current tempature format is celsius
        if (tempFormat == "C") {
            if (temp < 15) {
                $("#currentTemp").text(`${temp}°C`);
                $("#currentTemp").css("color", "AliceBlue" ); 
            }
            else if (temp >= 15 && temp < 25) {
                $("#currentTemp").text(`${temp}°C`);
                $("#currentTemp").css("color", "blue");
            }
            else if (temp >= 25 && temp < 30) {
                $("#currentTemp").text(`${temp}°C`);
                $("#currentTemp").css("color", "green");
            }
            else if (temp >= 30 && temp < 35) {
                $("#currentTemp").text(`${temp}°C`);
                $("#currentTemp").css("color", "orange");
            }
            else if (temp >= 35) {
                $("#currentTemp").text(`${temp}°C`);
                $("#currentTemp").css("color", "red");
            } 
        }
        //current tempature format is fahrenheit
        if (tempFormat == "F") {
            if (temp < 59) {
                $("#currentTemp").text(`${temp}°F`);
                $("#currentTemp").css("color", "AliceBlue");
            }
            else if (temp >= 59 && temp < 77) {
                $("#currentTemp").text(`${temp}°F`);
                $("#currentTemp").css("color", "blue");
            }
            else if (temp >= 77 && temp < 86) {
                $("#currentTemp").text(`${temp}°F`);
                $("#currentTemp").css("color", "green");
            }
            else if (temp >= 86 && temp < 95) {
                $("#currentTemp").text(`${temp}°F`);
                $("#currentTemp").css("color", "orange");
            }
            else if (temp >= 95) {
                $("#currentTemp").text(`${temp}°F`);
                $("#currentTemp").css("color", "red");
            }
        }
    
    }

     //call the updateTemperature function when the button is clicked
    $("#tempFormat").click(() => {

        updateTemperature();
       
    });


   




















});













    







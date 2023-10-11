const userTab = document.querySelector("[data-userWeather]"); // for fetching the custom HTML Attribute we need to use []
const searchTab = document.querySelector("[data-searchWeather]");
const userContainer = document.querySelector(".weather-container");

const grantAccessContainer = document.querySelector(".grant-location-container");
const serchForm = document.querySelector("[data-searchForm]");
const loadingScreen = document.querySelector(".loading-container")
const userInfoContainer = document.querySelector(".user-info-container")
const showError = document.querySelector(".show-error");
const btnSubmit = document.querySelector(".btnSubmit");

let currTab = userTab;
const API_KEY = "3e0c575db607da17d3572ff4c8a4f02e";
currTab.classList.add("current-tab");
getFromSessionStorage();

function swithTab(clickedTab)
{
    if(clickedTab != currTab)
    {   
        if(clickedTab === userTab)
        {
            showError.classList.remove("active");
        }
        
        currTab.classList.remove("current-tab");
        currTab = clickedTab;
        currTab.classList.add("current-tab");

        // Cliked on the serach form
        if(!serchForm.classList.contains("active"))
        {
            userInfoContainer?.classList.remove("active");
            grantAccessContainer?.classList.remove("active");
            serchForm.classList.add("active");

        }
        else
        {
            serchForm.classList.remove("active");
            userInfoContainer.classList.remove("active");
            getFromSessionStorage(); /*fetching the live loaction when we going to the remvoing active class from serchform */
        }
    }
}


btnSubmit.addEventListener("click" , ()=>{
    let cityName = serchInput.value;

    if(cityName === "")return;

    else fetchSearchUserWeatherInfo(cityName);
    showError.classList.remove("active");
})
     
userTab.addEventListener('click',()=>
{
    //pass the click tab
    swithTab(userTab);
});

searchTab.addEventListener('click',()=>
{
    //pass the click tab
    swithTab(searchTab);
});

//chek if the location-coardinate are present or not
function getFromSessionStorage()
{
    const localCoordinates = sessionStorage.getItem("user-coordinates");

    if(!localCoordinates)
    {
        /* current live loactin co-ordinates are not present In session storage then display the grant access screen */
        grantAccessContainer.classList.add("active");
        userInfoContainer.classList.remove("active");
    }
    else
    {
        const coordinates = JSON.parse(localCoordinates);
        fetchUserWeatherInfo(coordinates);
    }

}

/* as we got the co-ordinates so we are redy to fetch the weather data using that co-ordinates */
async function fetchUserWeatherInfo(coordinates)
{
    const {lat , lon} = coordinates;

    // Live loction  co-ordinates milalay remvoe the grant Access page
    grantAccessContainer.classList.remove("active");

    //make loader animation visible till we are fetching the data from the server
    loadingScreen.classList.add("active");
    
    
    //API CALL
    try
    {   
        //wating till we not get the data
        let response = await fetch(`https://api.openweathermap.org/data/2.5/weather?lat=${lat}&lon=${lon}&appid=${API_KEY}&units=metric`);
        const data = await response.json();
        
        // once we get the data from the above api call remove the loading animations
        loadingScreen.classList.remove("active");

        userInfoContainer.classList.add("active");

        //disply the received data on the screen
        renderWeatherInfo(data);


    }
    catch(err)
    {
        loadingScreen.classList.remove("active");
    }
}

function renderWeatherInfo(weatherInfo)
{
    //fetch the elements 
    const cityName = document.querySelector("[data-cityName]");
    const countyIcon = document.querySelector("[data-countryIcon]");
    const desc = document.querySelector("[data-weatherDesc]");
    const weatherIcon = document.querySelector("[data-weatherIcon]")
    const temp = document.querySelector("[data-temp]");
    const windSpeed = document.querySelector("[data-windSpeed]");
    const humidity = document.querySelector("[data-humidity]");
    const cloudiness = document.querySelector("[data-cloudiness]");

    // console.log(weatherInfo);

    cityName.innerText = weatherInfo?.name; 
    countyIcon.src =   `https://flagcdn.com/144x108/${weatherInfo?.sys?.country.toLowerCase()}.png`; // add the Coutyr flag image
    desc.innerText = weatherInfo?.weather?.[0]?.description;
    weatherIcon.src = `http://openweathermap.org/img/w/${weatherInfo?.weather?.[0]?.icon}.png`;; // weather icon
    temp.innerText = `${weatherInfo?.main?.temp} °C`;
    windSpeed.innerText = `${weatherInfo?.wind?.speed} m/s`;
    humidity.innerText = `${weatherInfo?.main?.humidity}%`;
    cloudiness.innerText = `${weatherInfo?.clouds?.all}%`;


}

function getLocation()
{
    if(navigator.geolocation)
    {
        navigator.geolocation.getCurrentPosition(showPosition);
    }

    else
    {
        alert("no geolocation supported");
    }
}

function showPosition(position)
{
    const userCoordinates = {
        lat : position.coords.latitude , 
        lon :  position.coords.longitude, 
    } 

    //storing the co-ordinates in session storage
    sessionStorage.setItem("user-coordinates" , JSON.stringify(userCoordinates));
    fetchUserWeatherInfo(userCoordinates);
}

// grante the location when we click the Grant access buttton
const grantAccessButton = document.querySelector("[data-grantAccess]");
grantAccessButton.addEventListener("click",getLocation );

let serchInput = document.querySelector("[data-serchInput]");

serchForm.addEventListener("submit" ,(e)=>
{
    e.preventDefault();
    let cityName = serchInput.value;

    if(cityName === "")return;

    else
    fetchSearchUserWeatherInfo(cityName);

    showError.classList.remove("active");

});


// fetching the user locatin using the city name
async function fetchSearchUserWeatherInfo(city)
{
    loadingScreen.classList.add("active");
    userInfoContainer?.classList.remove("active");
    grantAccessContainer?.classList.remove("active");

    try
    {
        const response =  await fetch(`https://api.openweathermap.org/data/2.5/weather?q=${city}&appid=${API_KEY}&units=metric`); //API call

        const data = await response.json();
        
        if(response.status === 404)
        {
            loadingScreen.classList.remove("active");   
            console.log(showError);
            showError.classList.add("active");
            console.log("error\n")
            return;
        }

        showError.classList.remove("active");
        loadingScreen.classList.remove("active");
        userInfoContainer.classList.add("active");

        console.log(data);
        renderWeatherInfo(data);
    }
    catch(er)
    {

    }
}
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Serialization;

namespace App2
{
   
    class OpenWeatherAPI
    {
        // make config for noted /config
        readonly string __appID = "5c6543a82eca14382fee79fbe187badf"; // /config

        readonly string __openWeatherURL = "http://api.openweathermap.org/data/2.5/"; // /config

        public Uri OpenWeatherUri;
        public OpenWeatherAPI()
        {
            OpenWeatherUri = new Uri(__openWeatherURL);
            AppID = __appID;
        }
        public string AppID { get; set; }

        // Methods

        public async Task<CurrentWeather> GetCurrentWeather(string city = "Kiev")
        {
            string wStr = OpenWeatherUri.AbsoluteUri.ToString() +
                        "weather?q=" + city +
                        "&mode=xml&units=metric&APPID=" + AppID;

            using (var client = new HttpClient())
            {
                HttpResponseMessage response = await client.GetAsync(wStr);

                if (!response.IsSuccessStatusCode)
                {
                    throw (new WException("!IsSuccessStatusCode"));
                }

                XmlSerializer xmlSer = new XmlSerializer(typeof(CurrentWeather));

                var responseStr = await response.Content.ReadAsStreamAsync();

                XmlReader xmlRead = XmlReader.Create(responseStr);

                if (xmlSer.CanDeserialize(xmlRead))
                {
                    return (CurrentWeather)xmlSer.Deserialize(xmlRead);
                }
                else
                {

                }
            }

            return null;
        }

        public async Task<Forecast> GetForecastWeather(string city = "Kiev")
        {
            string wStr = OpenWeatherUri.AbsoluteUri.ToString() +
                        "forecast/city?q=" + city +
                        "&mode=xml&units=metric&APPID=" + AppID;

            using (var client = new HttpClient())
            {
                HttpResponseMessage response = await client.GetAsync(wStr);

                if (!response.IsSuccessStatusCode)
                {
                    throw (new WException("!IsSuccessStatusCode"));
                }

                XmlSerializer xmlSer = new XmlSerializer(typeof(Forecast));

                var responseStr = await response.Content.ReadAsStreamAsync();

                XmlReader xmlRead = XmlReader.Create(responseStr);

                if (xmlSer.CanDeserialize(xmlRead))
                {
                    return (Forecast)xmlSer.Deserialize(xmlRead);
                }
                else
                {

                }
            }

            return null;
        }

        // exceptions

        public class WException : Exception
        {
            public WException(string message) : base(message)
            {
            }
        }

    }
}

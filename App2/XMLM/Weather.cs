using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace App2
{
    [XmlRoot("weatherdata", Namespace = "")]
    public sealed class Forecast : ForecastData
    {
    }

    public class ForecastData
    {
        [XmlArray("forecast")]
        [XmlArrayItem("time", Type = typeof(ForecastTime))]
        public ForecastTime[] Forecast { get; set; }
    }

    public sealed class ForecastTime
    {
        [XmlAttribute("from")]
        public DateTime From { get; set; }

        [XmlAttribute("to")]
        public DateTime To { get; set; }

        [XmlAttribute("day")]
        public DateTime Day { get; set; }

        [XmlElement("windSpeed")]
        public WindSpeed WindSpeed { get; set; }

        [XmlElement("temperature")]
        public Temperature Temperature { get; set; }

        [XmlElement("clouds")]
        public ForecastClouds Clouds { get; set; }
    }
    public sealed class ForecastClouds
    {

        [XmlAttribute("value")]
        public string Value { get; set; }

        [XmlAttribute("all")]
        public int All { get; set; }

        [XmlAttribute("unit")]
        public string Unit { get; set; }
    }


    public sealed class WindSpeed
    {

        [XmlAttribute("mps")]
        public double Mps { get; set; }

        [XmlAttribute("name")]
        public string Name { get; set; }
    }

    public sealed class Temperature
    {
        [XmlAttribute("value")]
        public double Value { get; set; }

        [XmlAttribute("min")]
        public double Min { get; set; }


        [XmlAttribute("max")]
        public double Max { get; set; }

        [XmlAttribute("unit")]
        public string Unit { get; set; }
    }

    public sealed class City
    {

        [XmlAttribute("id")]
        public int Id { get; set; }

        [XmlAttribute("name")]
        public string Name { get; set; }

    }
    public class WeatherItem
    {
        [XmlElement("city")]
        public City City { get; set; }

        [XmlElement("temperature")]
        public Temperature Temperature { get; set; }

        [XmlElement("clouds")]
        public Clouds Clouds { get; set; }

        [XmlElement("humidity")]
        public Humidity Humidity { get; set; }
    }

    public sealed class Humidity
    {

        [XmlAttribute("value")]
        public int Value { get; set; }

        [XmlAttribute("unit")]
        public string Unit { get; set; }
    }

    public sealed class Clouds
    {
        [XmlAttribute("value")]
        public int Value { get; set; }

        [XmlAttribute("name")]
        public string Name { get; set; }
    }

    [XmlRoot("current", Namespace = "")]
    public sealed class CurrentWeather : WeatherItem
    {
    }
}

using System;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;
using DXOverlay;
using Windows.UI;
using Windows.System.Threading;
using Windows.UI.Core;
using System.Diagnostics;
using Windows.Devices.Input;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace App2
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private readonly string localTimeStr = "Local Time ";
        forecastUI[] fui = new forecastUI[7];
        RendM renderer = new RendM();
        float[] pos = { 0, 0 };

        bool getOnce = false;

        ThreadPoolTimer PeriodicTimer;
        public MainPage()
        {
            this.InitializeComponent();

            cPane.Children.Add(renderer);

            MouseDevice mouseDevice = MouseDevice.GetForCurrentView();
            mouseDevice.MouseMoved += OnMouseMoved;

            renderer.InitRainDraw(50);

            CreateTimer();

            Task t = RefreshW();
        }

        void CreateTimer()
        {
            PeriodicTimer = ThreadPoolTimer.CreatePeriodicTimer(
                   async (timer) =>
                   {
                       //renderer.RenderRoutine();
                       await Dispatcher.RunAsync(
                           CoreDispatcherPriority.Low, () =>
                           {
                               renderer.RenderRoutine(pos[0], pos[1]);
                           });
                   },
                   TimeSpan.FromMilliseconds(20));
        }

        private async Task RefreshW(string city = "Kiev")
        {
           await RefreshCurrentWeather(city);
           await RefreshForecastWeather(city);
        }
        private async Task RefreshForecastWeather(string city = "Kiev")
        {
            OpenWeatherAPI ow = new OpenWeatherAPI();

            var we = await ow.GetForecastWeather(city);

            fGrid.Children.Clear();

            for (int i = 0; i < 7; i++)
            {
                fui[i] = new forecastUI();

                Grid.SetColumn(fui[i], 0);
                Grid.SetRow(fui[i], i);

                fui[i].fromTime = we.Forecast[i].From;
                fui[i].toTime = we.Forecast[i].To;

                fui[i].temperature = we.Forecast[i].Temperature.Value.ToString();
                fui[i].windSpeed = we.Forecast[i].WindSpeed.Name;

                fui[i].textState = we.Forecast[i].Clouds.Value;
                ChangeImage(we.Forecast[i].Clouds.Value, fui[i].imageBox);

                fGrid.Children.Add(fui[i]);   
            } 

            
        }

        private async Task RefreshCurrentWeather(string city = "Kiev")
        {
            OpenWeatherAPI ow = new OpenWeatherAPI();

            var we = await ow.GetCurrentWeather(city);
            
            string tempStr;

            if (we.Temperature.Unit == "metric")
                tempStr = " C ";
            else if (we.Temperature.Unit == "imperial")
                tempStr = " F ";
            else tempStr = " K ";

            time_box.Text = localTimeStr + DateTime.Now;

            clouds_box.Text = we.Clouds.Name;

            temp_box.Text = we.Temperature.Value.ToString() + tempStr;
            temp_box_max.Text = we.Temperature.Max.ToString() + tempStr;
            temp_box_min.Text = we.Temperature.Min.ToString() + tempStr;
            humidity_box.Text = we.Humidity.Value.ToString() + we.Humidity.Unit.ToString();

            ChangeImage(we.Clouds.Name, image);
        }

        static void ChangeImage(string type, Image img)
        {
            string file = "sun.png";

            switch (type)
            {
                case "clear sky":
                    file = "sun.png";
                    break;

                case "few clouds":
                    file = "cloudy.png";
                    break;

                case "overcast clouds":
                    file = "cloudy.png";
                    break;

                case "scattered clouds":
                    file = "cloudy-1.png";
                    break;

                case "broken clouds":
                    file = "cloudy-1.png";
                    break;

                case "shower rain":
                    file = "rainy.png";
                    break;

                case "rain":
                    file = "rain-1.png";
                    break;

                case "thunderstorm":
                    file = "storm.png";
                    break;

                case "snow":
                    file = "hail.png";
                    break;

                case "mist":
                    file = "wind.png";
                    break;
                default:
                    file = "sun.png";
                    break;
            }

            file = "Assets/" + file;

            BitmapImage bi = new BitmapImage(new Uri(img.BaseUri, file));
            img.Source = bi;
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {
          
        }

        private void Grid_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void comboBox_SelectionChanged_1(object sender, SelectionChangedEventArgs e)
        {
            string city = (sender as ComboBox).SelectedItem.ToString();
            textBlock.Text = "Today in " + city;
            Task m = RefreshW(city);
        }

        private void button_Click_1(object sender, RoutedEventArgs e)
        {
           
        }

        private void cPane_PointerMoved(object sender, Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
        {
            if (getOnce) return;
            Windows.UI.Input.PointerPoint pt = e.GetCurrentPoint(this);
            pos[0] = (float)pt.Position.X;
            pos[1] = (float)pt.Position.Y;
            getOnce = true;
        }

        private void OnMouseMoved(MouseDevice sender, MouseEventArgs args)
        {

            pos[0] += (float)args.MouseDelta.X;
            pos[1] += (float)args.MouseDelta.Y;
        }
    }
}

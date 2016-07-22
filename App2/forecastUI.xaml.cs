using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace App2
{
    public sealed partial class forecastUI : UserControl
    {
        public void ResetForm()
        {
            InitializeComponent();
        }

        public Image imageBox
        {
            set;
            get;
        }

        public DateTime fromTime
        {
            set { time_from_box.Text = "From: " + value.ToString(); }
        }

        public DateTime toTime
        {
            set { time_to_box.Text = "To: " + value.ToString();  }
        }

        public string windSpeed
        {  
            set { wind_box.Text = value; }
        }

        public string textState
        {
            set { text_state_box.Text = value; }
        }
        public string temperature
        {
            set { f_temp_box.Text = value; }
        }

        public forecastUI()
        {
            this.InitializeComponent();

            imageBox = state_image;
        }

        private void button_Click(object sender, RoutedEventArgs e)
        {

        }
    }
}

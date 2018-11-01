using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace GUI
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class StartUp : Window
    {
        public StartUp()
        {
            InitializeComponent();
            string exeDirectory = AppDomain.CurrentDomain.BaseDirectory;
            ImageBox.Source = new BitmapImage(new Uri(exeDirectory + "vidSplitLogo.png"));
        }

        String getOpenFileDialogResult(String DefaultExt, String Filter)// "" blank means it was cancelled.
        {
            Microsoft.Win32.OpenFileDialog openfile = new Microsoft.Win32.OpenFileDialog();
            openfile.DefaultExt = DefaultExt;
            openfile.Filter = Filter;

            Nullable<bool> result = openfile.ShowDialog();
            if (result == true)
                return openfile.FileName;
            else return "";
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {

            String fileName = getOpenFileDialogResult("*.MP4", "Audio/Video Files|*.WAV;*.MP4;*.MP3;*.MOV;*.AVI");
            if (fileName != "")
            {
                MainWindow win = new MainWindow(fileName);
                win.Show();
                try
                {
                    this.Close();
                }
                catch (Exception ex)
                {
                    System.Windows.MessageBox.Show(ex.ToString());
                }
            }
        }

    }
}

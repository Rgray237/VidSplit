using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using CLRWrapper;


namespace GUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow(string fileName)
        {
            try
            {
                InitializeComponent();
                filename = fileName;
                vidSplitter = new VidSplitWrapper(fileName);
                vidSplitter.writeOut("1");
                exeDirectory = vidSplitter.getInstallDirectory();//AppDomain.CurrentDomain.BaseDirectory;
                vidSplitter.writeOut("2");
                checkIfInstalledCorrectly();
                vidSplitter.writeOut("3");
                //showInstalledDirectory();
                enableAllButtons();
                vidSplitter.writeOut("4");
                disableCancelBtn();
                vidSplitter.writeOut("5");
                refreshWorkingDirectoryText();
                vidSplitter.writeOut("6");
                //createThumbnail();
                vidSplitter.writeOut("7");
                int lastInd = filename.LastIndexOf("\\");
                vidSplitter.writeOut("8");
                inFileNameTxtBox.Text = fileName;
                vidSplitter.writeOut("9");
                lastInd = fileName.LastIndexOf(".");
                vidSplitter.writeOut("10");
                inFileTypeTxtBox.Text = fileName.Substring(lastInd, fileName.Length - lastInd);
                vidSplitter.writeOut("11");

                inFileDurationTxtBox.Text = vidSplitter.getFileDuration();
                vidSplitter.writeOut(inFileDurationTxtBox.Text);
                ////System.Windows.MessageBox.Show(exeDirectory);
                inFileFullDuration = new TimeSpan(0, 0, (int)vidSplitter.getFileDurationSecs());
                vidSplitter.writeOut(inFileFullDuration.ToString());
                //System.Windows.MessageBox.Show("2");
                ImageBox.Source = new BitmapImage(new Uri(exeDirectory + "\\vidSplitLogo.png"));
                vidSplitter.writeOut("14");
                //System.Windows.MessageBox.Show("3");
            }
            catch (Exception e)
            {
                System.Windows.MessageBox.Show(e.ToString());
            }


        }

        ~MainWindow()
        {
            vidSplitter.writeOut("Disposing");
            vidSplitter.Dispose();
        }


        private void showInstalledDirectory()
        {
            System.Windows.MessageBox.Show(vidSplitter.getInstallDirectory());
        }

        private void checkIfInstalledCorrectly()
        {
            if (System.IO.Directory.Exists(exeDirectory + "\\ffmpeg"))
                return;
            else
                System.Windows.MessageBox.Show("VidSplit was installed incorrectly, or the required file structure was tampered with. \n\nUninstall the application and run the setup file again.");
            System.Windows.MessageBox.Show(exeDirectory + "\\ffmpeg" + " " + exeDirectory + "\\temp");
        }

        static int max_tracks = 99;
        CLRWrapper.VidSplitWrapper vidSplitter;

        string filename;
        string exeDirectory;
        int timeModeBoth = 0, timeModeStart = 1, timeModeDuration = 2;
        int timeMode;
        string thumbnailPath;
        bool startTimesEnabled = true, durationsEnabled = true;
        TimeSpan inFileFullDuration;
        List<track> trks = new List<track>(max_tracks);

        private void createThumbnail()
        {
            thumbnailPath = "";
            thumbnailPath = vidSplitter.createThumbnailImage();

            if (thumbnailPath != "")
            {
               //System.Windows.MessageBox.Show(thumbnailPath);
                BitmapImage bi = new BitmapImage();

                bi.BeginInit();

                bi.CacheOption = BitmapCacheOption.OnLoad;

                bi.UriSource = new Uri(thumbnailPath);

                bi.EndInit();
                imageBoxThumbnail.Source = bi;
            }

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

        private void setOutputPath()
        {
            System.Windows.Forms.FolderBrowserDialog fbd = new System.Windows.Forms.FolderBrowserDialog();

            System.Windows.Forms.DialogResult rslt = fbd.ShowDialog();

            if (rslt == System.Windows.Forms.DialogResult.OK)
            {
                vidSplitter.setWorkingDirectory(fbd.SelectedPath);
                refreshWorkingDirectoryText();
            }
        }

        private void disableAllButtons()
        {
            button2.IsEnabled = false;
            button3.IsEnabled = false;
            button4.IsEnabled = false;
            RdioMp3.IsEnabled = false;
            RdioMp4.IsEnabled = false;
        }


        private void enableAllButtons()
        {
            button2.IsEnabled = true;
            button3.IsEnabled = true;
            button4.IsEnabled = true;
            RdioMp3.IsEnabled = true;
            RdioMp4.IsEnabled = true;
        }


        private bool checkIfAllTracksAreValid()
        {
            bool goodSoFar = true;
            foreach (track trk in trks)
            {
                if (!trk.areAllTimesValid())
                {
                    goodSoFar = false;
                }
            }

            if (timeMode == timeModeStart)
            {
                if (trks.Count == 0 || trks[0].getStartTimeSpan().TotalSeconds > inFileFullDuration.TotalSeconds)
                    return false;

                trks[trks.Count - 1].setDurationGivenNextStartTime(inFileFullDuration);

                if (trks.Count > 1)
                    for (int i = trks.Count - 2; i >= 0; i--)
                    {
                        if (trks[i + 1].getStartTimeSecs() < trks[i].getStartTimeSecs() || !trks[i+1].areAllTimesValid())
                            return false;
                    }
            }

            if (timeMode == timeModeDuration)
            {
                if (trks.Count == 0)
                    return true;
                
                for (int i = 1; i < trks.Count; i++)
                {
                    trks[i].setStartTimeGivenPreviousTimes(trks[i - 1].getStartTimeSpan(), trks[i - 1].getDurationSpan());
                    if (trks[i].getDurationSpan() > inFileFullDuration || !trks[i].areAllTimesValid() || trks[i].getDurationSpan()+trks[i].getStartTimeSpan()>inFileFullDuration)
                        return false;
                }
            }

            return goodSoFar;
        }

        private void clearTrackErrors()
        {
            foreach (track trk in trks)
            {
                trk.removeErrorhighlights();
            }
        }

        private void clearTrackProgress()
        {
            foreach (track trk in trks)
            {
                trk.showClear();
            }
        }

        private void calculateTrackDurations()
        {
            if (trks.Count == 0 || trks[0].getStartTimeSpan().TotalSeconds>inFileFullDuration.TotalSeconds)
                return;

            trks[trks.Count - 1].setDurationGivenNextStartTime(inFileFullDuration);

            if (trks.Count > 1)
                for (int i = trks.Count - 2; i >= 0; i--)
                {
                    trks[i].setDurationGivenNextStartTime(trks[i + 1].getStartTimeSpan());
                }
        }

        private void calculateTrackStarts()
        {
            if (trks.Count == 0)
                return;
            if (trks.Count > 1)
            {
                for (int i = 1; i < trks.Count; i++)
                {
                    trks[i].setStartTimeGivenPreviousTimes(trks[i - 1].getStartTimeSpan(), trks[i - 1].getDurationSpan());
                }
            }
        }

        private async void createTracks()
        {

            if (checkIfAllTracksAreValid())
            {
                if (timeMode == timeModeStart)
                    calculateTrackDurations();
                if (timeMode == timeModeDuration)
                    calculateTrackStarts();
                if (ArtistTxt.Text != "")
                    vidSplitter.setArtistName(ArtistTxt.Text);

                disableAllButtons();
                enableCancelBtn();
                foreach (track trk in trks)
                {
                    bool shouldCreateTrack = true;
                    if (!vidSplitter.isTrackTimeValid(trk.getStartTime(), trk.getDuration()))
                    {
                        trk.messageTimeIsOutOfBounds(vidSplitter.getFileDuration());
                        shouldCreateTrack = false;
                    }
                    if (vidSplitter.doesFileExist(trk.name) && shouldCreateTrack)
                    {
                        shouldCreateTrack = trk.shouldReplaceExistingFile();
                    }
                    if (shouldCreateTrack)
                    {
                        trk.showLoading();
                        bool trackCreated = await Task.Run(() => vidSplitter.createTrack(trk.name, trk.number, trk.getStartTime(), trk.getDuration()));
                        if (trackCreated)
                            trk.showDone();
                        else
                            trk.showError();
                    }
                    else
                        trk.showError();
                }
            }

            else
            {
                if (timeMode == timeModeStart)
                    System.Windows.MessageBox.Show("Invalid Start Times.\nIn start time mode, each track must have a start time before the end of the input file and later than the previous track's start time.");
                if (timeMode == timeModeDuration)
                    System.Windows.MessageBox.Show("Invalid Durations.\nIn duration mode, the sum of all the durations must not exceed the total length of the input file.");
                System.Windows.MessageBox.Show("Invalid Start Times and/or Durations.\nDurations must be greater than zero, and a track's Start Time + its Duration must not exceed the total length of the input file.");

            }
            enableAllButtons();
        }

        private void listAddTrack()
        {
            if (vidSplitter != null)
            {
                track trk1 = new track(startTimesEnabled, durationsEnabled);
                int trknumber = listViewTracks.Items.Count + 1;
                trk1.setTrackNumber(trknumber);
                trks.Add(trk1);
                listViewTracks.ItemsSource = trks;
                listUpdateTracks();
            }
        }


        private void listRemoveSelectedTracks()
        {

            foreach (track itm in listViewTracks.SelectedItems)
            {
                trks.Remove(itm);
            }
            listUpdateTracks();
        }

        private void listUpdateTracks()
        {
            foreach (track trk in trks)
            {
                trk.setTrackNumber(trks.IndexOf(trk) + 1);
            }
            listViewTracks.Items.Refresh();

            //listPrint();
        }

        private void listPrint()
        {
            string str = "";
            foreach (track trk in trks)
            {
                str += trk.number + trk.hrs + "\n";
            }
            System.Windows.MessageBox.Show(str);
        }

        private void refreshWorkingDirectoryText()
        {
            OutputFolderText.Text = vidSplitter.getWorkingDirectory();
        }




        private void listViewTracks_KeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == System.Windows.Input.Key.Delete)
                listRemoveSelectedTracks();
        }

        private void button3_Click(object sender, RoutedEventArgs e)
        {
            listAddTrack();
        }

        private void button4_Click(object sender, RoutedEventArgs e)
        {
            clearTrackErrors();
            clearTrackProgress();
            createTracks();
        }

        private void disableCancelBtn()
        {
            CancelBtn.IsEnabled = false;
        }

        private void enableCancelBtn()
        {
            CancelBtn.IsEnabled = true;
        }


        private void button2_Click(object sender, RoutedEventArgs e)
        {
            setOutputPath();
        }

        private void setOutputMp3()
        {
            vidSplitter.setOutputFileType(vidSplitter.vsTypeMp3());
        }

        private void setOutputMp4()
        {
            vidSplitter.setOutputFileType(vidSplitter.vsTypeMp4());
        }

        private void setOutputMov()
        {
            vidSplitter.setOutputFileType(vidSplitter.vsTypeMov());
        }

        private void setOutputAVI()
        {
            vidSplitter.setOutputFileType(vidSplitter.vsTypeAVI());
        }

        private void RdioMp3_Click(object sender, RoutedEventArgs e)
        {
            setOutputMp3();
        }

        private void RdioMp4_Click(object sender, RoutedEventArgs e)
        {
            setOutputMp4();
        }

        private void RdioMov_Click(object sender, RoutedEventArgs e)
        {
            setOutputMov();
        }

        private void RdioAVI_Click(object sender, RoutedEventArgs e)
        {
            setOutputAVI();
        }


        private void enableStartTimes()
        {
            startTimesEnabled = true;
            for (int i = 0; i < trks.Count; i++)
            {
                trks[i].enableStartTimes();
            }
        }

        private void disableStartTimes()
        {
            startTimesEnabled = false;

            for (int i = 0; i < trks.Count; i++)
            {
                trks[i].disableStartTimes();
            }
        }

        private void enableDurations()
        {
            durationsEnabled = true;

            for (int i = 0; i < trks.Count; i++)
            {
                trks[i].enableDurations();
            }
        }

        private void disableDurations()
        {
            durationsEnabled = false;

            for (int i = 0; i < trks.Count; i++)
            {
                trks[i].disableDurations();
            }
        }
        private void clearAllTrackTimes()
        {
            for (int i = 0; i < trks.Count; i++)
            {
                trks[i].clearTrackTimes();
            }
        }

        private void setTimeMode(int timeMode)//0 -- start and duration, 1 -- start time only, 2 -- durations only
        {
            clearAllTrackTimes();
            this.timeMode = timeMode;
            switch (timeMode)
            {
                case 0:
                    enableStartTimes();
                    enableDurations();
                    break;
                case 1:
                    enableStartTimes();
                    disableDurations();
                    break;
                case 2:
                    disableStartTimes();
                    enableDurations();
                    break;
                default:
                    enableStartTimes();
                    enableDurations();
                    break;
            }
        }


        private void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            vidSplitter.cancelTrackCreation();
        }

        private void AboutBtn_Click(object sender, RoutedEventArgs e)
        {
            VideoSplitGUI.About abt = new VideoSplitGUI.About();
            abt.Show();
        }

        

        private void RdioSandD_Click(object sender, RoutedEventArgs e)
        {
            if (timeMode != timeModeBoth)
                setTimeMode(timeModeBoth);
        }

        private void RdioS_Click(object sender, RoutedEventArgs e)
        {
            if (timeMode != timeModeStart)
                setTimeMode(timeModeStart);
        }

        private void RdioD_Click(object sender, RoutedEventArgs e)
        {
            if (timeMode != timeModeDuration)
                setTimeMode(timeModeDuration);
        }
    }
}

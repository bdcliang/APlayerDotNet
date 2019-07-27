using APlayerDotNet;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        APlayerDotNet.APlayer player = null;
        public Form1()
        {
            InitializeComponent();

            player = new APlayerDotNet.APlayer();
            player.SetHwnd(pictureBox1.Handle);
            this.Text= player.GetVersion();
            //player.Mute();
            Bitmap bit =(Bitmap)Bitmap.FromFile(Environment.CurrentDirectory + "\\bg.jpg");
            player.SetCustomLogo(bit);
            
            player.MediaPlaying += Player_MediaPlaying;
            player.MediaReady += Player_MediaReady;
        }

        private void Player_MediaReady()
        {
            Console.WriteLine("=====================video end.....");
        }

        /// <summary>
        /// 
        /// </summary>
        private void Player_MediaPlaying()
        {
            Console.WriteLine("==========================================");
        }

        private void Player_MediaEvent(APlayerDotNet.PLAY_STATE _status)
        {
            //if(_status==PLAY_STATE)
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
          
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string file = @"C:\Users\Administrator.Delphi-PC\Videos\suzhou.mp4";
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "(媒体文件)|*.mp4;*.avi;*.rmvb;*.wmv";
            if(dlg.ShowDialog()==DialogResult.OK)
            {
                player.Play(dlg.FileName);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            player.SetAspectRatio(pictureBox1.Width, pictureBox1.Height);
        }
    }
}

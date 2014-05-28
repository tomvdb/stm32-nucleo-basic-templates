using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;
using System.Security.Cryptography;

namespace stmbootloaderapp
{
  public partial class mainFrm : Form
  {
    private byte[] firmware;
    private SerialPort port = new SerialPort();

    public mainFrm()
    {
      InitializeComponent();
    }

    private void prepBinary()
    {
      UInt32 sanityCheck = BitConverter.ToUInt32(firmware, 0x10C);

      if (sanityCheck != 0xDEADBEEF)
      {
        MessageBox.Show("Sanity Check Failed - 0xDEADBEEF");
        return;
      }

      UInt32 len = (uint)firmware.Length;
      byte[] firmwareLen = BitConverter.GetBytes(len);

      // insert firmware length into firmware
      firmware[0x10c + 4] = firmwareLen[0];
      firmware[0x10c + 5] = firmwareLen[1];
      firmware[0x10c + 6] = firmwareLen[2];
      firmware[0x10c + 7] = firmwareLen[3];

      // calc md5 sum
      MD5 md5 = MD5.Create();

      byte[] hash = md5.ComputeHash(firmware);

      string hashCode = "";

      for (int c = 0; c < hash.Length; c++)
        hashCode += hash[c].ToString("X2");

      //MessageBox.Show( hashCode );
      Array.Resize(ref firmware, (firmware.Length + hash.Length));

      // add md5 hash
      for (int c = 0; c < hash.Length; c++)
        firmware[len + c] = hash[c];
    }

    private void openToolStripMenuItem_Click(object sender, EventArgs e)
    {
      // get firmware file
      OpenFileDialog openDialog = new OpenFileDialog();

      if (openDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
      {
        string fileName = openDialog.FileName;

        // file exists?
        if (File.Exists(fileName))
        {
          BinaryReader firmwareFile = new BinaryReader(File.Open(fileName, FileMode.Open));
          firmware = firmwareFile.ReadBytes((int)firmwareFile.BaseStream.Length);
          firmwareFile.Close();

          // prepare binary for uploading
          prepBinary();
        }
        else
        {
          MessageBox.Show(fileName + " doesn't exist");
        }
      }
    }

    private void toolStripMenuItem2_Click(object sender, EventArgs e)
    {
      SaveFileDialog saveDialog = new SaveFileDialog();

      if (saveDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
      {
        string fileName = saveDialog.FileName;

        if (File.Exists(fileName))
        {
          if (MessageBox.Show("File already exists! Overwrite?", "Warning", MessageBoxButtons.YesNo) == System.Windows.Forms.DialogResult.No)
            return;
        }
        
        BinaryWriter modifiedFirmwareFile = new BinaryWriter(File.Open( fileName, FileMode.Create ));
        modifiedFirmwareFile.Write(firmware);
        modifiedFirmwareFile.Close();
      }
    }

    private byte[] makeFrame(byte cmd, byte len, byte[] data)
    {
      byte[] buffer = new byte[len+4];

      buffer[0] = 0x21;
      buffer[1] = 0x21;
      buffer[2] = cmd;
      buffer[3] = len;

      // add data
      for (int c = 0; c < len; c++)
      {
        buffer[4 + c] = data[c];
      }

      return buffer;
    }

    private void button1_Click(object sender, EventArgs e)
    {
      if (port.IsOpen)
        port.Close();

      try
      {
        port.BaudRate = 9600;
        port.StopBits = StopBits.One;
        port.DataBits = 8;
        port.Handshake = Handshake.None;
        port.PortName = "COM142";
        port.DataReceived += new SerialDataReceivedEventHandler(port_DataReceived);
        port.Open();
      
        debug("Connected");
      }
      catch (Exception Ex)
      {
        debug("Connect Failed: " + Ex.Message);
      }


    }


    private delegate void UpdateLBDelegate(Object obj);

    private  void debug(Object obj)
    {
      if (listBox1.InvokeRequired)
      {
        UpdateLBDelegate ulb = new UpdateLBDelegate(debug);
        listBox1.Invoke(ulb, new object[] { obj });
      }
      else
      {
        int i = listBox1.Items.Add(DateTime.Now.ToShortTimeString() + " : " + obj);
      }

    }

    void port_DataReceived(object sender, SerialDataReceivedEventArgs e)
    {
      //string data = port.ReadLine();
      byte[] buffer = new byte[255];

      int len = port.Read(buffer, 0, 255);

      string data = "";

      for (int c = 0; c < len; c++)
      {
        data += buffer[c].ToString("X2");
      }

      debug(data);
    }

    private void button2_Click(object sender, EventArgs e)
    {
      if (!port.IsOpen)
        return;

      byte[] data = makeFrame(1, 0, null);
      port.Write(data, 0, data.Length);
    }
  }
}

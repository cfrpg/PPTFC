﻿using System;
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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using System.IO.Ports;
using System.Threading;

namespace PPTGCS
{
	/// <summary>
	/// MainWindow.xaml 的交互逻辑
	/// </summary>
	public partial class MainWindow : Window
	{
		ObservableCollection<Parameter> parameters;		

		SerialPort port;
		byte[] inbuff = new byte[1024];
		byte[] outbuff = new byte[1024];
		bool portlock;	

		public MainWindow()
		{
			InitializeComponent();
			parameters = new ObservableCollection<Parameter>();
			parameters.Add(new Parameter() { ID = 0, Name = "reverse", Type = -1, Value = 0 });
			parameters.Add(new Parameter() { ID = 1, Name = "phi_p", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 2, Name = "phi_i", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 3, Name = "phi_d", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 4, Name = "yaw_p", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 5, Name = "aile_mix", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 6, Name = "elev_mix", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 7, Name = "phi_mix", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 8, Name = "yaw_scale", Type = 5, Value = 0 });
			paramListView.DataContext = parameters;
			getPorts();
			port = new SerialPort();
			port.BaudRate = 115200;
			port.DataReceived += Port_DataReceived;
		}

		private void Port_DataReceived(object sender, SerialDataReceivedEventArgs e)
		{
			if (portlock)
				return;
			portlock = true;
			int len = port.BytesToRead;
			port.Read(inbuff, 0, len);
			string str = Encoding.ASCII.GetString(inbuff, 0, len);
			Action a = () => { consoleText.Text += str; consoleText.ScrollToEnd(); };
		
			Dispatcher.Invoke(a);
			portlock = false;
		}

		private void UploadBtn_Click(object sender, RoutedEventArgs e)
		{
			if (!port.IsOpen)
				return;
			sendString(((Parameter)paramListView.SelectedItem).GetCmd());			
		}

		void getPorts()
		{
			portComboBox.Items.Clear();
			string[] ports = SerialPort.GetPortNames();
			foreach (var n in ports)
			{
				portComboBox.Items.Add(n);
			}
		}

		void sendString(string str)
		{
			if (!port.IsOpen)
				return;
			byte[] buf = Encoding.ASCII.GetBytes(str);
			port.Write(buf, 0, buf.Length);
		}

		private void RefreshBtn_Click(object sender, RoutedEventArgs e)
		{
			getPorts();
		}

		private void ConnectBtn_Click(object sender, RoutedEventArgs e)
		{
			if (port.IsOpen)
				port.Close();
			if (portComboBox.SelectedItem == null)
				return;
			port.PortName = portComboBox.SelectedItem.ToString();
			port.Open();
			sendString("*show/");	
		
		}

		private void ShowBtn_Click(object sender, RoutedEventArgs e)
		{
			sendString("*show/");
		}

		private void SendCmdBtn_Click(object sender, RoutedEventArgs e)
		{
			sendString("*"+cmdText.Text+"/");
		}

		bool tryReadParams(int cnt)
		{
			while(!readParams())
			{
				Thread.Sleep(10);
				cnt--;
				if (cnt <= 0)
					break;
			}
			if (cnt > 0)
				return true;
			return false;

		}

		bool readParams()
		{
			string str = consoleText.Text;
			int pos=str.LastIndexOf("SHOW CMD");
			if (pos < 0)
				return false;
			str = str.Substring(pos);
			pos = str.LastIndexOf("Param end.");
			if (pos < 0)
				return false;
			str = str.Substring(0, pos + 1);
			string[] lines = str.Split(("\r\n").ToCharArray());
			for (int i = 1; i < lines.Length - 1; i++)
			{
				pos = lines[i].IndexOf(":");
				if (pos < 0)
					continue;
				string num = lines[i].Substring(1, pos - 1);
				int id = int.Parse(num);
				pos = lines[i].LastIndexOf(":");
				num = lines[i].Substring(pos + 1, lines[i].Length - 1 - pos);
				float value = float.Parse(num);
				if (parameters[id].Type > 0)
					parameters[id].Value = value;
				else
					parameters[id].SetBinaryValue(value);
			}
			return true;
		}

		private void ReadoutBtn_Click(object sender, RoutedEventArgs e)
		{
			readParams();
		}
	}
}

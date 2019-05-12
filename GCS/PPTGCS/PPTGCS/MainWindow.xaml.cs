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
			parameters.Add(new Parameter() { ID = 0, Name = "Reverse", Type = 0, Value = 0 });
			parameters.Add(new Parameter() { ID = 1, Name = "phi_p", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 2, Name = "phi_1", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 3, Name = "phi_d", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 4, Name = "yaw_p", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 5, Name = "aile_mix", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 6, Name = "elev_mix", Type = 5, Value = 0 });
			parameters.Add(new Parameter() { ID = 7, Name = "phi_mix", Type = 5, Value = 0 });
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

		
	}
}

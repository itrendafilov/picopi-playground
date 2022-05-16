using System;
using System.Runtime.InteropServices;
using USBInterface;

namespace SimConnect_playground
{
    class HidCommunicationManager
    {
        private HidAPICalls hidAPI = new HidAPICalls();

        /* TI Specific VID/PID */
        public static ushort USB_VENDOR = 0x2047;
        public static ushort USB_PRODUCT = 0x03FC;
        public static byte TI_EXAMPLE_REPORT_ID = 0x3F;
        private bool is64bit = true;

        public int getNumberOfInterfaces()
        {
            return numDevices; //hidAPI.getNumberOfInterfaces(USB_VENDOR, USB_PRODUCT);
        }

        public string[] getSerials()
        {
            return hidAPI.getSerialsForVidPid(USB_VENDOR, USB_PRODUCT);
        }

        public int[] getInterfaces()
        {
            return new int[0];
        }

        private void disconnectDeviceNative(int handle)
        {

        }

        /* Management Data */
        private bool connected = false;
        private USBDevice connectedDevice = null;

        private DeviceScanner scanner;
        public HidCommunicationManager()
        {
            // setup a scanner before hand
            scanner = new DeviceScanner(USB_VENDOR, USB_PRODUCT, 2000);
            scanner.DeviceArrived += onEnter;
            scanner.DeviceRemoved += onExit;
            scanner.StartAsyncScan();
        }

        private int numDevices = 0;

        public void Dispose()
        {
            scanner.StopAsyncScan();
        }

        private void onHandle(object s, USBInterface.ReportEventArgs a)
        {
            //Console.WriteLine(string.Join(", ", a.Data));
        }

        private void onEnter(object s, EventArgs a)
        {
            numDevices++;
        }
        private void onExit(object s, EventArgs a)
        {
            numDevices--;
            // ToDo: Disconnect?
        }

        /**
		 * Open the handle to our HID device. Note there is no polling function
		 * currently that determines if our device is still connected.
		 * 
		 * @param vid
		 * @param pid
		 * @param serial
		 * @param infNumber
		 * @throws HidCommunicationException
		 */
        public void connectDevice(String serial, int infNumber)
        {
            connectedDevice = new USBDevice(USB_VENDOR, USB_PRODUCT, null, false, 31);

            // add handle for data read
            connectedDevice.InputReportArrivedEvent += onHandle;
            // after adding the handle start reading
            connectedDevice.StartAsyncRead();

            setConnected(true);
        }

        /**
         * 
         */
        public void disconnectDevice()
        {
            setConnected(false);
            connectedDevice.Dispose();
            connectedDevice = null;
        }

        /**
         * Wrapper function to accept String and send to native function as a byte
         * array. The HidAPI packet specifies that we need to send the ReportID
         * followed of the number of bytes to write, followed by the actual data to
         * write. Since we use one byte to represent the size in the HID packet, we
         * have to carve the string data into 252 byte packets and send them to the
         * device.
         * 
         * @param vid
         *            Vendor ID
         * @param pid
         *            Product ID
         * @param s
         *            String to send
         * @return Negative if the send failed, number of bytes sent if the
         *         operation went through
         * @throws HidCommunicationException
         */
        public uint sendData(String s)
        {

            uint res = 0;
            String temp = "";

            if (!isConnected())
                throw new HidCommunicationException(
                        "Not connected! Connect to device first!");

            /* We need to send the data to the JNI level at 253 byte chunks */
            while (s.Length > 252)
            {
                temp = s.Substring(0, 252);
                s = s.Substring(253, s.Length - 1);
                byte[] ta = new byte[255];
                ta[0] = TI_EXAMPLE_REPORT_ID;
                ta[1] = (byte)(ta.Length - 2);

                for (int i = 0; i < temp.Length; i++)
                {
                    ta[i + 2] = (byte)temp[i];
                }

                connectedDevice.Write(ta);
            }
            /* Prepare the packet */
            byte[] arr = new byte[s.Length + 2];
            arr[0] = TI_EXAMPLE_REPORT_ID;
            arr[1] = (byte)(arr.Length - 2);

            for (int i = 0; i < s.Length; i++)
            {
                arr[i + 2] = (byte)s[i];
            }

            //arr[arr.length - 1] = 0x0d;

            connectedDevice.Write(arr);

            return res;
        }

        /**
         * Wrapper function for the native receive data from HID device function. We
         * receive the information in a byte array, parse out how long the data is,
         * and then convert it into a standard Java string.
         * 
         * @param vid
         *            Vendor ID
         * @param pid
         *            Product ID
         * @return String value of whatever was read
         * @throws HidCommunicationException
         */
        public String receiveData()
        {

            if (!isConnected())
                throw new HidCommunicationException(
                        "Not connected! Connect to device first!");

            byte[] buf;
            String sBuf;

            /* Native function call */
            buf = connectedDevice.Read();

            if (buf.Length == 0) return "";

            if (buf == null)
                throw new HidCommunicationException(
                        "Unable to read buffer from HID device!");

            /* If the buffer is two bytes, there is no data to read */
            if (buf.Length == 1 && buf[0] == 0)
                return "";

            int length = buf[1];
            char[] parsedArr = new char[length];

            /* Extracting the relevant data */
            for (int i = 0; i < length; i++)
            {
                parsedArr[i] = (char)buf[i + 2];
            }
            sBuf = new String(parsedArr);

            return sBuf;
        }

        public bool isConnected()
        {
            return connected;
        }

        private void setConnected(bool connected)
        {
            this.connected = connected;
        }

        public class HidCommunicationException : Exception
        {
            public HidCommunicationException(String s) : base(s)
            {
            }
        }


    }
}

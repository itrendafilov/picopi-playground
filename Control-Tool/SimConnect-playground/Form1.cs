using Microsoft.FlightSimulator.SimConnect;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SimConnect_playground
{
    public partial class Form1 : Form
    {
        private const int WM_USER_SIMCONNECT = 0x402;
        private SimConnect simConnect = null;

        #region Enumerations

        public enum DATA_REQUESTS
        {
            REQUEST_1
        }

        public enum DEFINITIONS
        {
            PERIODIC,
            INIT
        }

        public enum EventEnum
        {
            AUTOPILOT_ON,
            AUTOPILOT_OFF,
            HEADING_BUG_INC,
            HEADING_BUG_DEC,
            AP_ALT_VAR_INC,
            AP_ALT_VAR_DEC,
            AP_ALT_HOLD_ON,
            AP_ALT_HOLD_OFF,
            AP_HDG_HOLD_ON,
            AP_HDG_HOLD_OFF,
            AUTO_THROTTLE_ARM,
            AP_APR_HOLD_ON,
            AP_APR_HOLD_OFF,
            AP_AIRSPEED_ON,
            AP_AIRSPEED_OFF,
            AP_SPD_VAR_INC,
            AP_SPD_VAR_DEC,
            AP_ALT_VAR_SET_ENGLISH,
            AP_VS_VAR_SET_ENGLISH,
            AP_SPD_VAR_SET,
            HEADING_BUG_SET,
            INC_COWL_FLAPS,
            DEC_COWL_FLAPS,
            APU_STARTER,
            APU_OFF_SWITCH,
            APU_GENERATOR_SWITCH_SET,
            FLAPS_UP,
            FLAPS_DOWN,
            FLAPS_1,
            FLAPS_2,
            FLAPS_3,
            FLAPS_SET,
            FLAPS_INCR,
            FLAPS_DECR,
            TOGGLE_MASTER_ALTERNATOR,
            ADF_SET,
            ADF_COMPLETE_SET,
            SPOILERS_ARM_ON,
            SPOILERS_ARM_OFF,
            SPOILERS_ON,
            SPOILERS_OFF,
            GEAR_UP,
            GEAR_DOWN,
            COM_STBY_RADIO_SET,
            COM_STBY_RADIO_SWAP,
            NAV1_STBY_SET,
            NAV1_RADIO_SWAP,
            ENGINE_AUTO_START,
            ENGINE_AUTO_SHUTDOWN,
            VOR1_SET,
            THROTTLE_SET,
            AXIS_LEFT_BRAKE_SET,
            AXIS_RIGHT_BRAKE_SET,
            BRAKES,
            ATC,
            ATC_MENU_1,
            ATC_MENU_2,
            ATC_MENU_3,
            ATC_MENU_4,
            ATC_MENU_5,
            ATC_MENU_6,
            ATC_MENU_7,
            ATC_MENU_8,
            ATC_MENU_9,
            ATC_MENU_0,
            AP_NAV1_HOLD_ON,
            AP_NAV1_HOLD_OFF,
            TOGGLE_GPS_DRIVES_NAV1,
            PARKING_BRAKES,
            TOGGLE_MASTER_BATTERY
        }

        public enum GROUP
        {
            GROUP1
        }

        #endregion Enumerations
        #region Nested Types

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct AviationData
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 0x100)]
            public string Title;

            //Position
            public double Plane_Latitude;
            public double Plane_Longitude;
            public double Plane_Altitude;
            public double Indicated_Altitude;
            public double Airspeed_Indicated;
            
            public double Plane_Heading_Degrees_Magnetic;
            public double Plane_Heading_Degrees_True;

            public double Kohlsman_Setting;

            //Plane orientation
            public double Plane_Pitch;
            public double Plane_Bank;
            public double Plane_Heading;

            //Radios
            public double Com_Active_Frequency_1;
            public double Com_Active_Frequency_2;
            public double Com_Standby_Frequency_1;
            public double Com_Standby_Frequency_2;
            public double Nav_Active_Frequency_1;
            public double Nav_Active_Frequency_2;
            public double Nav_Standby_Frequency_1;
            public double Nav_Standby_Frequency_2;

            //Auto Pilot
            public int Autopilot_Master;
            public int Autopilot_Approach_Hold;
            public int Autopilot_Heading_Lock;
            public double Autopilot_Heading_Lock_Dir;
            public int Autopilot_Altitude_Lock;
            public double Autopilot_Altitude_Lock_Var;
            public double Autopilot_Vertical_Hold_Var;
            public int Autopilot_Throttle_Arm;
            public int Autopilot_Airspeed_Hold;
            public double Autopilot_Airspeed_Hold_Var;
            public int Autopilot_Nav1_Lock;
            public int Gps_Drives_Nav1;

            //Engines
            public int Number_Of_Engines;
            public double General_Eng_Throttle_Lever_Position_1;
            public double General_Eng_Throttle_Lever_Position_2;
            public double General_Eng_Throttle_Lever_Position_3;
            public double General_Eng_Throttle_Lever_Position_4;

            //Other controls
            public double Flaps_Handle_Percent;
            public int Gear_Handle_Position;
            public int Spoilers_Armed;
            public double Spoilers_Handle_Position;
            public double Brake_Left_Position;
            public double Brake_Right_Position;

            //electrical
            public int APU_GENERATOR_SWITCH;
            public int APU_GENERATOR_ACTIVE;
            public int TOGGLE_MASTER_BATTERY;
        }

        public class commande
        {
            #region Properties

            public string cmd
            {
                get; set;
            }

            public string param
            {
                get; set;
            }

            #endregion Properties
        }

        public class commandes
        {
            #region Properties

            public List<commande> data
            {
                get; set;
            }

            #endregion Properties
        }

        static Object lockObj = new Object();
        #endregion Nested Types

        private HidCommunicationManager hidCM;
        static AviationData aviationData;

        public Form1()
        {
            InitializeComponent();
            pictureBox1.Image = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            this.hidCM = new HidCommunicationManager();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (hidCM.isConnected())
            {
                hidCM.disconnectDevice();
                hidCM.Dispose();   // ToDo: proper close
                hidCM = null;
            }
            if (simConnect != null)
            {
                closeConnection();
            }
        }

        // Simconnect client will send a win32 message when there is 
        // a packet to process. ReceiveMessage must be called to
        // trigger the events. This model keeps simconnect processing on the main thread.
        protected override void DefWndProc(ref Message m)
        {
            if (m.Msg == WM_USER_SIMCONNECT)
            {
                if (simConnect != null)
                {
                    try
                    {
                        simConnect.ReceiveMessage();
                    }
                    catch (COMException ce)
                    {
                        appendText("\r\nSIMCONNECT -> COM EXCEPTION\r\n");
                        // if this happens, try to re-connect
                        closeConnection();
                        Thread.Sleep(10);
                        initDataRequest();

                        errorsOccured++;
                        varErrors.Text = errorsOccured.ToString();
                    }
                }
            }
            else
            {
                base.DefWndProc(ref m);
            }
        }

        private void closeConnection()
        {
            if (simConnect != null)
            {
                // Dispose serves the same purpose as SimConnect_Close()
                simConnect.Dispose();
                simConnect = null;
                simStatus.Text = "Disconnected";
                appendText("\r\nSIMCONNECT -> Connection closed\r\n");
            }
        }

        // Set up all the SimConnect related data definitions and event handlers
        private void initDataRequest()
        {
            if (simConnect != null)
            {
                return;
            }

            try
            {

                // ToDo: Get more ideas from here:
                //https://github.com/harsharoramuj/fsxio/blob/master/fsxio/Form1.cs

                simConnect = new SimConnect("Flight Instrument Panel Bridge", base.Handle, 0x402, null, 0);

                simConnect.OnRecvOpen += new SimConnect.RecvOpenEventHandler(OnRecvOpen);
                simConnect.OnRecvException += new SimConnect.RecvExceptionEventHandler(OnRecvException);
                simConnect.OnRecvQuit += new SimConnect.RecvQuitEventHandler(OnRecvQuit);
                simConnect.OnRecvSimobjectData += new SimConnect.RecvSimobjectDataEventHandler(OnRecvSimobjectData);


                foreach (EventEnum item in Enum.GetValues(typeof(EventEnum)))
                {
                    simConnect.MapClientEventToSimEvent(item, item.ToString());
                }


                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Title", "", SIMCONNECT_DATATYPE.STRING256, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Plane Latitude", "degrees latitude", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Plane Longitude", "degrees longitude", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Plane Altitude", "feet", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Indicated Altitude", "feet", SIMCONNECT_DATATYPE.FLOAT64, 0.0f, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Airspeed Indicated", "knots", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Plane Heading Degrees Magnetic", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Plane Heading Degrees True", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Kohlsman setting hg", "inHg", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "PLANE PITCH DEGREES", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "PLANE BANK DEGREES", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "PLANE HEADING DEGREES MAGNETIC", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Com Active Frequency:1", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Com Active Frequency:2", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Com Standby Frequency:1", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Com Standby Frequency:2", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Nav Active Frequency:1", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Nav Active Frequency:2", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Nav Standby Frequency:1", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Nav Standby Frequency:2", "MHz", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Master", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Approach Hold", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Heading Lock", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Heading Lock Dir", "degrees", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Altitude Lock", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Altitude Lock Var", "feet", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Vertical Hold Var", "feet/minute", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Throttle Arm", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Airspeed Hold", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Airspeed Hold Var", "knots", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Autopilot Nav1 Lock", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Gps Drives Nav1", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Number Of Engines", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "General Eng Throttle Lever Position:1", "", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "General Eng Throttle Lever Position:2", "", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "General Eng Throttle Lever Position:3", "", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "General Eng Throttle Lever Position:4", "", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Flaps Handle Percent", "", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Gear Handle Position", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Spoilers Armed", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Spoilers Handle Position", "", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Brake Left Position", "", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "Brake Right Position", "", SIMCONNECT_DATATYPE.FLOAT64, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "APU GENERATOR SWITCH", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "APU_GENERATOR_ACTIVE", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);
                simConnect.AddToDataDefinition(DEFINITIONS.PERIODIC, "TOGGLE_MASTER_BATTERY", "", SIMCONNECT_DATATYPE.INT32, 0, SimConnect.SIMCONNECT_UNUSED);

                simConnect.RegisterDataDefineStruct<AviationData>(DEFINITIONS.PERIODIC);

                simConnect.RequestDataOnSimObject(DATA_REQUESTS.REQUEST_1, DEFINITIONS.PERIODIC, SimConnect.SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD.SIM_FRAME, 0, 0, 0, 0);

                timer1.Interval = 16; // ~ 60 FPS
                timer1.Enabled = true;

            }
            catch (Exception _)
            {
                simStatus.Text = "Unable to Connect";
            }
        }

        private void OnRecvOpen(SimConnect sender, SIMCONNECT_RECV_OPEN data)
        {
            appendText("\r\nSIMCONNECT -> Connected\r\n");
            simStatus.Text = "Connected";
        }

        private void OnRecvException(SimConnect sender, SIMCONNECT_RECV_EXCEPTION data)
        {
            Console.WriteLine("SimConnect error: " + data.dwException.ToString());
        }

        private void OnRecvQuit(SimConnect sender, SIMCONNECT_RECV data)
        {
            appendText("\r\nSIMCONNECT -> QuitEvent\r\n");
            closeConnection();
            timer1.Stop();
        }

        void OnRecvSimobjectDataBytype(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE data)
        {
            // not used at the moment
        }


        private int dataframesReceived = 0;
        private void OnRecvSimobjectData(SimConnect sender, SIMCONNECT_RECV_SIMOBJECT_DATA data)
        {
            lock (lockObj)
            {
                dataframesReceived++;
                try
                {
                    aviationData = (AviationData)data.dwData[0];
                }
                catch (Exception e)
                {
                    Console.WriteLine("OnRecvSimobjectData exception:" + e.Message);
                }
            }
        }

        String valKias = "";
        String valAltitude = "";
        String valBaro = "";
        String valPitch = "";
        String valBank = "";
        private static int timerTicks = 0;

        private int lastDataframesReceived = 0;
        private int errorsOccured = 0;

        private void timer1_Tick(object sender, EventArgs e)
        {
            lock (lockObj)
            {
                try
                {
                    timerTicks++;
                    if (timerTicks % 20 == 0)
                    {
                        timerTicks = 0;
                        varDataRate.Text = dataframesReceived.ToString();
                        dataframesReceived = 0;
                    }


                    String kias = (Math.Round(aviationData.Airspeed_Indicated, 1) * 10).ToString("0");
                    String altitude = Math.Round(aviationData.Indicated_Altitude, 0).ToString("0");
                    String baro = (Math.Round(aviationData.Kohlsman_Setting, 2) * 100).ToString("0");
                    String pitch = (Math.Round(-1 * aviationData.Plane_Pitch, 1) * 10).ToString("0");
                    String bank = (Math.Round(aviationData.Plane_Bank, 1) * 10).ToString("0");

                    if (0 != String.Compare(kias, valKias))
                    {
                        if (radioButton2.Checked)
                        {
                            sendCommand("PARAM KIAS " + kias + "!");
                        }
                        valKias = kias;
                        varKias.Text = kias;
                    }
                    if (0 != String.Compare(valAltitude, altitude))
                    {
                        if (radioButton2.Checked)
                        {
                            sendCommand("PARAM ALTITUDE " + altitude + "!");
                        }
                        valAltitude = altitude;
                        varAltitude.Text = altitude;
                    }
                    if (0 != String.Compare(valBaro, baro))
                    {
                        if (radioButton2.Checked)
                        {
                            sendCommand("PARAM BARO " + baro + "!");
                        }
                        valBaro = baro;
                        varBaro.Text = baro;
                    }
                    if (0 != String.Compare(valPitch, pitch))
                    {
                        if (radioButton1.Checked)
                        {
                            sendCommand("PARAM PITCH " + pitch + "!");
                        }
                        valPitch = pitch;
                        varPitch.Text = pitch;
                    }
                    if (0 != String.Compare(valBank, bank))
                    {
                        if (radioButton1.Checked)
                        {
                            sendCommand("PARAM BANK " + bank + "!");
                        }
                        valBank = bank;
                        varBank.Text = bank;
                    }
                } catch (Exception _)
                {

                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            errorsOccured = 0;
            varErrors.Text = errorsOccured.ToString();
            initDataRequest();
        }


        #region DrawLinesPlayground
        private void button2_Click(object sender, EventArgs e)
        {
            ((Bitmap)pictureBox1.Image).SetPixel(0, 0, Color.Blue);

            using (Graphics grf = Graphics.FromImage((Bitmap)pictureBox1.Image))
            {
                using (Brush brsh1 = new SolidBrush(Color.Yellow))
                {
                    using (Brush brsh2 = new SolidBrush(Color.White))
                    {
                        grf.FillEllipse(brsh1, 80, 40, 160, 160);
                        grf.FillEllipse(brsh2, 120, 80, 80, 80);
                    }
                }
            }

            ((Bitmap)pictureBox1.Image).SetPixel(160, 120, Color.Red);
            pictureBox1.Refresh();

            double rotate = Math.PI / 12, angle = rotate;
            while (angle < 2 * Math.PI)
            {
                drawThinLineAtAngle(angle);
                angle += rotate;
            }

        }

        private void drawThinLineAtAngle(double angle)
        {
            int x1 = (int)Math.Round(80.0 * Math.Cos(angle)) + 160;
            int y1 = (int)Math.Round(80.0 * Math.Sin(angle)) + 120;
            int x2 = (int)Math.Round(40.0 * Math.Cos(angle)) + 160;
            int y2 = (int)Math.Round(40.0 * Math.Sin(angle)) + 120;

            Bitmap bitmap = (Bitmap)pictureBox1.Image;
            bitmap.SetPixel(x1, y1, Color.Blue);
            bitmap.SetPixel(x2, y2, Color.Blue);
            pictureBox1.Refresh();

            drawLineBresenham(bitmap, x1, y1, x2, y2, Color.Red);
            pictureBox1.Refresh();

        }

        private void drawLineBresenham(Bitmap bitmap, int x1, int y1, int x2, int y2, Color color)
        {
            int dx = x2 - x1;
            int dy = y2 - y1;

            if (dy < 0)
            {
                dy = y1 - y2; // recalculate is probably faster than multiply by -1
                if (dx < 0)
                {
                    dx = x1 - x2; // recalculate is probably faster than multiply by -1
                    if (dx > dy) drawLineBresenhamByX(bitmap, x2, y2, dx, dy, color, true);
                    else drawLineBresenhamByY(bitmap, x2, y2, dx, dy, color, true);
                }
                else
                {
                    if (dx > dy) drawLineBresenhamByX(bitmap, x1, y1, dx, dy, color, false);
                    else drawLineBresenhamByY(bitmap, x2, y2, dx, dy, color, false);
                }
            }
            else
            {
                if (dx < 0)
                {
                    dx = x1 - x2; // recalculate is probably faster than multiply by -1
                    if (dx > dy) drawLineBresenhamByX(bitmap, x2, y2, dx, dy, color, false);
                    else drawLineBresenhamByY(bitmap, x1, y1, dx, dy, color, false);
                }
                else
                {
                    if (dx > dy) drawLineBresenhamByX(bitmap, x1, y1, dx, dy, color, true);
                    else drawLineBresenhamByY(bitmap, x1, y1, dx, dy, color, true);
                }
            }
        }

        private void drawLineBresenhamByX(Bitmap bitmap, int x1, int y1, int dx, int dy, Color color, bool incY)
        {
            int len = dx;

            // Draw a line in Quadrant I
            dy *= 2;  // Multiply dy by 2 to reduce calucaltions going forward
            int error = 0, threshold = dx - dy;
            dx *= 2;  // Multiply dy by 2 to reduce calucaltions going forward

            // We draw left to right until we move past right point
            while (len-- >= 0)
            {
                // we use x1 and y1 as iterators
                bitmap.SetPixel(x1++, y1, color);

                if (error > threshold) // error + 2*dy > dx (pre-calculated)
                {
                    // we move up or down based on sign
                    if (incY) y1++;
                    else y1--;

                    error -= dx; // error - 2*dx (pre-calculated)
                }
                error += dy;     // error + 2*dy (pre-calculated)
            }
        }

        private void drawLineBresenhamByY(Bitmap bitmap, int x1, int y1, int dx, int dy, Color color, bool incX)
        {
            int len = dy;

            // Draw a line in Quadrant I
            dx *= 2;  // Multiply dy by 2 to reduce calucaltions going forward
            int error = 0, threshold = dy - dx;
            dy *= 2;  // Multiply dy by 2 to reduce calucaltions going forward

            // We draw left to right until we move past right point
            while (len-- >= 0)
            {
                // we use x1 and y1 as iterators
                bitmap.SetPixel(x1, y1++, color);

                if (error > threshold) // error + 2*dx > dy (pre-calculated)
                {
                    // we move up or down based on sign
                    if (incX) x1++;
                    else x1--;

                    error -= dy; // error - 2*dy (pre-calculated)
                }
                error += dx;     // error + 2*dx (pre-calculated)
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            ((Bitmap)pictureBox1.Image).SetPixel(0, 0, Color.Blue);

            using (Graphics grf = Graphics.FromImage((Bitmap)pictureBox1.Image))
            {
                using (Brush brsh1 = new SolidBrush(Color.Yellow))
                {
                    using (Brush brsh2 = new SolidBrush(Color.White))
                    {
                        grf.FillEllipse(brsh1, 80, 40, 160, 160);
                        grf.FillEllipse(brsh2, 120, 80, 80, 80);
                    }
                }
            }

            ((Bitmap)pictureBox1.Image).SetPixel(160, 120, Color.Red);
            pictureBox1.Refresh();

            double rotate = Math.PI / 24, angle = 0;
            for (int i = 0; i < 48; i++)
            {
                drawThickLineAtAngle(angle);
                angle += rotate;
            }
        }

        private void drawThickLineAtAngle(double angle)
        {
            Bitmap bitmap = (Bitmap)pictureBox1.Image;

            float cos = (float)Math.Cos(angle);
            int x1 = (int)Math.Round(80.0 * Math.Cos(angle)) + 160;
            int y1 = (int)Math.Round(80.0 * Math.Sin(angle)) + 120;
            int x2 = (int)Math.Round(40.0 * Math.Cos(angle)) + 160;
            int y2 = (int)Math.Round(40.0 * Math.Sin(angle)) + 120;

/*
            int width = 5;
            angle -= Math.PI / 2;
            double rx1a = width * Math.Cos(angle) + x1;
            double ry1a = width * Math.Sin(angle) + y1;
            angle += Math.PI;
            double rx1b = width * Math.Cos(angle) + x1;
            double ry1b = width * Math.Sin(angle) + y1;
            bitmap.SetPixel((int)Math.Round(rx1a), (int)Math.Round(ry1a), Color.Cyan);
            bitmap.SetPixel((int)Math.Round(rx1b), (int)Math.Round(ry1b), Color.Cyan);
            int rdx = dx, rdy = dy;
            int tx1a, ty1a, tx1b, ty1b;

            if (incY)
            {
                tx1a = (int)Math.Round(rx1b);
                ty1a = (int)Math.Round(ry1b);
                tx1b = (int)Math.Round(rx1a);
                ty1b = (int)Math.Round(ry1a);
            }
            else
            {
                tx1a = (int)Math.Round(rx1a);
                ty1a = (int)Math.Round(ry1a);
                tx1b = (int)Math.Round(rx1b);
                ty1b = (int)Math.Round(ry1b);
            }*/

            drawLineThick(bitmap, x1, y1, x2, y2, 2, Color.Red);
            // Debug only, draw centerpoints
            bitmap.SetPixel(x1, y1, Color.Red);
            bitmap.SetPixel(x2, y2, Color.Red);
            pictureBox1.Refresh();

        }

        private void drawLineH(Bitmap bitmap, int x1, int x2, int y, Color color)
        {
            if (x1 > x2) throw new Exception("Bad input");

/*            Color old;
            old = bitmap.GetPixel(x1, y);
            if (Color.Cyan.ToArgb().Equals(old.ToArgb()))
                throw new Exception("Not drawing on left line");

            old = bitmap.GetPixel(x2, y);
            if (Color.Cyan.ToArgb().Equals(old.ToArgb()))
                throw new Exception("Not drawing on right line");*/

            while (x1 <= x2)
            {
/*                Color old = bitmap.GetPixel(x1, y);
                
                if (Color.DarkMagenta.ToArgb().Equals(old.ToArgb()) ||
                    Color.Blue.ToArgb().Equals(old.ToArgb()) ||
                    Color.Green.ToArgb().Equals(old.ToArgb()))
                {
                    throw new Exception("Duplicate draw of pixel");
                }*/
                bitmap.SetPixel(x1, y, color);
                x1++;
            }

            pictureBox1.Refresh();
        }

        private void drawLineV(Bitmap bitmap, int x, int y1, int y2, Color color)
        {
            if (y1 > y2) throw new Exception("Bad input");

            while (y1 <= y2)
            {
                bitmap.SetPixel(x, y1, color);
                y1++;
            }
        }

        private void drawLineThick(Bitmap bitmap, int x1, int y1, int x2, int y2, int width, Color color)
        {
            int dx = x2 - x1;
            int dy = y2 - y1;

            if (dy == 0)
            {
                // Special case - horizontal lines only
                int len = width * 2 + 1;
                y1 -= width;
                if (dx > 0)
                {
                    while (len-- > 0)
                    {
                        drawLineH(bitmap, x1, x2, y1, color);
                        y1++;
                    }
                } else
                {
                    while (len-- > 0)
                    {
                        drawLineH(bitmap, x2, x1, y1, color);
                        y1++;
                    }
                }

                return;
            }

            if (dx == 0)
            {
                // Special case - vertical lines only
                int len = width * 2 + 1;
                x1 -= width;
                if (dy > 0)
                {
                    while (len-- > 0)
                    {
                        drawLineV(bitmap, x1, y1, y2, color);
                        x1++;
                    }
                } else
                {
                    while (len-- > 0)
                    {
                        drawLineV(bitmap, x1, y2, y1, color);
                        x1++;
                    }
                }

                return;
            }

            if (dy < 0)
            {
                dy = y1 - y2; // recalculate is probably faster than multiply by -1
                if (dx < 0)
                {
                    dx = x1 - x2; // recalculate is probably faster than multiply by -1
                    if (dy > dx) drawLineThickByY(bitmap, x2, y2, dx, dy, width, color, true); 
                    else drawLineThickByX(bitmap, x2, y2, dx, dy, width, color, true);
                }
                else
                {
                    if (dy > dx) drawLineThickByY(bitmap, x2, y2, dx, dy, width, color, false); 
                    else drawLineThickByX(bitmap, x1, y1, dx, dy, width, color, false);
                }
            }
            else
            {
                if (dx < 0)
                {
                    dx = x1 - x2; // recalculate is probably faster than multiply by -1
                    if (dx > dy) drawLineThickByX(bitmap, x2, y2, dx, dy, width, color, false);
                    else drawLineThickByY(bitmap, x1, y1, dx, dy, width, color, false);
                }
                else
                {
                    if (dx > dy) drawLineThickByX(bitmap, x1, y1, dx, dy, width, color, true);
                    else drawLineThickByY(bitmap, x1, y1, dx, dy, width, color, true);
                }
            }
        }

        private void drawLineThickByX(Bitmap bitmap, int x1, int y1, int dx, int dy, int width, Color color, bool incY)
        {
            // Step 0: Get polar angle of the centerline  Debug only!
            double angle = Math.Atan((double)dy / (double)dx * (incY ? 1 : -1));
            angle -= Math.PI / 2;
            double rx1a = width * Math.Cos(angle) + x1;
            double ry1a = width * Math.Sin(angle) + y1;
            angle += Math.PI;
            double rx1b = width * Math.Cos(angle) + x1;
            double ry1b = width * Math.Sin(angle) + y1;
            bitmap.SetPixel((int)Math.Round(rx1a), (int)Math.Round(ry1a), Color.Cyan);
            bitmap.SetPixel((int)Math.Round(rx1b), (int)Math.Round(ry1b), Color.Cyan);
            int rdx = dx, rdy = dy;
            int tx1a, ty1a, tx1b, ty1b;

            if (incY)
            {
                tx1a = (int)Math.Round(rx1b);
                ty1a = (int)Math.Round(ry1b);
                tx1b = (int)Math.Round(rx1a);
                ty1b = (int)Math.Round(ry1a);
            } else
            {
                tx1a = (int)Math.Round(rx1a);
                ty1a = (int)Math.Round(ry1a);
                tx1b = (int)Math.Round(rx1b);
                ty1b = (int)Math.Round(ry1b);
            }

            // Step 1: Calculate perpendicular start points when rotated CW and CCW.
            //         Lines are parallel therefore endpoints are with same dx, dy.

            // reusing errora and errorb for w0 and w1 to save memory/register space
            int x1a, y1a, x1b, y1b, x1c, y1c, errora, errorb, errorc;
            // calculate w0 = dy/sqrt(dx^2 + dy^2)
            // ToDo: optimize 1/sqrt() performance with low-level calulcation hacks
            // https://en.wikipedia.org/wiki/Fast_inverse_square_root#Aliasing_to_an_integer_as_an_approximate_logarithm
            errora = (int)((width * dy) / Math.Sqrt(dy * dy + dx * dx));
            x1a = x1 - errora;
            x1b = x1 + errora;

            // calculate w1 = sqrt(4*width^2 - (x1a - x1b)^2)/2
            errorb = x1b - x1a; // calculate new coeff for y point
            errorb *= errorb; // (x1a - x1b)^2
            errorb = (int)(Math.Sqrt(4*width * width - errorb) / 2);

            // Compensate losses in integer math
            // errorb/2 > errora checks if Y errors are dominant
            if (errorb >> 1 > errora)
            {   // compensate rounding errors in Y
                errorb++;
            }
            else
            {   // apply rounding error correction in X retroactively
                x1a--;
                x1b++;
            }

            if (incY)
            {
                y1a = y1 + errorb;
                y1b = y1 - errorb;// - 1; // Compensate loss of fraction
            }
            else
            {
                y1a = y1 - errorb;// - 1; // Compensate loss of fraction
                y1b = y1 + errorb;
            }

            x1a = tx1a; y1a = ty1a;
            x1b = tx1b; y1b = ty1b;

            bitmap.SetPixel(x1a, y1a, Color.Pink);
            bitmap.SetPixel(x1b, y1b, Color.Pink);

            double realdist = Math.Sqrt(Math.Pow(Math.Round(rx1a) - Math.Round(rx1b), 2) + Math.Pow(Math.Round(ry1a) - Math.Round(ry1b), 2));
            double rounddist = Math.Sqrt(Math.Pow(tx1a - tx1b, 2) + Math.Pow(ty1a - ty1b, 2));
            double calcdist = Math.Sqrt(Math.Pow(x1a - x1b, 2) + Math.Pow(y1a - y1b, 2));

            pictureBox1.Refresh();

            // Step 2: We need to draw the start cap
            //         It closes a triangle with a third line from (x1a,y1a) to (x1b,y1b).
            int dxc = (x1a > x1b) ? x1a - x1b : x1b - x1a;
            int dyc = incY ? y1a - y1b : y1b - y1a;
            int lenc = dyc;

            x1c = x1b;
            y1c = y1b;

            // Debug only - draw lines
            int coeff = incY ? 1 : -1;

            drawLineBresenham(bitmap, x1a, y1a, x1a + dx, y1a + coeff * dy, Color.Cyan);
            drawLineBresenham(bitmap, x1b, y1b, x1b + dx, y1b + coeff * dy, Color.Cyan);
            drawLineBresenham(bitmap, x1a, y1a, x1b, y1b, Color.Cyan);
            drawLineBresenham(bitmap, x1a + dx, y1a + coeff * dy, x1b + dx, y1b + coeff * dy, Color.Cyan);

            // Draw line to screen.
            using (var graphics = Graphics.FromImage(bitmap))
            {
                Pen blackPen = new Pen(Color.Black, width*2 + 1);
                /*
                graphics.DrawLine(blackPen, x1, y1, x1 + dx, incY ? y1 + dy : y1 - dy);

                Pen redPen = new Pen(Color.Red, 1);
                graphics.DrawLine(redPen, tx1a, ty1a, tx1a + dx, ty1a + coeff * dy);
                graphics.DrawLine(redPen, tx1b, ty1b, tx1b + dx, ty1b + coeff * dy);
                graphics.DrawLine(redPen, tx1a, ty1a, tx1b, ty1b);
                graphics.DrawLine(redPen, tx1a + dx, ty1a + coeff * dy, tx1b + dx, ty1b + coeff * dy);

                graphics.DrawLine(redPen, x1a, y1a, x1a + dx, y1a + coeff * dy);
                graphics.DrawLine(redPen, x1b, y1b, x1b + dx, y1b + coeff * dy);
                graphics.DrawLine(redPen, x1a, y1a, x1b, y1b);
                graphics.DrawLine(redPen, x1a + dx, y1a + coeff * dy, x1b + dx, y1b + coeff * dy);
                */
            }


            pictureBox1.Refresh();
            drawFilledSquareByX(bitmap, x1a, y1a, x1b, y1b, dx, dy, color, incY);

            // Debug only - draw the outer box to validate: 
            /*            drawLineBresenham(bitmap, (int)rx1a, (int)ry1a, (int)rx1a + rdx, (int)ry1a + coeff * rdy, Color.Cyan);
                        drawLineBresenham(bitmap, (int)rx1b, (int)ry1b, (int)rx1b + rdx, (int)ry1b + coeff * rdy, Color.Cyan);
                        drawLineBresenham(bitmap, (int)rx1a, (int)ry1a, (int)rx1b, (int)ry1b, Color.Cyan);
                        drawLineBresenham(bitmap, (int)rx1a + rdx, (int)ry1a + coeff * rdy, (int)rx1b + rdx, (int)ry1b + coeff * rdy, Color.Cyan);
            */
        }

        private void drawLineThickByY(Bitmap bitmap, int x1, int y1, int dx, int dy, int width, Color color, bool incY)
        {

        }

        private void drawFilledSquareByX(Bitmap bitmap, int x1a, int y1a, int x1b, int y1b, int dx, int dy, Color color, bool incY)
        {
            int dxc = (x1a > x1b) ? x1a - x1b : x1b - x1a;
            int dyc = incY ? y1a - y1b : y1b - y1a;
            int lenc = dyc;

            int x1c = x1b, y1c = y1b;

            int lena = dx, lenb = dx; // ToDo: we may need to add +1 to both
            int errora, errorb, errorc;

            dy <<= 1;  // Multiply dy by 2 to reduce calucaltions going forward
            errora = dy - dx; // pre-load the threshold 2dy - dx as into errora and errorb
            errorb = errora;  // this is reducing to tests for positive error
            dx <<= 1;  // Multiply dy by 2 to reduce calucaltions going forward

            //         Same slope so reuse dxc and dyc for both start and close cap
            dxc <<= 1;  // Multiply dy by 2 to reduce calucaltions going forward
            errorc = dxc - dyc;
            dyc <<= 1;  // Multiply dy by 2 to reduce calucaltions going forward

            // Move all line coordinates and draw long horizontral lines 
            // filling the space between them

            // Check if we need to move X before we draw C to B
            if (errorb < 0) goto move_line_b;

        draw_from_c_to_b:
            // Draw a row from C to B
            drawLineH(bitmap, x1c, x1b, y1c, Color.Blue);

        //move_line_c: // We move C by Y so we always draw it
            if (incY) y1c++;
            else y1c--;

            if (errorc >= 0) // error + 2*dx >= dy (pre-calculated)
            {
                x1c--;
                errorc -= dyc; // error - 2*dy (pre-calculated)
            }
            errorc += dxc;     // error + 2*dx (pre-calculated)
            lenc--;
            if (lenc > 0)
            {
                if (y1c != y1b) goto move_line_b; // let Line B catch
                else goto draw_from_c_to_b;
            }
            else goto move_line_b; // no need to draw last C as it matches A

        move_line_b:
            x1b++;
            if (errorb > 0) // error + 2*dy > dx (pre-calculated)
            {
                // we move up or down based on sign
                if (incY) y1b++;
                else y1b--;

                errorb -= dx; // error - 2*dx (pre-calculated)
            }
            errorb += dy;     // error + 2*dy (pre-calculated)
            lenb--;
            if (lenb > 0)
            {
                if (errorb <= 0) goto move_line_b; // Loop till last X is reached

                if (lenc > 0)
                {
                    if (y1b == y1c) goto draw_from_c_to_b;
                    else goto move_line_b;
                }

                // When finished filling from C try to fill from B to A
                if (y1a == y1b) goto draw_line_from_b_to_a;
                else goto move_line_b;
            }
            else  // Last B matches with D so ommit double drawing
                goto initialize_line_d;

            draw_line_from_b_to_a: // ... and move A afterwards
            drawLineH(bitmap, x1a, x1b, y1a, Color.DarkMagenta);

        move_line_a:
            x1a++;
            if (errora > 0) // error + 2*dy > dx (pre-calculated)
            {
                // we move up or down based on sign
                if (incY) y1a++;
                else y1a--;

                errora -= dx; // error - 2*dx (pre-calculated)
            }
            errora += dy;     // error + 2*dy (pre-calculated)
            lena--;
            if (lena > 0)
            {
                if (lenb > 0 && y1a != y1b) goto move_line_b; // Let line B catch
                else if (lenb == 0 && y1c == y1a) goto draw_line_from_d_to_a; // move Line D
                else goto move_line_a;
            }
            else goto draw_line_from_d_to_a;  // Draw last line

            move_line_d:
            if (incY) y1c++;
            else y1c--;

            if (errorc > 0) // error + 2*dx > dy (pre-calculated)
            {
                x1c--;
                errorc -= dyc; // error - 2*dy (pre-calculated)
            }
            errorc += dxc;     // error + 2*dx (pre-calculated)
            lenc--;
            if (lena > 0) goto move_line_a;

            initialize_line_d:
            // Reuse line C variables for D
            // Close cap from ends of B and A. It is now currnt LineB + dxc,dyc
            lenc = dyc >> 1;   // Divide by 2 to restore the value to original
            errorc = dxc - lenc; // 2dxc-dyc (pre-calculated)

            x1c = x1b;
            y1c = y1b;

        draw_line_from_d_to_a:
            // We draw from D to A
            drawLineH(bitmap, x1a, x1c, y1c, Color.Green);
            if (lenc > 0) goto move_line_d;
        }
        #endregion DrawLinesPlayground

        private void button5_Click(object sender, EventArgs e)
        {
            comboBox1.Items.Clear();

            int num = hidCM.getNumberOfInterfaces();
            if (num <= 0) return; // Nothing to do more

            string[] serials;
            /* Calling our native functions and processing the results */
            serials = hidCM.getSerials();

            for (int i = 0; i < serials.Length; i++)
            {
                if (!serials[i].Equals(""))
                {
                    comboBox1.Items.Add(serials[i]);
                }
            }

            if (comboBox1.Items.Count > 0)
            {
                comboBox1.SelectedIndex = 0;
            }
        }

        BackgroundWorker receiveWorker;
        static Object logLockObj = new Object();

        delegate void appendTextCallback(string text);

        private void appendText(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.textBox2.InvokeRequired)
            {
                appendTextCallback d = new appendTextCallback(appendText);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                lock (logLockObj)
                {
                    if (this.textBox2.Text.Length > 1024)
                    {
                        this.textBox2.Text = text;
                    }
                    else
                    {
                        this.textBox2.Text += text;
                    }
                }
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            hidCM.connectDevice((string)comboBox1.SelectedItem, 0);
            // see https://docs.microsoft.com/en-us/dotnet/api/system.componentmodel.backgroundworker?redirectedfrom=MSDN&view=net-5.0
            receiveWorker = new BackgroundWorker();
            receiveWorker.DoWork +=
                new DoWorkEventHandler(receiveWorker_DoWork);
            receiveWorker.RunWorkerAsync();
        }

        private void button7_Click(object sender, EventArgs e)
        {
            lock (logLockObj)
            {
                // Clear text button
                textBox2.Text = "";
            }
        }

        private void sendCommand(String command)
        {
            appendText("\r\nTX -> " + command);
            if (!hidCM.isConnected()) return;
            hidCM.sendData(command);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            // Send message button
            sendCommand(textBox1.Text);
        }

        private void receiveWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            for(; ; )
            {
                if (!hidCM.isConnected()) return; // need to connect first
                                                  // Refresh received data
                String message = hidCM.receiveData();
                if ("".Equals(message)) return; // empty string, nothing to do

                appendText("\r\nRX -> " + message);
            }
        }

        private void testCmdSendOnce_Click(object sender, EventArgs e)
        {
            sendCommand(testCmdName.Text + " " + testCmdValue.Value + "!");
        }

        private void testCmdValue_ValueChanged(object sender, EventArgs e)
        {
            sendCommand(testCmdName.Text + " " + testCmdValue.Value + "!");
        }

        private void testCmdSendSweep_Click(object sender, EventArgs e)
        {
            testCmdSweepTimer.Enabled = false;
            if (!hidCM.isConnected()) return; // do nothing
            testCmdValue.Value = testCmdSweepFrom.Value;
            testCmdSweepTimer.Interval = (int)testCmdSweepPeriod.Value;
            testCmdSweepTimer.Enabled = true;
        }

        private void testCmdSweepTimer_Tick(object sender, EventArgs e)
        {
            if (!hidCM.isConnected())
            {
                testCmdSweepTimer.Enabled = false;
                return;
            }
            if (testCmdSweepFrom.Value < testCmdSweepTo.Value)
            {
                if (testCmdValue.Value >= testCmdSweepTo.Value)
                {
                    testCmdSweepTimer.Enabled = false;
                }
                else
                {
                    testCmdValue.Value += testCmdSweepStep.Value;
                }
            }
            else
            {
                if (testCmdValue.Value <= testCmdSweepTo.Value)
                {
                    testCmdSweepTimer.Enabled = false;
                }
                else
                {
                    testCmdValue.Value -= testCmdSweepStep.Value;
                }
            }
        }

        private void setSweepParams(int f, int t, int s, int p)
        {
            testCmdSweepTimer.Enabled = false;
            testCmdSweepFrom.Value = f;
            testCmdSweepTo.Value = t;
            testCmdSweepStep.Value = s;
            testCmdSweepPeriod.Value = p;
        }
        private void label8_DoubleClick(object sender, EventArgs e)
        {
            setSweepParams(0, 1000, 100, 1000);
        }

        private void label9_DoubleClick(object sender, EventArgs e)
        {
            setSweepParams(300, 900, 10, 250);
        }

        private void label11_DoubleClick(object sender, EventArgs e)
        {
            testCmdName.Text = label11.Text;
        }

        private void label10_DoubleClick(object sender, EventArgs e)
        {
            testCmdName.Text = label10.Text;
        }

        private void testCmdSweepStep_ValueChanged(object sender, EventArgs e)
        {
            testCmdValue.Increment = testCmdSweepStep.Value;
        }

        private void testCmdSweepFrom_ValueChanged(object sender, EventArgs e)
        {
            testCmdValue.Value = testCmdSweepFrom.Value;
        }
        private void label12_DoubleClick(object sender, EventArgs e)
        {
            setSweepParams(7000, 9000, 125, 1000);
        }

        private void label13_DoubleClick(object sender, EventArgs e)
        {
            setSweepParams(10000, 12000, 5, 100);
        }

        private void label14_DoubleClick(object sender, EventArgs e)
        {
            setSweepParams(15000, 13000, 10, 100);
        }

        private void label15_DoubleClick(object sender, EventArgs e)
        {
            setSweepParams(9800, 10200, 1, 40);
        }
        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton1.Checked)
            {
                sendCommand("INSTRUMENT HORIZON!");
            }
        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton2.Checked)
            {
                sendCommand("INSTRUMENT ALTISPEED!");
            }
        }

        private void radioButton3_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton3.Checked)
            {
                sendCommand("INSTRUMENT RADIO!");
            }
        }

        private void label16_DoubleClick(object sender, EventArgs e)
        {
            testCmdName.Text = label16.Text;
        }

        private void label18_DoubleClick(object sender, EventArgs e)
        {
            testCmdName.Text = label18.Text;
        }
    }
}
